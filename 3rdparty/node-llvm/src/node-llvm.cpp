#include <v8.h>
#include <node.h>
#include <string>

/*
 * LLVM wrapper API. In the longer term, maybe we should just copy ruby-llvm
 * (if we had an FFI-like class to implement it with, that would be awesome).
 */


using namespace v8;

class MyLLVM : node::ObjectWrap
{
private:
public:
  std::string title;
  std::string icon;
  
  // Holds our constructor function
  static Persistent<FunctionTemplate> pft;

  // Node.js calls Init() when you load the extension through require().
  // Init() defines our constructor function and prototype methods. It then
  // binds our constructor function as a property of the target object. Target
  // is the "target" onto which an extension is loaded.
  //
  // For example:
  //   var notify = require("../build/default/gtknotify.node");
  // will bind our constructor function to notify.Notification so that we can
  // call "new notify.Notification();"
  static void Init(Handle<Object> target) {

    HandleScope scope;

    // Wrap our New() method so that it's accessible from Javascript
    Local<FunctionTemplate> local_function_template = FunctionTemplate::New(New);
    
    // Make it persistent and assign it to our object's pft attribute
    pft = Persistent<FunctionTemplate>::New(local_function_template);

    // Each JavaScript object keeps a reference to the C++ object for which it
    // is a wrapper with an internal field.
    pft->InstanceTemplate()->SetInternalFieldCount(1); // 1 since this is a constructor function

    // Set a class name for objects created with our constructor
    pft->SetClassName(String::NewSymbol("Notification"));
    
    // Set property accessors
    pft->InstanceTemplate()->SetAccessor(String::New("title"), GetTitle, SetTitle);
    pft->InstanceTemplate()->SetAccessor(String::New("icon"), GetIcon, SetIcon);
    
    // Node.js macro to help bind C++ methods to Javascript methods
    // (see https://github.com/joyent/node/blob/v0.2.0/src/node.h#L34)
    // Arguments: our constructor function, Javascript method name, C++ method name
    NODE_SET_PROTOTYPE_METHOD(pft, "send", Send);
    
    // Set the "notification" property to the target and assign it to our
    // constructor function
    target->Set(String::NewSymbol("notification"), pft->GetFunction());
  }

  // new Notification();
  // This is our constructor function. It instantiate a C++ MyLLVM object and
  // returns a Javascript handle to this object.
  static Handle<Value> New(const Arguments& args)
  {
    HandleScope scope;
    MyLLVM* myllvm = new MyLLVM();
    // Set some default values
    
    // Wrap our C++ object as a Javascript object
    myllvm->Wrap(args.This());
    
    
    // Our constructor function returns a Javascript object which is a
    // wrapper for our C++ object, This is the expected behavior when calling
    // a constructor function with the new operator in Javascript.
    return args.This();
  }

  // notification.send();
  // This is a method part of the constructor function's prototype
  static Handle<Value> Send(const Arguments& args)
  {
    HandleScope scope;

    // Extract C++ object reference from "this" aka args.This() argument
    MyLLVM* myllvm = node::ObjectWrap::Unwrap<MyLLVM>(args.This());
    
    // Convert first argument to V8 String
    String::Utf8Value v8str(args[0]);
    
    // Return value
    return Boolean::New(true);
  }
  
  // notification.title
  static Handle<Value> GetTitle(Local<String> property, const AccessorInfo& info)
  {
    // Extract the C++ request object from the JavaScript wrapper.
    MyLLVM* myllvm = node::ObjectWrap::Unwrap<MyLLVM>(info.Holder());
    return String::New(myllvm->title.c_str());
  }

  static void SetTitle(Local<String> property, Local<Value> value, const AccessorInfo& info)
  {
    MyLLVM* myllvm = node::ObjectWrap::Unwrap<MyLLVM>(info.Holder());
    String::Utf8Value v8str(value);
    myllvm->title = *v8str;
  }

  // notification.icon
  static Handle<Value> GetIcon(Local<String> property, const AccessorInfo& info)
  {
    // Extract the C++ request object from the JavaScript wrapper.
    MyLLVM* myllvm = node::ObjectWrap::Unwrap<MyLLVM>(info.Holder());
    return String::New(myllvm->icon.c_str());
  }

  static void SetIcon(Local<String> property, Local<Value> value, const AccessorInfo& info)
  {
    MyLLVM* myllvm = node::ObjectWrap::Unwrap<MyLLVM>(info.Holder());
    String::Utf8Value v8str(value);
    myllvm->icon = *v8str;
  }
};


// See https://www.cloudkick.com/blog/2010/aug/23/writing-nodejs-native-extensions/ & http://www.freebsd.org/cgi/man.cgi?query=dlsym
/* 
 * Thats it for actual interfacing with v8, finally we need to let Node.js know
 * how to dynamically load our code. Because a Node.js extension can be loaded
 * at runtime from a shared object, we need a symbol that the dlsym function
 * can find.
 */
extern "C" {

static void
init(Handle<Object> target)
{
  MyLLVM::Init(target);
}

// @see http://github.com/ry/node/blob/v0.2.0/src/node.h#L101
NODE_MODULE(llvm, init);

}
