#!/usr/bin/env coffee -b

parser = require("./parser")
runtime = require("./runtime")
interpreter = require("./interpreter")
fs = require("fs")
util = require("util")


input = fs.readFileSync("test.bc", 'utf-8')
funktions = parser.parse(input)
engine = new interpreter.Interpreter()
rt = new runtime.Runtime(engine, funktions)
rt.call('main', [])
