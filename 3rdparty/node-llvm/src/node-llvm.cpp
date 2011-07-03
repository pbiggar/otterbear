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

template<typename T>
class LLVMObject : node::ObjectWrap
{
  // TODO: add type safety, probably a combination of static (via templates)
  // and dynamaic (via type signature members in debug mode).
  LLVMObject<T>(T storage)
  {
    this->storage = storage;
  }

  T storage;

  static v8::Handle<v8::Value>
  ConstantFPGet(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    // args
    llvm::LLVMContext* arg0 = Unwrap<llvm::LLVMContext>(args[0]);
    llvm::APFloat* arg1 = Unwrap<llvm::APFloat>(args[1]);

    llvm::Value* result = llvm::ConstantFP::get(*arg0, *arg1);

    LLVMObject<llvm::Value*>* wrapper = new LLVMObject<llvm::Value*>(result);
    v8::Handle<v8::Value> r(wrapper);
    return r;
  }

  static v8::Handle<v8::Value>
  APFloat(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    // args
    double arg0 = args[0]->NumberValue();

    llvm::Value* result = llvm::APFloat(arg0);

    return new LLVMObject(result);
  }

  static v8::Handle<v8::Value>
  getGlobalContext(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    // no args

    llvm::LLVMContext& result = llvm::getGlobalContext();

    return new LLVMObject(result);
  }

  static v8::Handle<v8::Value>
  Module(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    // args
    const char* arg0 = args[0]->ToString()->GetExternalAsciiStringResource()->data();
    llvm::LLVMContext* arg1 = Unwrap<llvm::LLVMContext>(args[1]);

    llvm::Module* result = new llvm::Module(arg0, *arg1);

    return new LLVMObject(result);
  }
};

//  llvm::IRBuilder<> builder;
//  llvm::Module module;
//    builder(cx),
class LLVM : node::ObjectWrap
{
public:
  LLVM() {}

  static void Init(v8::Handle<v8::Object> target)
  {
    v8::HandleScope scope;

    v8::Local<v8::String> symbol = v8::String::NewSymbol("LLVM");

    pft = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(LLVM::New));
    pft->InstanceTemplate()->SetInternalFieldCount(1);
    pft->SetClassName(symbol);
    target->Set(symbol, pft->GetFunction());
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
