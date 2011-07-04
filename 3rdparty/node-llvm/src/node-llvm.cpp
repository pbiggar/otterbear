#include <string>

#include <v8.h>
#include <node.h>

#include "llvm-c/Core.h"

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
U(v8::Handle<v8::Value> v)
{
  return static_cast<T>(v8::External::Unwrap(v));
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

#define ARG(I,TYPE) TYPE arg##I = U<TYPE>(args[I]);

#define RETURNN(TYPE,ARGS) return v8::External::Wrap(TYPE ARGS);
#define RETURN0(FUNCTION) RETURNN(FUNCTION, ());
#define RETURN1(FUNCTION) RETURNN(FUNCTION, (arg0));
#define RETURN2(FUNCTION) RETURNN(FUNCTION, (arg0, arg1));
#define RETURN3(FUNCTION) RETURNN(FUNCTION, (arg0, arg1, arg2));
#define RETURN4(FUNCTION) RETURNN(FUNCTION, (arg0, arg1, arg2, arg3));

  static v8::Handle<v8::Value>
  ConstReal(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMTypeRef);
    ARG(1, double);

    RETURN2(LLVMConstReal);
  }

  static v8::Handle<v8::Value>
  DoubleTypeInContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMContextRef);

    RETURN1(LLVMDoubleTypeInContext);
  }

  static v8::Handle<v8::Value>
  GetGlobalContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    RETURN0(LLVMGetGlobalContext);
  }

  static v8::Handle<v8::Value>
  ModuleCreateWithNameInContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, char*);
    ARG(1, LLVMContextRef);

    RETURN2(LLVMModuleCreateWithNameInContext);
  }

  static v8::Handle<v8::Value>
  CreateBuilderInContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMContextRef);
    RETURN1(LLVMBuilderRef);
  }

  static v8::Handle<v8::Value>
  FunctionType(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    ARG(0, LLVMTypeRef);
    ARG(1, LLVMTypeRef*);
    ARG(2, unsigned); // TODO array
    ARG(3, LLVMBool);

    RETURN4(LLVMFunctionType);
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
    DECLARE(CreateBuilderInContext);
    DECLARE(ModuleCreateWithNameInContext);
    DECLARE(DoubleTypeInContext);
    DECLARE(ConstReal);
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
