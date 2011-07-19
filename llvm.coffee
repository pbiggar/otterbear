# LLVM 
llvm = require("./3rdparty/node-llvm/build/default/llvm.node").LLVM()

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
  llvm.VerifyFunction(f, llvm.PrintMessageAction)
  llvm.DumpModule(m)
  process.exit(-1)

result = llvm.RunFunction(ee, f, [])
fres = llvm.GenericValueToFloat(floatType, result)
console.log(fres)

console.log("DONE")

exports.LLVM = class
  constructor: () ->

