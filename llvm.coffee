internal = require("./3rdparty/node-llvm/build/default/llvm.node")

internal = new internal.LLVM()
cx = internal.GetGlobalContext()
builder = internal.CreateBuilderInContext(cx)
floatType = internal.DoubleTypeInContext(cx)
zero = internal.ConstReal(floatType, 0.0)
one = internal.ConstReal(floatType, 1.0)
two = internal.ConstReal(floatType, 2.0)
m = internal.ModuleCreateWithNameInContext("test app", cx)
ft =  internal.FunctionType(floatType, [floatType, floatType], false)
f =  internal.AddFunction(m, 'main', ft)
bb = internal.AppendBasicBlockInContext(cx, f, 'entry')
internal.PositionBuilderAtEnd(builder, bb)
internal.BuildRet(builder, one)

[fail, ee, str] = internal.CreateJITCompilerForModule(m, 0)
if fail
  console.log(str)
  internal.VerifyFunction(f, internal.PrintMessageAction)
  internal.DumpModule(m)
  process.exit(-1)

result = internal.RunFunction(ee, f, [])
fres = internal.GenericValueToFloat(floatType, result)
console.log(fres)

console.log("DONE")



