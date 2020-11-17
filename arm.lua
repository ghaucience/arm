-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local uci  = require "luci.model.uci".cursor()
local fs  = require "nixio.fs"
local os  = require "os"
local jsc = require "luci.jsonc"
httpx =require("socket.http")

function arm_ubus_send(attr, value)
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
                        		attribute =  attr,
                        		value = value 
				}
                        }           
                 }
        }                              
        luci.util.exec('ubus send DS.ARM \'' .. jsc.stringify(cmd) .. '\'')
end

--f = SimpleForm("Security Alarm", translate("Security Alarm"), translate("This page is for Security Setting"))
f = SimpleForm("Security Alarm", translate(""), translate(""))
f.reset = false

local armstr 	= fs.readfile("/tmp/test.json")
local armjson	= jsc.parse(armstr or '{}')


-- !! luci.util.dumptable(armjson)

-- !! Security Sences
--[[
                {
                        "enable" : 1,
                        "idx" : 1,
                        "name" : "KitchAlarm"
                }
--]]
uu = f:section(Table, armjson.sences, "Security Alarm", translate(""))

o = uu:option(DummyValue, "idx",	translate("Sence"))
function o.cfgvalue(self, section)
    if (armjson.sences[section].idx == nil) then                                                               
            return '-'
    end     
    return string.format("%d", armjson.sences[section].idx)
end

o = uu:option(DummyValue, "name",	translate("Name"))

o = uu:option(ListValue, "enable",	translate("ARM/DISARM"))
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
function o.cfgvalue(self, section)
    if (armjson.sences[section].enable == nil) then                                                               
            return '0'
    end     
    return string.format("%d", armjson.sences[section].enable)
end
function o.write(self, section, value)
    if (value == '0') then
	    io.stderr:write('disenable sence\n')
	    arm_ubus_send('arm.dab_sence',{idx = armjson.sences[section].idx})
    else
	    io.stderr:write('enable sence\n')
	    arm_ubus_send('arm.eab_sence',{idx = armjson.sences[section].idx})
    end
end

o = uu:option(Button, 	  "Delete",	translate("Delete Scene"))
o.inputstyle = "remove"
function o.write(self, section)
    io.stderr:write('sence deleted\n')
    arm_ubus_send('arm.del_sence',{idx = armjson.sences[section].idx})
end
	
-- !! Security Devices
--[[
                {
                        "action_idx" : 0,
                        "enable" : 1,
                        "mac" : "ec1bbdfffe8b1450",
                        "modelstr" : "Mose",
                        "sence_idx" : 1,
                        "trig_idx" : 1,
                        "type" : "1212"
                },
--]]
uu = f:section(Table, armjson.devices, "Devices", translate(""))
o = uu:option(DummyValue, "modelstr",	translate("Name"))

o = uu:option(DummyValue, "source",	translate("Wireless"))
function o.cfgvalue(self, section)
    if (armjson.devices[section].source == nil) then                                                               
            return 'Zigbee'
    end     
    return string.format("%s", armjson.devices[section].source)
end

o = uu:option(DummyValue, "mac",	translate("Mac address"))

o = uu:option(ListValue, "trig_idx",	translate("Triggering conditions"))
o:value("0", "None");
for k,v in ipairs(armjson.conds) do
    o:value(string.format("%d", v.idx), translate(v.name));
end
function o.cfgvalue(self, section)
    if (armjson.devices[section].trig_idx == nil) then                                                               
	return '0'
    end
    return string.format("%d", armjson.devices[section].trig_idx)
end
function o.write(self, section, value)
    io.stderr:write('trigger select\n')
    arm_ubus_send('arm.trg_device',{mac = armjson.devices[section].mac, trig_idx = value})
end


o = uu:option(ListValue, "action_idx",	translate("Actions"))
o:value("0", "None");
for k,v in ipairs(armjson.actions) do
    o:value(string.format("%d", v.idx), translate(v.name));
end
function o.cfgvalue(self, section)
    if (armjson.devices[section].action_idx == nil) then                                                               
	return '0'
    end
    return string.format("%d", armjson.devices[section].action_idx)
end
function o.write(self, section, value)
    io.stderr:write('action select\n')
    arm_ubus_send('arm.act_device',{mac = armjson.devices[section].mac, action_idx = value})
end

o = uu:option(ListValue, "Denable",	translate("Enable/Disable"))
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
function o.cfgvalue(self, section)
    if (armjson.devices[section].enable == nil) then                                                               
            return '0'
    end     
    return string.format("%d", armjson.devices[section].enable)
end
function o.write(self, section, value)
    io.stderr:write('devices enable/disable\n')
    if (value == '0') then
    	arm_ubus_send('arm.dab_device',{mac = armjson.devices[section].mac})
    else
    	arm_ubus_send('arm.eab_device',{mac = armjson.devices[section].mac})
    end
end

o = uu:option(ListValue, "sence_idx",	translate("Sence"))
for k,v in ipairs(armjson.sences) do
	o:value(v.idx);
end
function o.cfgvalue(self, section)
    return string.format("%d", armjson.devices[section].sence_idx)
end
function o.write(self, section, value)
    io.stderr:write('device sence select\n')
    arm_ubus_send('arm.grp_device',{mac = armjson.devices[section].mac, sence_idx = value})
end

return f
