map = Map("mosq-subscriber")

section = map:section(NamedSection, "mosq_sct", "mosqs", "Settings")

flag = section:option(Flag, "enable", "Enable", "Enable connection")

address = section:option(Value, "brokerhost", "IP address")
address.datatype = "ip4addr"
address.default = "192.168.2.1"

p = section:option(Value, "brokerport", "Port", "Host port")
p.datatype = "port"
p.default = "1883"

s = map:section(NamedSection, "mosq_sct_set", "mosqsm", "Messages");
s.template = "mosq-sub/messages"

return map