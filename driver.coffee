#!/usr/bin/env coffee -b

parser = require("./parser")
builtins = require("./builtins")
runtime = require("./runtime")
interpreter = require("./interpreter")
compiler = require("./compiler")
util = require("./util")
var_dump = util.var_dump
fs = require("fs")

llvm = require("./3rdparty/node-llvm/build/default/llvm.node")

llvm = new llvm.LLVM()
console.log(llvm)

zero = llvm.doubleValue(0.0)
one = llvm.doubleValue(1.0)
two = llvm.doubleValue(2.0)

bb.return(one)

#pointer = func.functionPointer()
#pointer.call()
#
#three = llvm.add(one, two)
#six = llvm.add(three, three)
#five = llvm.subtract(six, one)
#eleven = llvm.add(six, five)

#sig  = llvm.signature(llvm.doubleType, [llvm.doubleType, llvm.doubleType])
#func = llvm.function(sig)
#bb = func.basicBlock()
#bb.return(eleven)

#pointer = func.functionPointer()
#pointer.call()




input = fs.readFileSync("test.bc", 'utf-8')

funktions = builtins.builtins
user_functions = parser.parse(input)
for k,v of user_functions
  funktions[k] = v

#engine = new interpreter.Interpreter()
#engine = new compiler.Compiler()
#engine.compile(funktions)
#rt = new runtime.Runtime(engine, funktions)

#rt.call('main', [])
