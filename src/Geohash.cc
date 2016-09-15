#include "Geohash.h"
#include <iostream>

using namespace v8;
using nascent::Geohash;


Nan::Persistent<Function> Geohash::constructorTemplate;


void Geohash::Init() {

    // create scope instance
    Nan::HandleScope scope;

    // create constructor template
    Local<FunctionTemplate> functionTemplate = Nan::New<FunctionTemplate>(New);
    functionTemplate->SetClassName(Nan::New("Geohash").ToLocalChecked());
    functionTemplate->InstanceTemplate()->SetInternalFieldCount(1);

    // TODO: map instance methods

    // sync constructor with template
    constructorTemplate.Reset(functionTemplate->GetFunction());
}

Local<Object> Geohash::NewInstance(Nan::MaybeLocal<Object> options) {

    // create an escapable scope (allows us to return instance upward)
    Nan::EscapableHandleScope scope;

    // resolve constructor using template
    Local<Function> constructor = Nan::New<v8::Function>(constructorTemplate);

    // invoke constructor using default constructor
    Nan::MaybeLocal<Object> result;
    if (options.IsEmpty()) {
        std::cout << "Geohash::NewInstance()" << std::endl;
        result = Nan::NewInstance(constructor);
    }

    // or using specified options object
    else {
        std::cout << "Geohash::NewInstance({})" << std::endl;
        Local<Value> argv[] = { options.ToLocalChecked() };
        result = Nan::NewInstance(constructor, 1, argv);
    }

    // TODO: throw if instance wasn't created

    // return instance
    Local<Object> instance = result.ToLocalChecked();
    return scope.Escape(instance);
}


Geohash::Geohash() {
}

Geohash::~Geohash() {
}


void Geohash::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {

    // create new instance
    Geohash* instance = new Geohash();

    // TODO: initialize instance with options (if any)
    std::cout << "Geohash::New(" << (info.Length() > 0 ? "{}" : "") << ")" << std::endl;

    // wrap C++ instance in JavaScript counterpart
    instance->Wrap(info.This());

    // return wrapped instance
    info.GetReturnValue().Set(info.This());
}