#!/usr/bin/env coffee -b

parser = require("./parser")
runtime = require("./runtime")
interpreter = require("./interpreter")
fs = require("fs")
util = require("util")


input = fs.readFileSync("test.bc", 'utf-8')
funktions = parser.parse(input)
util.inspect(funktions)
rt = new runtime.Runtime(funktions)
engine = new interpreter.Interpreter()
engine.execute(rt, funktions['main'])
