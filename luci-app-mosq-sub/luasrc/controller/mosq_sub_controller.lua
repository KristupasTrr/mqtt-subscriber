module("luci.controller.mosq_sub_controller", package.seeall)

function index() 
    entry({"admin", "services", "mosq"}, cbi("mosq_sub_model", _("MOSQ Sub Messaging"), 125))
end