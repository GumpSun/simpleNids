SimpleNids is a simple network traffic detection system based on libnids. Can effectively detect network packets, including: IP packets, IP fragment packets, UDP packets, TCP packets, HTTP packets

It can read data packets from specified network card devices or specified pcap for analysis, and the parsing result is JSON. Kafka can be configured to write analysis results to the topic.

Dependency on lib:
install libjson & libnids & librdkafka

Then compile:
$ make
Run:
$ ./ simpleNids -h
   
Analysis result:

IP packet:
{ "src": "x.x.x.x", "dst": "x.x.x.x", "total_length": 566, "protocol": 17, "packet_type": "ip", "time": 1460270272 }

IP fragmentation package:
{ "src": "x.x.x.x6", "dst": "x.x.x.x1", "total_length": 2632, "protocol": 17, "packet_type": "ip_frag", "time": 1460270253 }

UDP packet:
{ "src": "x.x.x.x:5763", "dst": "x.x.x.x:8080", "total_length": 1616, "data": "M-SEARCH * HTTP\/1.1\r\nMX: 50\r\nST: upnp:device\r\nMAN: \"ssdp:discover\"\r\nUser-Agent: curl /1.0.3.0\r\nConnection: close\r\nHost: x.x.x.x:8080\r\n\r\n", "packet_type": "udp", "time": 1460270562 }

HTTP packet:
{ "src": "x.x.x.x:590", "dst": "x.x.x.x:80", "request_method": "GET", "request_url": "test/annual.html? column=&subject=&nrnd=254515268&rnd=58420", "request_host": "trace.com", "request_connection": "keep-alive", "request_accept": "image\/webp,image\/*,*\/*; q=0.8", "request_user-agent": "Chrome\/49.0.2623.110 Safari\/537.36", "request_referer": "http:\/\/google.com", "request_accept-encoding": "gzip, deflate, sdch", "request_accept-language": "zh-CN,zh; q=0.8", "request_cookie": "RK=werxuimctwki;  cgv=73465965873;  login=3983202;  ptcz=3432432432432432;  guin=sa200111;  pgv=12312; ", "response_code": 200, "response_server": "nginx\/1.12.0", "response_date": "Fri, 01 Apr 2015 00:00:00 GMT", "response_content-type": "image\/gif", "response_content-length": "2", "response_connection": "keep-alive", "response_body": "01", "packet_type": "http", "time": 14602703475 }

TCP packet:
todo
---------------------------------------------------------------------------------------------

中文：
simpleNids是一款基于libnids编写的简易网络流量检测系统。 
可以有效检测网络数据包，包括: 
ip packets
ip fragment packets
udp packets
tcp packets 
http packets

既可以从指定网卡device也可以指定pcap读取数据包进行分析，解析结果为json。可以配置kafka将分析结果写入topic。

依赖lib:
install libjson & libnids & librdkafka

Then compile:
   $ make
Run:
   $ ./simpleNids -h
   
解析结果：

IP包：
{ "src": "x.x.x.x", "dst": "x.x.x.x", "total_length": 566, "protocol": 17, "packet_type": "ip", "time": 1460270272 }

IP碎片包：
{ "src": "x.x.x.x6", "dst": "x.x.x.x1", "total_length": 2632, "protocol": 17, "packet_type": "ip_frag", "time": 1460270253 }

UDP包：
{ "src": "x.x.x.x:5763", "dst": "x.x.x.x:8080", "total_length": 1616, "data": "M-SEARCH * HTTP\/1.1\r\nMX: 50\r\nST: upnp:device\r\nMAN: \"ssdp:discover\"\r\nUser-Agent: curl /1.0.3.0\r\nConnection: close\r\nHost: x.x.x.x:8080\r\n\r\n", "packet_type": "udp", "time": 1460270562 }

HTTP包：
{ "src": "x.x.x.x:590", "dst": "x.x.x.x:80", "request_method": "GET", "request_url": "test/annual.html?column=&subject=&nrnd=254515268&rnd=58420", "request_host": "trace.com", "request_connection": "keep-alive", "request_accept": "image\/webp,image\/*,*\/*;q=0.8", "request_user-agent": "Chrome\/49.0.2623.110 Safari\/537.36", "request_referer": "http:\/\/google.com", "request_accept-encoding": "gzip, deflate, sdch", "request_accept-language": "zh-CN,zh;q=0.8", "request_cookie": "RK=werxuimctwki; cgv=73465965873; login=3983202; ptcz=3432432432432432; guin=sa200111; pgv=12312;", "response_code": 200, "response_server": "nginx\/1.12.0", "response_date": "Fri, 01 Apr 2015 00:00:00 GMT", "response_content-type": "image\/gif", "response_content-length": "2", "response_connection": "keep-alive", "response_body": "01", "packet_type": "http", "time": 14602703475 }

TCP包：
todo
