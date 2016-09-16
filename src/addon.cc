// geo.cc
#include <nan.h>
#include "Geohash.h"

using namespace v8;


void GeohashConstructor(const Nan::FunctionCallbackInfo<Value>& info) {

    // fail if too many arguments are specified
    const int argumentCount = info.Length();
    if (argumentCount > 1) {
        Nan::ThrowError("Invalid number of arguments.");
        return;
    }

    // support optional parameters (i.e. no options specified)
    Nan::MaybeLocal<Object> options;

    // handle case where option is provided
    if (argumentCount == 1) {

        // fail if argument isn't an object
        if (!info[0]->IsObject()) {
            Nan::ThrowTypeError("Constructor can only take a options object`.");
            return;
        }

        // or assign object as options
        else {
            options = Nan::MaybeLocal<Object>(info[0]->ToObject());
        }
    }

    // create new wrapped object instance and return it
    info.GetReturnValue().Set(nascent::Geohash::NewInstance(options));
}

void Init(Local<Object> exports, Local<Object> module) {

    // define scope
    Nan::HandleScope scope;

    // initialize geohash class
    nascent::Geohash::Init();

    // export constructor
    Nan::SetMethod(module, "exports", GeohashConstructor);
}

NODE_MODULE(_nd_geohash_core, Init)
