-- Bundled using <moonly>
-- Get moonly at <https://github.com/themusaigen/moonly>

-- Module <mod.lua> (src\mod.lua)
package.preload["mod"] = function(...)
return function(a, b)
  return a % b
end
end

-- Module <sum.lua> (src\sum.lua)
package.preload["sum"] = function(...)
return function (a, b)
  return a + b
end
end

-- Module <init.lua> (src\test-project\init.lua)
package.preload["test-project"] = function(...)
return {
  value = 1234
}

end

-- Module <module.lua> (src\test-project\module.lua)
package.preload["test-project.module"] = function(...)
return {
  something_secret = 1337
}
end

-- Module <init.lua> (src\test-project\submodule\init.lua)
package.preload["test-project.submodule"] = function(...)

end

-- Module <submodule.lua> (src\test-project\submodule\submodule.lua)
package.preload["test-project.submodule.submodule"] = function(...)

end

-- Module <init.lua> (lib\test-lib\init.lua)
package.preload["test-lib"] = function(...)
return {
  someMethod = function()
    print("Hello world")
  end
}

end

-- Core file <init.lua>
local lib = require("test-lib")
local project = require("test-project")
local sum = require("sum")
local mod = require("mod")

lib.someMethod()

assert(project.value == 1234)

assert(sum(2, 2) == 4)
assert(mod(4, 2) == 0)