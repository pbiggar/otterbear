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
cx = llvm.GetGlobalContext()
builder = llvm.CreateBuilderInContext(cx)
floatType = llvm.DoubleTypeInContext(cx)
zero = llvm.ConstReal(floatType, 0.0)
one = llvm.ConstReal(floatType, 1.0)
two = llvm.ConstReal(floatType, 2.0)
m = llvm.ModuleCreateWithNameInContext("test app", cx)
ft =  llvm.FunctionType(floatType, [floatType, floatType], false)
f =  llvm.AddFunction(m, 'main', ft)
bb = llvm.AppendBasicBlockInContext(cx, f, 'entry')
llvm.PositionBuilderAtEnd(builder, bb)
llvm.BuildRet(builder, zero)
[success, ee, str] = llvm.CreateJITCompilerForModule(m, 0)
console.log(success)
console.log(ee)
console.log(str)
if !success
  sys.exit(str)
else
  llvm.RunFunction(ee, f, [])

console.log(llvm)

llvm.VerifyFunction(f)
llvm.DumpModule(m)

console.log("DONE")



#pointer = func.functionPointer()
#pointer.call()
#
#three = llvm.add(one, two)
#six = llvm.add(three, three)
#five = llvm.subtract(six, one)
#eleven = llvm.add(six, five)

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
