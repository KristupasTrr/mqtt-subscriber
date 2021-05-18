module("luci.controller.mosq_sub_controller", package.seeall)

function index()

    entry({"admin", "services", "mqtt", "subscriber"}, firstchild(), _("Subscriber"), 3)
    entry({"admin", "services", "mqtt", "subscriber", "settings"}, cbi("mosq_sub_model_settings"), _("Settings"), 1).leaf=true
    entry({"admin", "services", "mqtt", "subscriber", "topics"}, cbi("mosq_sub_model_topics"), _("Topics"), 2).leaf=true
    entry({"admin", "services", "mqtt", "subscriber", "messages"}, cbi("mosq_sub_model_messages"), _("Messages"), 3).leaf=true
    entry({"admin", "services", "mqtt", "subscriber", "events"}, cbi("mosq_sub_model_events"), _("Events"), 4).leaf=true

end
