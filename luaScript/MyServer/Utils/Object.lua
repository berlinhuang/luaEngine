--
-- Created by IntelliJ IDEA.
-- User: root
-- Date: 18-2-2
-- Time: 上午9:34
-- To change this template use File | Settings | File Templates.
--
local   getmetatable, print, string, type, pairs =
        getmetatable, print, string, type, pairs

module( "Utils" )

-- 对象的基本信息
-------------------------------------------------------------------------------
function objinfo(obj)
    local meta = getmetatable(obj)
    if meta ~= nil then
        metainfo(meta)
    else
        print("no object infomation !!")
    end
end

function metainfo(meta)
    if meta ~= nil then
        local name = meta["__name"]
        if name ~= nil then
            metainfo(meta["__parent"])
            print("<"..name..">")
            -- 遍历对象的各个属性
            for key,value in pairs(meta) do
                if not string.find(key, "__..") then
                    if type(value) == "function" then
                        print("\t[f] "..name..":"..key.."()") -- [f] test:set()
                    elseif type(value) == "userdata" then
                        print("\t[v] "..name..":"..key) -- [v] test:_test
                    end
                end
            end
        end
    end
end