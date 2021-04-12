module("luci.controller.mosq_sub_controller", package.seeall)

function index() 
    entry({"admin", "services", "mosq"}, firstchild(), _("MOSQ Sub Messaging"), 125).dependent = false
    entry({"admin", "services", "mosq", "settings"}, cbi("mosq_sub_model_settings"), _("Settings"), 1)
    entry({"admin", "services", "mosq", "topics"}, cbi("mosq_sub_model_topics"), _("Topics"), 2)
    entry({"admin", "services", "mosq", "messages"}, cbi("mosq_sub_model_messages"), _("Messages"), 3)
end
