#ifndef ND_GEOHASH_GEOHASH_H
#define ND_GEOHASH_GEOHASH_H

#include <nan.h>

namespace nascent {

class Geohash : public Nan::ObjectWrap {

    public:
        static void Init();
        static v8::Local<v8::Object> NewInstance(Nan::MaybeLocal<v8::Object> options);

    private:
        static Nan::Persistent<v8::Function> constructorTemplate_;
        int hashKeyLength_;

        Geohash();
        ~Geohash();

        static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static void GetHash(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static void GetHashKey(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static void GetHashRanges(const Nan::FunctionCallbackInfo<v8::Value>& info);

};  // class Geohash

}  // namespace nascent

#endif // ND_GEOHASH_GEOHASH_H



