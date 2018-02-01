--
-- Created by IntelliJ IDEA.
-- User: root
-- Date: 18-1-18
-- Time: 下午2:45
-- To change this template use File | Settings | File Templates.
--

local   pairs, print, type, string, tostring, getmetatable, setmetatable =
        pairs, print, type, string, tostring, getmetatable, setmetatable

module( "Utils" )


-- 打印表格
function printTable( t )
    local printCache={}
    local function subPrintTable(t,indent)
        if (printCache[tostring(t)]) then
            print(indent.."*"..tostring(t))
        else
            printCache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
--                        pos=tostring(pos)
                        print(indent.."["..pos.."] => "..tostring(t).." {")
                        subPrintTable(val,indent..string.rep(" ",string.len(pos)+8))
                        print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        print(indent.."["..pos..'] => "'..val..'"')
                    else
                        print(indent.."["..pos.."] => "..tostring(val))
                    end
                end
            else
                print(indent..tostring(t))
            end
        end
    end

    if (type(t)=="table") then
        print(tostring(t).." {")
        subPrintTable(t,"  ")
        print("}")
    else
        subPrintTable(t,"  ")
    end
    print()
end


-- 深度拷贝
function deepCopy(object)
    local searchTable = {}

    local function func(object)
        if type(object) ~= "table" then
            return object
        end
        local newTable = {}
        searchTable[object] = newTable
        for k, v in pairs(object) do
            newTable[func(k)] = func(v)
        end
        return setmetatable(newTable, getmetatable(object))
    end

    return func(object)
end



-- 按key排序
function pairsByKeys(t, desc)
    local a={}
    for n in pairs(t) do --key
        table.insert(a,n)
    end
    desc = desc or false
    local function f(a,b)
        if desc then
            return a > b --降序
        else
            return a < b  --升序
        end
    end
    table.sort(a,f,desc)
    local i =0
    local iter=function()
        i=i+1
        if a[i] ==nil then
            return nil
        else
            return a[i],t[a[i]]
        end
    end
    return iter --调用函数 iter
end



