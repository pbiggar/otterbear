#!/usr/bin/env coffee -b

fs = require("fs")

parser = require("./parser")
builtins = require("./builtins")
runtime = require("./runtime")
interpreter = require("./interpreter")
compiler = require("./compiler")
util = require("./util")
var_dump = util.var_dump


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
llvm.BuildRet(builder, one)

[fail, ee, str] = llvm.CreateJITCompilerForModule(m, 0)
if fail
  console.log(str)
  llvm.VerifyFunction(f)
  llvm.DumpModule(m)
  process.exit(-1)

result = llvm.RunFunction(ee, f, [])
fres = llvm.GenericValueToFloat(floatType, result)
console.log(fres)

console.log("DONE")




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
