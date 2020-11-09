# simpleNids
simpleNids是一款基于libnids编写的简易网络流量检测系统。 可以有效检测网络数据包，包括: ip packets ip fragment packets tcp packets http packets udp packets 既可以从指定网卡device也可以指定pcap读取数据包进行分析，解析结果为json。可以配置kafka将分析结果写入topic。
