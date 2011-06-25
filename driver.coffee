#!/usr/bin/env coffee

parser = require("./parser")
interpreter = require("./interpreter")
fs = require("fs")
util = require("./util")


input = fs.readFileSync("test.bc", 'utf-8')
bytecodes = parser.parse(input)
console.log(bytecodes)
interpreter.interpret(bytecodes)
