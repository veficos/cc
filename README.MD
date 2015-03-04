# Overview
YunPian API for golang

# Usage
    func test() {
	    tpl := "【测试】#platform# 后台 #service# 出现问题，问题可能是：#info#，请尽快解决。"
        pattern := "#platform#=XX,#service#=派发服务,#info#=无法连接到[127.0.0.1:2000]"
        fmt.Println(YunPianSMSTPLSend("Your apikey", tpl, pattern, "Your mobile")
    }

