#include <string>

#include <v8.h>
#include <node.h>

#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"


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

//  llvm::IRBuilder<> builder;
//  llvm::Module module;
//    builder(cx),
class LLVM : node::ObjectWrap
{
public:
  LLVM() {}

  static v8::Handle<v8::Value>
  ConstantFPGet(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    // args
    llvm::LLVMContext* arg0 = U<llvm::LLVMContext*>(args[0]);
    llvm::APFloat* arg1 = U<llvm::APFloat*>(args[1]);

    llvm::Value* result = llvm::ConstantFP::get(*arg0, *arg1);

    return v8::External::Wrap(result);
  }

  static v8::Handle<v8::Value>
  APFloat(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    // args
    double arg0 = args[0]->NumberValue();

    llvm::APFloat* result = new llvm::APFloat(arg0);

    return v8::External::Wrap(result);
  }

  static v8::Handle<v8::Value>
  getGlobalContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    // no args

    llvm::LLVMContext& result = llvm::getGlobalContext();

    return v8::External::Wrap(&result); // TODO
  }

  static v8::Handle<v8::Value>
  Module(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    // args
    const char* arg0 = args[0]->ToString()->GetExternalAsciiStringResource()->data();
    llvm::LLVMContext* arg1 = U<llvm::LLVMContext*>(args[1]);

    llvm::Module* result = new llvm::Module(arg0, *arg1);

    return v8::External::Wrap(result);
  }


  // new LLVM();
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
  }

private:
  static v8::Persistent<v8::FunctionTemplate> pft;
};

v8::Persistent<v8::FunctionTemplate> LLVM::pft;
 
extern "C" {

static void
init(v8::Handle<v8::Object> target)
{
  llvm::InitializeNativeTarget();
  LLVM::Init(target);
}

NODE_MODULE(llvm, init);

}
