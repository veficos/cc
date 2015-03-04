package YunPianSMS

import (
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"net"
	"net/http"
	"net/url"
	"strings"
	"time"
)

const (
	YUNPIAN_BASE_URI    = "http://yunpian.com"
	YUNPIAN_SRV_VERSION = "v1"
	YUNPIAN_ENCODING    = "UTF-8"

	YUNPIAN_USER_GET_URL = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/user/get.json"
	YUNPIAN_USER_SET_URL = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/user/set.json"

	YUNPIAN_TPL_GET_DEFAULT_URL = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/tpl/get_default.json"
	YUNPIAN_TPL_ADD_URL         = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/tpl/add.json"
	YUNPIAN_TPL_DEL_URL         = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/tpl/del.json"
	YUNPIAN_TPL_GET_URL         = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/tpl/get.json"
	YUNPIAN_TPL_UPDATE_URL      = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/tpl/update.json"

	YUNPIAN_SMS_SEND_URL           = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/sms/send.json"
	YUNPIAN_SMS_PULL_STATUS_URL    = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/sms/pull_status.json"
	YUNPIAN_SMS_PULL_REPLY_URL     = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/sms/pull_reply.json"
	YUNPIAN_SMS_GET_BLACK_WORD_URL = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/sms/get_black_word.json"
	YUNPIAN_SMS_GET_REPLY_URL      = YUNPIAN_BASE_URI + "/" + YUNPIAN_SRV_VERSION + "/sms/get_reply.json"
)

type YunPianUserGetResponse struct {
	Code int    `json:"code"`
	Msg  string `json:"msg"`
	User struct {
		Nick             string      `json:"nick"`
		GmtCreated       string      `json:"gmt_created"`
		Mobile           string      `json:mobile`
		Email            string      `json:"email"`
		IPWhitelist      interface{} `json:"ip_whitelist"`
		ApiVersion       string      `json:"api_version"`
		Balance          int         `json:"balance"`
		AlarmBalance     int         `json:"alarm_balance"`
		EmergencyContact string      `json:"emergency_contact"`
		EmergencyMobile  string      `json:"emergency_mobile"`
	} `json:"user"`
}

type YunPianTplResponse struct {
	Code     int    `json:"code"`
	Msg      string `json:"msg"`
	Template struct {
		TplID       int    `json:"tpl_id"`
		TplContent  string `json:"tpl_content"`
		CheckStatus string `json:"check_status"`
		Reason      string `json:"reason"`
	} `json:"template"`
}

type YunPianTplResponses struct {
	Code     int    `json:"code"`
	Msg      string `json:"msg"`
	Template []struct {
		TplID       int    `json:"tpl_id"`
		TplContent  string `json:"tpl_content"`
		CheckStatus string `json:"check_status"`
		Reason      string `json:"reason"`
	} `json:"template"`
}

type YunPianSMSSendResponse struct {
	Code   int    `json:"code"`
	Msg    string `json:"msg"`
	Result struct {
		Count int `json:"count"`
		Fee   int `json:"fee"`
		Sid   int `json:"sid"`
	} `json:"result"`
}

type YunPianSMSPullStatusResponse struct {
	Code      int    `json:"code"`
	Msg       string `json:"msg"`
	SMSStatus []struct {
		Sid             int    `json:"sid"`
		Uid             int    `json:"uid"`
		UserReceiveTime string `json:"user_receive_time"`
		ErrorMsg        string `json:"error_msg"`
		Mobile          string `json:"mobile"`
		ReportStatus    string `json:"report_status"`
	} `json:"sms_status"`
}

type YunPianSMSPullReplyResponse struct {
	Code      int    `json:"code"`
	Msg       string `json:"msg"`
	SMSStatus []struct {
		Mobile     string `json:"mobile"`
		ReplyTime  string `json:"reply_time"`
		Text       string `json:"text"`
		Extend     string `json:"extend"`
		BaseExtend string `json:"base_extend"`
	} `json:"sms_reply"`
}

type YunPianSMSPullGetBlackWordResponse struct {
	Code   int    `json:"code"`
	Msg    string `json:"msg"`
	Result struct {
		BlackWord string `json:"black_word"`
	} `json:"result"`
}

type YunPianSMSGetReplyResponse struct {
	Code     int    `json:"code"`
	Msg      string `json:"msg"`
	SMSReply []struct {
		Mobile    string `json:"mobile"`
		Text      string `json:"text"`
		ReplyTime string `json:"reply_time"`
	} `json:"sms_reply"`
}

type YunPianResponse struct {
	Code   int    `json:"code"`
	Msg    string `json:"msg"`
	Detail string `json:"detail"`
}

func yunpianSendPost(url string, reader io.Reader) ([]byte, error) {
	reqest, err := http.NewRequest("POST", url, reader)
	if err != nil {
		return nil, err
	}

	client := http.Client{
		Transport: &http.Transport{
			Dial: func(netw, addr string) (net.Conn, error) {
				deadline := time.Now().Add(5 * time.Second)
				c, err := net.DialTimeout(netw, addr, time.Second*3)
				if err != nil {
					return nil, err
				}
				c.SetDeadline(deadline)
				return c, nil
			},
		},
	}

	// 发送post请求
	response, err := client.Do(reqest)
	if err != nil {
		if response != nil {
			defer response.Body.Close()
		}
		return nil, err
	}

	bodyByte, err := ioutil.ReadAll(response.Body)
	return bodyByte, err
}

func YunPianUserGet(apikey string) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)

	response, err := yunpianSendPost(YUNPIAN_USER_GET_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianUserGetResponse{}

	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	if result.Code != 0 {
		errResult := YunPianResponse{}
		err := json.Unmarshal(response, &errResult)
		if err != nil {
			return nil, err
		}
		return errResult, nil
	}

	return &result, nil
}

func YunPianUserSet(apikey string, emergency_contact string, emergency_mobile string, alarm_balance int) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)
	v.Add("emergency_contact", emergency_contact)
	v.Add("emergency_mobile", emergency_mobile)
	v.Add("alarm_balance", fmt.Sprintf("%d", alarm_balance))

	response, err := yunpianSendPost(YUNPIAN_USER_GET_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianResponse{}
	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	return result, err
}

func YunPianTplAdd(apikey string, tpl_content string, notify_node int64) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)
	v.Add("tpl_content", tpl_content)
	v.Add("notify_node", fmt.Sprintf("%ld", notify_node))

	response, err := yunpianSendPost(YUNPIAN_TPL_ADD_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianTplResponse{}

	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	if result.Code != 0 {
		errResult := YunPianResponse{}
		err := json.Unmarshal(response, &errResult)
		if err != nil {
			return nil, err
		}
		return errResult, nil
	}

	return &result, nil
}

func YunPianTplDel(apikey string, tpl_id int) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)
	v.Add("tpl_id", fmt.Sprintf("%d", tpl_id))

	response, err := yunpianSendPost(YUNPIAN_TPL_DEL_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianResponse{}
	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	return result, err
}

func YunPianTplUpdate(apikey string, tpl_id int64, tpl_content string) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)
	v.Add("tpl_id", fmt.Sprintf("%d", tpl_id))
	v.Add("tpl_content", tpl_content)

	response, err := yunpianSendPost(YUNPIAN_TPL_UPDATE_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianTplResponse{}

	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	if result.Code != 0 {
		errResult := YunPianResponse{}
		err := json.Unmarshal(response, &errResult)
		if err != nil {
			return nil, err
		}
		return errResult, nil
	}

	return &result, nil
}

func YunPianTplGet(apikey string, tpl_id ...int64) (interface{}, error) {

	v := url.Values{}
	v.Set("apikey", apikey)

	if len(tpl_id) != 0 {
		v.Add("tpl_id", fmt.Sprintf("%d", tpl_id[0]))
	}

	response, err := yunpianSendPost(YUNPIAN_TPL_GET_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	if len(tpl_id) != 0 {
		r := YunPianTplResponse{}
		err = json.Unmarshal(response, &r)
		if err != nil {
			return nil, err
		}

		if r.Code != 0 {
			errResult := YunPianResponse{}

			err := json.Unmarshal(response, &errResult)
			if err != nil {
				return nil, err
			}

			return errResult, nil

		}
		return r, nil

	} else {
		r := YunPianTplResponses{}
		err = json.Unmarshal(response, &r)
		if err != nil {
			return nil, err
		}

		if r.Code != 0 {
			errResult := YunPianResponse{}

			err := json.Unmarshal(response, &errResult)
			if err != nil {
				return nil, err
			}

			return errResult, nil
		}

		return r, nil
	}
}

func YunPianTplGetDefault(apikey string, tpl_id ...int64) (interface{}, error) {

	v := url.Values{}
	v.Set("apikey", apikey)

	if len(tpl_id) != 0 {
		v.Add("tpl_id", fmt.Sprintf("%d", tpl_id[0]))
	}

	response, err := yunpianSendPost(YUNPIAN_TPL_GET_DEFAULT_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	if len(tpl_id) != 0 {
		r := YunPianTplResponse{}
		err = json.Unmarshal(response, &r)
		if err != nil {
			return nil, err
		}

		if r.Code != 0 {
			errResult := YunPianResponse{}

			err := json.Unmarshal(response, &errResult)
			if err != nil {
				return nil, err
			}

			return errResult, nil

		}
		return r, nil

	} else {
		r := YunPianTplResponses{}
		err = json.Unmarshal(response, &r)
		if err != nil {
			return nil, err
		}

		if r.Code != 0 {
			errResult := YunPianResponse{}

			err := json.Unmarshal(response, &errResult)
			if err != nil {
				return nil, err
			}

			return errResult, nil
		}

		return r, nil
	}
}

func YunPianSMSSend(apikey string, text string, mobile string) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)
	v.Add("text", text)
	v.Add("mobile", mobile)

	response, err := yunpianSendPost(YUNPIAN_SMS_SEND_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianSMSSendResponse{}

	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	if result.Code != 0 {
		errResult := YunPianResponse{}
		err := json.Unmarshal(response, &errResult)
		if err != nil {
			return nil, err
		}
		return errResult, nil
	}

	return &result, nil
}

func YunPianSMSTPLSend(apikey string, tpl string, pattern string, mobile string) (interface{}, error) {

	parrerns := strings.Split(pattern, ",")

	for i := 0; i < len(parrerns); i++ {
		params := strings.Split(parrerns[i], "=")
		if len(params) != 2 {
			return nil, errors.New("Params pattern error.")
		}
		tpl = strings.Replace(tpl, params[0], params[1], -1)
	}

	return YunPianSMSSend(apikey, tpl, mobile)
}

func YunPianSMSPullStatus(apikey string, page_size ...int) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)

	if len(page_size) != 0 {
		v.Add("page_size", fmt.Sprintf("%d", page_size[0]))
	}

	response, err := yunpianSendPost(YUNPIAN_SMS_PULL_STATUS_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianSMSPullStatusResponse{}

	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	if result.Code != 0 {
		errResult := YunPianResponse{}
		err := json.Unmarshal(response, &errResult)
		if err != nil {
			return nil, err
		}
		return errResult, nil
	}

	return &result, nil
}

func YunPianSMSPullReply(apikey string, page_size ...int) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)

	if len(page_size) != 0 {
		v.Add("page_size", fmt.Sprintf("%d", page_size[0]))
	}

	response, err := yunpianSendPost(YUNPIAN_SMS_PULL_REPLY_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianSMSPullReplyResponse{}

	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	if result.Code != 0 {
		errResult := YunPianResponse{}
		err := json.Unmarshal(response, &errResult)
		if err != nil {
			return nil, err
		}
		return errResult, nil
	}

	return &result, nil
}

func YunPianSMSGetBlackWord(apikey string, text string) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)
	v.Add("text", text)

	response, err := yunpianSendPost(YUNPIAN_SMS_GET_BLACK_WORD_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianSMSPullGetBlackWordResponse{}

	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	if result.Code != 0 {
		errResult := YunPianResponse{}
		err := json.Unmarshal(response, &errResult)
		if err != nil {
			return nil, err
		}
		return errResult, nil
	}

	return &result, nil
}

func YunPianSMSGetReply(apikey string, start_time string, end_time string, page_num int, page_size int, args ...string) (interface{}, error) {
	v := url.Values{}
	v.Set("apikey", apikey)
	v.Add("start_time", start_time)
	v.Add("end_time", end_time)
	v.Add("page_num", fmt.Sprintf("%d", page_num))
	v.Add("page_size", fmt.Sprintf("%d", page_size))

	if len(args) == 1 {
		v.Add("mobile", args[0])
	}

	response, err := yunpianSendPost(YUNPIAN_SMS_GET_REPLY_URL, strings.NewReader(v.Encode()))
	if err != nil {
		return nil, err
	}

	result := YunPianSMSGetReplyResponse{}

	err = json.Unmarshal(response, &result)
	if err != nil {
		return nil, err
	}

	if result.Code != 0 {
		errResult := YunPianResponse{}
		err := json.Unmarshal(response, &errResult)
		if err != nil {
			return nil, err
		}
		return errResult, nil
	}

	return &result, nil
}

/*
func test() {
	tpl := "【测试】#platform# 后台 #service# 出现问题，问题可能是：#info#，请尽快解决。"
    pattern := "#platform#=XX,#service#=派发服务,#info#=无法连接到[127.0.0.1:2000]"
    fmt.Println(YunPianSMSTPLSend("Your apikey", tpl, pattern, "Your mobile")
}
*/
