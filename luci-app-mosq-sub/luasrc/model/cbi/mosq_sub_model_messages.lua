map = Map("mosq-subscriber")

s = map:section(NamedSection, "", "", "Messages");
s.template = "mosq-sub/messages"

return map