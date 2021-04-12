map = Map("mosq-subscriber")

section = map:section(NamedSection, "mosq_sct", "mosqs", "Settings")

flag = section:option(Flag, "enable", "Enable", "Enable connection")

address = section:option(Value, "brokerhost", "IP address")
address.datatype = "ip4addr"
address.default = "192.168.2.1"

p = section:option(Value, "brokerport", "Port", "Host port")
p.datatype = "port"
p.default = "1883"

FileUpload.size = "262144"                                  
FileUpload.sizetext = translate("Selected file is too large, max 256 KiB")
FileUpload.sizetextempty = translate("Selected file is empty")            
FileUpload.unsafeupload = true                                            
                                                                          
tls_enabled = section:option(Flag, "tls", translate("TLS"), translate("Select to enable TLS encryption"))
                                                                                                         
ca_file = section:option(FileUpload, "cafile", "CA File")                                                
ca_file:depends({tls = "1"})                                                                             
                                                                                                         
cert_file = section:option(FileUpload, "certfile", "CERT File")                                          
cert_file:depends({tls = "1"})                                                                           
                                                                                                         
key_file = section:option(FileUpload, "keyfile", "Key File")   
key_file:depends({tls = "1"})                              


return map