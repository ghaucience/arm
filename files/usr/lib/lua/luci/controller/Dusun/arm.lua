module("luci.controller.Dusun.arm", package.seeall)

function index()
    entry({"admin", "solution", "arm"}, cbi("admin_dusun/arm"), _("Security"), 60)
    entry({"admin", "solution", "arm", "add_sence"}, call("action_arm_add_sence"), nil).leaf = true 
end

function action_arm_add_sence()                                                                              
        local jsc = require "luci.jsonc"                                                                  
        name = luci.http.formvalue("sence_name") or 'SenceName'
        cmd={                                                                                            
          PKT = {                                                                                         
            to = 'ARM',
            from =  "CLOUD",                                                                                      
            type =  "cmd",                                                                            
            data =  {                                                                                   
                    id =  "xxxx",                                                                       
                    command =  "setAttribute",                                                        
                    arguments =  {                                                                                               
                            mac =  "01020304050607",                                                       
                            ep =  "0",                                                                       
                            attribute =  'arm.add_sence',                                                       
                            value = {                                                                     
                                name = name                                                               
                            }                                                                             
                   }                                                                                         
           }                                                                                                                
        }                                                                                                       
       }                                                                                                 
       luci.util.exec('ubus send DS.ARM \'' .. jsc.stringify(cmd) .. '\'')                                
       --luci.http.redirect(luci.dispatcher.build_url("admin/dusun/arm"))                               
end  
