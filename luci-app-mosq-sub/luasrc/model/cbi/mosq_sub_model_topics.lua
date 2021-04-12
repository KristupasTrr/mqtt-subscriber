map = Map("mosq-subscriber")

st = map:section(TypedSection, "topic", "Topics")
st.addremove = true
st.anonymous = true
st.novaluetext = translate("There are no topics created yet.")

topic = st:option(Value, "topic", translate("Topic"), translate(""))
topic.datatype = "string"
topic.maxlength = 65536
topic.rmempty = false
topic.parse = function(self, section, novld, ...)
	local value = self:formvalue(section)
	if value == nil or value == "" then
		self.map:error_msg(translate("Topic name can not be empty"))
		self.map.save = false
	end
	Value.parse(self, section, novld, ...)
end

direction = st:option(ListValue, "direction", translate("Direction"), translate("The direction that the messages will be shared in"))
direction:value("out", "OUT")
direction:value("in", "IN")
direction:value("both", "BOTH")
direction.default = "out"

qos = st:option(ListValue, "qos", translate("QoS level"), translate("The publish/subscribe QoS level used for this topic"))
qos:value("0", "At most once (0)")
qos:value("1", "At least once (1)")
qos:value("2", "Exactly once (2)")
qos.rmempty=false
qos.default="0"

return map