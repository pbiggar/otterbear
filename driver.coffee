#!/usr/bin/env coffee -b

fs = require("fs")

parser = require("./parser")
builtins = require("./builtins")
runtime = require("./runtime")
interpreter = require("./interpreter")
compiler = require("./compiler")
util = require("./util")
var_dump = util.var_dump


input = fs.readFileSync("test.bc", 'utf-8')

funktions = builtins.builtins
user_functions = parser.parse(input)
for k,v of user_functions
  funktions[k] = v

engine = new compiler.Compiler()
engine.compile(funktions)

#engine = new interpreter.Interpreter()
#rt = new runtime.Runtime(engine, funktions)
#rt.call('main', [])
