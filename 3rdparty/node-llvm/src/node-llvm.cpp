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
  return v8::External::Wrap(v);
}

template<> v8::Handle<v8::Value> 
W<const char*>(const char* v)
{
  return v8::String::New(v);
}

template<> v8::Handle<v8::Value> 
W<char*>(char* v)
{
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
  return v8::Number::New(v);
}

template<> v8::Handle<v8::Value> 
W<int>(int v)
{
  return v8::Number::New(v);
}


template<typename T> T
U(v8::Handle<v8::Value> v)
{
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

class LLVM : node::ObjectWrap
{
public:
  LLVM() {}

#define OUTARG(I,TYPE) TYPE arg##I##_storage = NULL; TYPE* arg##I = &arg##I##_storage;
#define ARG(I,TYPE) TYPE arg##I = print(NULL, "arg" #I, U<TYPE>(args[I]));

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

#define RETURN0() return v8::Undefined()
#define RETURN1() return scope.Close(result)
#define RETURN2(I1) v8::Local<v8::Array> array_result = v8::Array::New(); \
  array_result->Set(0, result); \
  array_result->Set(1, W(arg##I1##_storage)); \
  return scope.Close(array_result)
#define RETURN3(I1,I2) v8::Local<v8::Array> array_result = v8::Array::New(); \
  array_result->Set(0, result); \
  array_result->Set(1, W(arg##I1##_storage)); \
  array_result->Set(2, W(arg##I2##_storage)); \
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

    v8::Array* arr = v8::Array::Cast(*args[1]);
    int length = arr->Length();
    LLVMTypeRef* arg1 = new LLVMTypeRef[length];
    for (int i = 0; i < length; i++)
    {
      arg1[i] = U<LLVMTypeRef>(arr->Get(i));
    }
    unsigned arg2 = length;

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
    LLVMVerifierFailureAction arg1 = LLVMPrintMessageAction;

    CALL2VOID(LLVMVerifyFunction);
    RETURN0();
  }

  static v8::Handle<v8::Value>
  CreateJITCompilerForModule(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    LLVMExecutionEngineRef arg0_storage = NULL;
    LLVMExecutionEngineRef* arg0 = &arg0_storage;

    ARG(1, LLVMModuleRef);
    ARG(2, int);

    char* arg3_storage = NULL;
    char** arg3 = &arg3_storage;

    CALL4(LLVMCreateJITCompilerForModule);

    RETURN3(0, 3);
  }

  static v8::Handle<v8::Value>
  RunFunction(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMExecutionEngineRef);
    ARG(1, LLVMValueRef);

    v8::Array* arr = v8::Array::Cast(*args[2]);
    int length = arr->Length();
    LLVMGenericValueRef* arg3 = new LLVMGenericValueRef[length];
    for (int i = 0; i < length; i++)
    {
      arg3[i] = U<LLVMGenericValueRef>(arr->Get(i));
    }
    unsigned arg2 = length;

    CALL4(LLVMRunFunction);
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
}

NODE_MODULE(llvm, init);

}
