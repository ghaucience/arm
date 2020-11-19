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

a = SimpleForm("Security Alarm", translate(""), translate(""))

a.reset = false
a.submit = false
a.embedded = true

-- !! Add Sence
-- ------------------------------------------------------------------------------------------
uu = a:section(SimpleSection, "Add Alarm", translate(""));
o = uu:option(DummyValue, "Add", translate(""))
o.template = "admin_dusun/arm"

--f = SimpleForm("Security Alarm", translate("Security Alarm"), translate("This page is for Security Setting"))
f = SimpleForm("Security Alarm", translate(""), translate(""))
f.reset = false

--local armstr 	= fs.readfile("/tmp/test.json")
local armstr 	= fs.readfile("/etc/config/dusun/alarm/alarm.json")
local armjson	= jsc.parse(armstr or '{}')


-- !! luci.util.dumptable(armjson)




-- !! Security Sences
-- ------------------------------------------------------------------------------------------
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
	    armjson.sences[section].enable = value
    else
	    io.stderr:write('enable sence\n')
	    arm_ubus_send('arm.eab_sence',{idx = armjson.sences[section].idx})
	    armjson.sences[section].enable = value
    end
end

o = uu:option(Button, 	  "Delete",	translate("Delete"))
--o.inputstyle = "remove"
function o.write(self, section)
    io.stderr:write('sence deleted\n')
    arm_ubus_send('arm.del_sence',{idx = armjson.sences[section].idx})

		for k,v in ipairs(armjson.sendevs) do
			if (v.sence_idx == armjson.sences[section].idx) then                                                                                            
				armjson.sendevs[k].sence_idx = 0                                                                                                        
			end                                                                                                                                             
    end

    armjson.sences[section] = nil
end
	
-- !! Security Devices
-- ------------------------------------------------------------------------------------------
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
uu = f:section(Table, armjson.sendevs, "Sence Devices", translate(""))
o = uu:option(DummyValue, "xidx",	translate("Idx"))
function o.cfgvalue(self, section)
    return string.format("%s", armjson.sendevs[section].idx)
end

o = uu:option(DummyValue, "modelstr",	translate("Name"))

o = uu:option(DummyValue, "source",	translate("Wireless"))
function o.cfgvalue(self, section)
    if (armjson.sendevs[section].source == nil) then                                                               
            return 'Zigbee'
    end     
    return string.format("%s", armjson.sendevs[section].source)
end

o = uu:option(DummyValue, "mac",	translate("Mac address"))

o = uu:option(ListValue, "trig_idx",	translate("Triggering conditions"))
o:value("0", "None");
for k,v in ipairs(armjson.conds) do
    o:value(string.format("%d", v.idx), translate(v.name));
end
function o.cfgvalue(self, section)
    if (armjson.sendevs[section].trig_idx == nil) then                                                               
	return '0'
    end
    return string.format("%d", armjson.sendevs[section].trig_idx)
end
function o.write(self, section, value)
    io.stderr:write('trigger select\n')
    arm_ubus_send('arm.trg_vdevice',{idx = armjson.sendevs[section].idx, trig_idx = value})
    armjson.sendevs[section].trig_idx = value
end


o = uu:option(ListValue, "action_idx",	translate("Actions"))
o:value("0", "None");
for k,v in ipairs(armjson.actions) do
    o:value(string.format("%d", v.idx), translate(v.name));
end
function o.cfgvalue(self, section)
    if (armjson.sendevs[section].action_idx == nil) then                                                               
	return '0'
    end
    return string.format("%d", armjson.sendevs[section].action_idx)
end
function o.write(self, section, value)
    io.stderr:write('action select\n')
    arm_ubus_send('arm.act_vdevice',{idx = armjson.sendevs[section].idx, action_idx = value})
    armjson.sendevs[section].action_idx = value
end

o = uu:option(ListValue, "Denable",	translate("Enable/Disable"))
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
function o.cfgvalue(self, section)
    if (armjson.sendevs[section].enable == nil) then                                                               
            return '0'
    end     
    return string.format("%d", armjson.sendevs[section].enable)
end
function o.write(self, section, value)
    io.stderr:write('sendevs enable/disable\n')
    if (value == '0') then
        armjson.sendevs[section].enable = value
    	arm_ubus_send('arm.dab_vdevice',{idx = armjson.sendevs[section].idx})
    else
        armjson.sendevs[section].enable = value
    	arm_ubus_send('arm.eab_vdevice',{idx = armjson.sendevs[section].idx})
    end
end

o = uu:option(ListValue, "sence_idx",	translate("Sence"))
o:value("0")
for k,v in ipairs(armjson.sences) do
	o:value(v.idx);
end
function o.cfgvalue(self, section)
    return string.format("%d", armjson.sendevs[section].sence_idx)
end
function o.write(self, section, value)
    io.stderr:write('device sence select\n')
		local find = 0
		for k,v in ipairs(armjson.sences) do
			if (v.idx == tonumber(value)) then
				find = 1
			end
		end
		if (find == 1 or value == '0') then
			arm_ubus_send('arm.grp_vdevice',{idx = armjson.sendevs[section].idx, sence_idx = value})
			armjson.sendevs[section].sence_idx = value
		end
end

o = uu:option(Button, "XDelete",	translate("Delete"))
function o.write(self, section, value)
			arm_ubus_send('arm.del_vdevice',{idx = armjson.sendevs[section].idx})
			armjson.sendevs[section] = nil
end

--[[
--]]
-- !! Actrue Devices
-- ------------------------------------------------------------------------------------------
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
uu = f:section(Table, armjson.devices, "Sence Devices", translate(""))
o = uu:option(DummyValue, "modelstr",	translate("Name"))

o = uu:option(DummyValue, "source",	translate("Wireless"))
function o.cfgvalue(self, section)
    if (armjson.devices[section].source == nil) then                                                               
            return 'Zigbee'
    end     
    return string.format("%s", armjson.devices[section].source)
end

o = uu:option(DummyValue, "mac",	translate("Mac address"))

o = uu:option(ListValue, "a_trig_idx",	translate("Triggering conditions"))
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
    armjson.devices[section].trig_idx = value
end


o = uu:option(ListValue, "a_ction_idx",	translate("Actions"))
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
    armjson.devices[section].action_idx = value
end

o = uu:option(ListValue, "a_Denable",	translate("Enable/Disable"))
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
        armjson.devices[section].enable = value
    	arm_ubus_send('arm.dab_device',{mac = armjson.devices[section].mac})
    else
        armjson.devices[section].enable = value
    	arm_ubus_send('arm.eab_device',{mac = armjson.devices[section].mac})
    end
end

o = uu:option(ListValue, "a_sence_idx",	translate("Sence"))
o:value("0")
for k,v in ipairs(armjson.sences) do
	o:value(v.idx);
end
function o.cfgvalue(self, section)
    return string.format("%d", armjson.devices[section].sence_idx)
end
function o.write(self, section, value)
    io.stderr:write('device sence select\n')
		local find = 0
		for k,v in ipairs(armjson.sences) do
			if (v.idx == tonumber(value)) then
				find = 1
			end
		end
		if (find == 1 or value == '0') then
			arm_ubus_send('arm.grp_device',{mac = armjson.devices[section].mac, sence_idx = value})
			armjson.devices[section].sence_idx = value
		end
end

o = uu:option(Button, "Create",	translate("Create Sence Device"))
function o.write(self, section, value)
			asize = #armjson.sendevs
			local ix = 1
			for i=1,asize+1 do
				local find = 0
				for k,v in ipairs(armjson.sendevs) do
					if (armjson.sendevs[k].idx == i) then
						find = 1
						break
					end
				end
				if (find ~= 1) then
					ix = i
					break
				end
			end
			armjson.sendevs[asize+1] = armjson.devices[section]
			armjson.sendevs[asize+1].idx = ix
			arm_ubus_send('arm.add_vdevice',{mac = armjson.devices[section].mac})
end
--[[
--]]

return a, f
