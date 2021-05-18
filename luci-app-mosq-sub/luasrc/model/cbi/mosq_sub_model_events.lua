map = Map("mosq-subscriber")

st = map:section(TypedSection, "event", "Events");
st.template = "mosq-sub/events"
st.addremove = true
st.anonymous = true
st.novaluetext = translate("There are no events created yet.")

topic = st:option(ListValue, "topic", "Topic", "The topic that expects event")
luci.model.uci.cursor():foreach("mosq-subscriber", "topic", function(s) topic:value(s.topic, s.topic) end)

account = st:option(ListValue, "account", "Account", "Sender account")
luci.model.uci.cursor():foreach("user_groups", "email", function(s) account:value(s.name, s.name) end)

key = st:option(Value, "key", "Key", "JSON data key")
key.datatype = "string"

valuetype = st:option(ListValue, "valuetype", "Value Type", "JSON data value type")
valuetype:value("1", "Decimal")
valuetype:value("2", "String")

comparisontype = st:option(ListValue, "comparisontype", "Comparison Type", "Comparison type to satisfy conditions")
comparisontype:value("1", "Equal")
comparisontype:value("2", "Not equal")
comparisontype:value("3", "Greater than")
comparisontype:value("4", "Greater or equal than")
comparisontype:value("5", "Less than")
comparisontype:value("6", "Less or equal than")

comparisonvalue = st:option(Value, "comparisonvalue", "Comparison Value", "Comparison value")
comparisonvalue.datatype = "string"

email = st:option(Value, "email", "Email", "Email to send notification")
email.datatype = "string"
email.maxlength = "256"


return map
