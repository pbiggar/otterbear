#include <string>

#include <v8.h>
#include <node.h>

#include "llvm-c/Core.h"
#include "llvm-c/Analysis.h"
#include "llvm-c/ExecutionEngine.h"


/*
 * LLVM wrapper API. In the longer term, maybe we should just copy ruby-llvm
 * (if we had an FFI-like class to implement it with, that would be awesome).
 *
 * The simplest way to implement this (while we're still prototyping) is to
 * provide direct access to a load of LLVM functions, and write the APIs in JS.
 * This gives us all the downsides of C++ (easy segfaults, no run-time safety),
 * with all the downsides of JS (no type-checking, poor relative performance).
 */


llvm::Value *Error(const char *Str) { exit(-1); }

template<typename T> T
print(const char* function, const char* type, T t)
{
//  printf("%p (%s,%s)\n", t, function, type);
  return t;
}

template<typename T> v8::Handle<v8::Value>
W(T v)
{
  if (v == NULL)
    return v8::Undefined();

  return v8::External::Wrap(v);
}

template<> v8::Handle<v8::Value> 
W<const char*>(const char* v)
{
  if (v == NULL)
    return v8::Undefined();

  return v8::String::New(v);
}

template<> v8::Handle<v8::Value> 
W<char*>(char* v)
{
  if (v == NULL)
    return v8::Undefined();

  return v8::String::New(v);
}

template<> v8::Handle<v8::Value> 
W<double>(double v)
{
  return v8::Number::New(v);
}

template<> v8::Handle<v8::Value> 
W<unsigned int>(unsigned int v)
{
  return v8::Uint32::New(v);
}

template<> v8::Handle<v8::Value> 
W<int>(int v)
{
  return v8::Int32::New(v);
}

template<> v8::Handle<v8::Value> 
W<long int>(long int v)
{
  return v8::Int32::New(v);
}

template<typename T> T
U(v8::Handle<v8::Value> v)
{
  if (v->IsUndefined())
    return NULL;

  return static_cast<T>(v8::External::Unwrap(v));
}

template<> const char*
U<const char*>(v8::Handle<v8::Value> v)
{
  return *v8::String::AsciiValue(v);
}

template<> char*
U<char*>(v8::Handle<v8::Value> v)
{
  if (v->IsUndefined())
    return NULL;

  return *v8::String::AsciiValue(v);
}

template<> double
U<double>(v8::Handle<v8::Value> v)
{
  return v->NumberValue();
}

template<> unsigned int
U<unsigned int>(v8::Handle<v8::Value> v)
{
  return v->Uint32Value();
}

template<> int
U<int>(v8::Handle<v8::Value> v)
{
  return v->Int32Value();
}

template<> long int
U<long int>(v8::Handle<v8::Value> v)
{
  return v->Int32Value();
}

class LLVM : node::ObjectWrap
{
public:
#define OUTARG(I,TYPE) TYPE arg##I##_storage = NULL; TYPE* arg##I = &arg##I##_storage;
#define ARG(I,TYPE) TYPE arg##I = print(NULL, "arg" #I, U<TYPE>(args[I]));
#define ARG_CAST(I,CALLER_TYPE,CALLEE_TYPE) CALLEE_TYPE arg##I = print(NULL, "arg" #I, (CALLEE_TYPE)(U<CALLER_TYPE>(args[I])));
#define ARG_POS(CALLEE_POS,TYPE,CALLER_POS) TYPE arg##CALLEE_POS = print(NULL, "arg(" #CALLER_POS "," #CALLEE_POS ")", U<TYPE>(args[CALLER_POS]));

// CALLER_ARG is the arg position in the calleR that has an array passed.
// LENGTH_ARG is the arg position in the calleE that will hold the length.
// ARRAY_ARG  is the arg position in the calleE that will hold the value pointer.
#define ARG_ARRAY(CALLER_ARG, LENGTH_ARG, ARRAY_ARG, TYPE) \
    v8::Array* arr = v8::Array::Cast(*args[CALLER_ARG]); \
    unsigned arg##LENGTH_ARG = arr->Length(); \
    TYPE* arg##ARRAY_ARG = new TYPE[arg##LENGTH_ARG]; \
    for (unsigned i = 0; i < arg##LENGTH_ARG; i++) \
    { \
      arg##ARRAY_ARG[i] = U<TYPE>(arr->Get(i)); \
    }

#define CALLN(FUNCTION,ARGS) v8::Handle<v8::Value> result = W((print(#FUNCTION, "call", FUNCTION ARGS)));

#define CALL0(FUNCTION) CALLN(FUNCTION, ());
#define CALL1(FUNCTION) CALLN(FUNCTION, (arg0));
#define CALL2(FUNCTION) CALLN(FUNCTION, (arg0, arg1));
#define CALL3(FUNCTION) CALLN(FUNCTION, (arg0, arg1, arg2));
#define CALL4(FUNCTION) CALLN(FUNCTION, (arg0, arg1, arg2, arg3));

#define CALLNVOID(FUNCTION,ARGS) FUNCTION ARGS;

#define CALL0VOID(FUNCTION) CALLNVOID(FUNCTION, ());
#define CALL1VOID(FUNCTION) CALLNVOID(FUNCTION, (arg0));
#define CALL2VOID(FUNCTION) CALLNVOID(FUNCTION, (arg0, arg1));
#define CALL3VOID(FUNCTION) CALLNVOID(FUNCTION, (arg0, arg1, arg2));
#define CALL4VOID(FUNCTION) CALLNVOID(FUNCTION, (arg0, arg1, arg2, arg3));

#define ASSIGN(VAL) v8::Handle<v8::Value> result = W(VAL);

#define RETURN0() return v8::Undefined()
#define RETURN1() return scope.Close(result)
#define RETURN2(I0) v8::Local<v8::Array> array_result = v8::Array::New(); \
  array_result->Set(0, result); \
  array_result->Set(1, W(arg##I0##_storage)); \
  return scope.Close(array_result)
#define RETURN3(I0,I1) v8::Local<v8::Array> array_result = v8::Array::New(); \
  array_result->Set(0, result); \
  array_result->Set(1, W(arg##I0##_storage)); \
  array_result->Set(2, W(arg##I1##_storage)); \
  return scope.Close(array_result)


  static v8::Handle<v8::Value>
  ConstReal(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMTypeRef);
    ARG(1, double);

    CALL2(LLVMConstReal);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  DoubleTypeInContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMContextRef);

    CALL1(LLVMDoubleTypeInContext);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  GetGlobalContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    CALL0(LLVMGetGlobalContext);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  ModuleCreateWithNameInContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, const char*);
    ARG(1, LLVMContextRef);

    CALL2(LLVMModuleCreateWithNameInContext);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  CreateBuilderInContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMContextRef);
    CALL1(LLVMCreateBuilderInContext);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  FunctionType(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMTypeRef);
    ARG_ARRAY(1, 2, 1, LLVMTypeRef);
    ARG(3, LLVMBool);

    CALL4(LLVMFunctionType);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  AddFunction(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMModuleRef);
    ARG(1, const char*);
    ARG(2, LLVMTypeRef);

    CALL3(LLVMAddFunction);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  AppendBasicBlockInContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMContextRef);
    ARG(1, LLVMValueRef);
    ARG(2, const char*);

    CALL3(LLVMAppendBasicBlockInContext);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  PositionBuilderAtEnd(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMBuilderRef);
    ARG(1, LLVMBasicBlockRef);

    CALL2VOID(LLVMPositionBuilderAtEnd);
    RETURN0();
  }

  static v8::Handle<v8::Value>
  BuildRet(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMBuilderRef);
    ARG(1, LLVMValueRef);

    CALL2(LLVMBuildRet);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  DumpModule(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMModuleRef);

    CALL1VOID(LLVMDumpModule);
    RETURN0();
  }

  static v8::Handle<v8::Value>
  VerifyFunction(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMValueRef);
    ARG_CAST(1, int, LLVMVerifierFailureAction);

    CALL2VOID(LLVMVerifyFunction);
    RETURN0();
  }

  static v8::Handle<v8::Value>
  PrintMessageAction(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ASSIGN((int)(LLVMPrintMessageAction));

    RETURN0();
  }

  static v8::Handle<v8::Value>
  CreateJITCompilerForModule(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    OUTARG(0, LLVMExecutionEngineRef);
    ARG_POS(1, LLVMModuleRef, 0);
    ARG_POS(2, int, 1);
    OUTARG(3, char*);

    CALL4(LLVMCreateJITCompilerForModule);

    RETURN3(0, 3);
  }

  static v8::Handle<v8::Value>
  RunFunction(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMExecutionEngineRef);
    ARG(1, LLVMValueRef);
    ARG_ARRAY(2, 2, 3, LLVMGenericValueRef);

    CALL4(LLVMRunFunction);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  GenericValueToFloat(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMTypeRef);
    ARG(1, LLVMGenericValueRef);

    CALL2(LLVMGenericValueToFloat);
    RETURN1();
  }

  static v8::Handle<v8::Value>
  New(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    LLVM* l = new LLVM();
    l->Wrap(args.This());
    return args.This();
  }

  static void Init(v8::Handle<v8::Object> target)
  {
    using namespace v8;
    HandleScope scope;

    pft = Persistent<FunctionTemplate>::New(FunctionTemplate::New(LLVM::New));

    pft->InstanceTemplate()->SetInternalFieldCount(1);
    pft->SetClassName(String::NewSymbol("LLVM"));

    target->Set(String::NewSymbol("LLVM"), pft->GetFunction());

#define DECLARE(NAME) pft->InstanceTemplate()->Set(#NAME, v8::FunctionTemplate::New(NAME));
    DECLARE(GetGlobalContext);
    DECLARE(ModuleCreateWithNameInContext);
    DECLARE(DoubleTypeInContext);
    DECLARE(ConstReal);
    DECLARE(FunctionType);
    DECLARE(AddFunction);
    DECLARE(AppendBasicBlockInContext);
    DECLARE(CreateBuilderInContext);
    DECLARE(PositionBuilderAtEnd);
    DECLARE(BuildRet);
    DECLARE(VerifyFunction);
    DECLARE(DumpModule);
    DECLARE(CreateJITCompilerForModule);
    DECLARE(RunFunction);
    DECLARE(GenericValueToFloat);
#undef DECLARE
  }

private:
  static v8::Persistent<v8::FunctionTemplate> pft;
};

v8::Persistent<v8::FunctionTemplate> LLVM::pft;
 
extern "C" {

static void
init(v8::Handle<v8::Object> target)
{
  LLVM::Init(target);
  LLVMLinkInJIT();
  LLVMInitializeX86Target();
  LLVMInitializeX86TargetInfo();
}

NODE_MODULE(llvm, init);

}
