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
 */


llvm::Value *Error(const char *Str) { exit(-1); }

class LLVM : node::ObjectWrap
{
private:
public:
  llvm::LLVMContext cx;
  llvm::IRBuilder<> builder;
  llvm::Module module;

  LLVM() : 
    builder(cx),
    module("LLVM::LLVM",cx)
  {
    llvm::InitializeNativeTarget();
  }


  
  // Holds our constructor function
  static v8::Persistent<v8::FunctionTemplate> pft;

  // Node.js calls Init() when you load the extension through require().
  // Init() defines our constructor function and prototype methods. It then
  // binds our constructor function as a property of the target object. Target
  // is the "target" onto which an extension is loaded.
  //
  // For example:
  //   var notify = require("../build/default/gtknotify.node");
  // will bind our constructor function to notify.Notification so that we can
  // call "new notify.Notification();"
  static void Init(v8::Handle<v8::Object> target)
  {
    initializeV8Object(target);
  }

  static void initializeV8Object (v8::Handle<v8::Object> target)
  {
    v8::HandleScope scope;

    // Make it persistent and assign it to our object's pft attribute
    pft = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(LLVM::New));

    // Each JavaScript object keeps a reference to the C++ object for which it
    // is a wrapper with an internal field.
    pft->InstanceTemplate()->SetInternalFieldCount(1); // 1 since this is a constructor function

    // Set a class name for objects created with our constructor
    pft->SetClassName(v8::String::NewSymbol("LLVM"));
    
    // Set property accessors
//    pft->InstanceTemplate()->SetAccessor(String::New("title"), GetTitle, SetTitle);
//    pft->InstanceTemplate()->SetAccessor(String::New("icon"), GetIcon, SetIcon);
    
    NODE_SET_PROTOTYPE_METHOD(pft, "doubleValue", doubleValue);
//    NODE_SET_PROTOTYPE_METHOD(pft, "add", add);
//    NODE_SET_PROTOTYPE_METHOD(pft, "subtract", add);
//    NODE_SET_PROTOTYPE_METHOD(pft, "signature", signature);
    
    // Set the "notification" property to the target and assign it to our
    // constructor function
    target->Set(v8::String::NewSymbol("LLVM"), pft->GetFunction());
  }

  // new LLVM();
  // This is our constructor function. It instantiate a C++ LLVM object and
  // returns a Javascript handle to this object.
  static v8::Handle<v8::Value>
  New(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    LLVM* l = new LLVM();
    l->Wrap(args.This());
    return args.This();
  }

  // LLVM.doubleValue
  // This is a method part of the constructor function's prototype
  static v8::Handle<v8::Value>
  doubleValue(const v8::Arguments& args)
  {
    v8::HandleScope scope;

    double arg = args[0]->NumberValue();

    LLVM* l = Unwrap<LLVM>(args.This());
   
    llvm::Value* result = llvm::ConstantFP::get(l->cx, llvm::APFloat(arg));
    return v8::External::Wrap(result);
  }
};

v8::Persistent<v8::FunctionTemplate> LLVM::pft;
 

/* 
 * Thats it for actual interfacing with v8, finally we need to let Node.js know
 * how to dynamically load our code. Because a Node.js extension can be loaded
 * at runtime from a shared object, we need a symbol that the dlsym function
 * can find.
 */
extern "C" {

static void
init(v8::Handle<v8::Object> target)
{
  LLVM::Init(target);
}

// @see http://github.com/ry/node/blob/v0.2.0/src/node.h#L101
NODE_MODULE(llvm, init);

}
