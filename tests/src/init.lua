local lib = require("test-lib")
local project = require("test-project")
local sum = require("sum")
local mod = require("mod")

lib.someMethod()

assert(project.value == 1234)

assert(sum(2, 2) == 4)
assert(mod(4, 2) == 0)