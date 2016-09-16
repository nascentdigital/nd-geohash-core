#include "Geohash.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <stack>
#include <cmath>
#include "s2.h"
#include "s2latlng.h"
#include "s2latlngrect.h"
#include "s2cell.h"
#include "s2cellid.h"
#include "s2cellunion.h"

using namespace v8;
using nascent::Geohash;


Nan::Persistent<Function> Geohash::constructorTemplate_;


void Geohash::Init() {

    // create scope instance
    Nan::HandleScope scope;

    // create constructor template for JavaScript wrapper
    Local<FunctionTemplate> constructorTemplate = Nan::New<FunctionTemplate>(New);
    constructorTemplate->SetClassName(Nan::New("Geohash").ToLocalChecked());
    constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

    // map instance methods
    Nan::SetPrototypeMethod(constructorTemplate, "getHash", GetHash);
    Nan::SetPrototypeMethod(constructorTemplate, "getHashKey", GetHashKey);
    Nan::SetPrototypeMethod(constructorTemplate, "getHashRanges", GetHashRanges);

    // sync constructor with template
    constructorTemplate_.Reset(constructorTemplate->GetFunction());
}


Geohash::Geohash()
    : hashKeyLength_(10) {
}

Geohash::~Geohash() {
}

Local<Object> Geohash::NewInstance(Nan::MaybeLocal<Object> options) {

    // create an escapable scope (allows us to return instance upward)
    Nan::EscapableHandleScope scope;

    // resolve constructor using template
    Local<Function> constructor = Nan::New<Function>(constructorTemplate_);

    // invoke constructor using default constructor
    Nan::MaybeLocal<Object> result;
    if (options.IsEmpty()) {
        result = Nan::NewInstance(constructor);
    }

    // or using specified options object
    else {
        Local<Value> argv[] = { options.ToLocalChecked() };
        result = Nan::NewInstance(constructor, 1, argv);
    }

    // TODO: throw if instance wasn't created

    // return instance
    Local<Object> instance = result.ToLocalChecked();
    return scope.Escape(instance);
}

void Geohash::New(const Nan::FunctionCallbackInfo<Value>& info) {

    // create new instance
    Geohash* instance = new Geohash();

    // set defaults

    // TODO: initialize instance with options (if any)
    if (info.Length() == 1) {

        // capture options
        Local<Object> options = info[0]->ToObject();

        // get hashkey length, if specified / valid
        Nan::MaybeLocal<Value> keyLengthOption = Nan::Get(options, Nan::New("keyLength").ToLocalChecked());
        if (!keyLengthOption.IsEmpty()) {
            Local<Value> keyLength = keyLengthOption.ToLocalChecked();
            if (keyLength->IsNumber()) {
                instance->hashKeyLength_ = keyLength->IntegerValue();
            }
        }
    }

    // wrap C++ instance in JavaScript counterpart
    instance->Wrap(info.This());

    // return wrapped instance
    info.GetReturnValue().Set(info.This());
}

void Geohash::GetHash(const Nan::FunctionCallbackInfo<Value>& info) {

	// fail if arguments are missing
    if (info.Length() != 2) {
        Nan::ThrowTypeError("Invalid number of arguments.");
        return;
    }

    // fail if both arguments are not numbers
    else if (!info[0]->IsNumber()
        || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Latitude and longitude must both be numbers.");
        return;
    }

    // capture arguments
    const double lat = info[0]->NumberValue();
    const double lng = info[1]->NumberValue();

    // create S2 cell from lat/lng pair
    const S2LatLng latLng = S2LatLng::FromDegrees(lat, lng);
    const S2Cell cell(latLng);
    const S2CellId cellId = cell.id();

    // return id (used as geohash)
    const std::string geohash = std::to_string(cellId.id());
    const Local<String> result = Nan::New(geohash).ToLocalChecked();
    info.GetReturnValue().Set(result);
}

void Geohash::GetHashKey(const Nan::FunctionCallbackInfo<Value>& info) {

    // fail if arguments are missing
    if (info.Length() != 1) {
        Nan::ThrowTypeError("Invalid number of arguments.");
        return;
    }

    // fail if geohash isn't a string
    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("Geohash must be a String.");
        return;
    }

    // parse numbers
    Nan::Utf8String geohash(info[0]->ToString());
    Geohash* instance = ObjectWrap::Unwrap<Geohash>(info.This());
    int hashKeyLength = instance->hashKeyLength_;

    // compensate for negative values by increasing hash key length
    uint64 numerator = std::stoull(std::string(*geohash));
    if (numerator < 0) {
        ++hashKeyLength;
    }

    // calculate hash key
    uint64 denominator = pow(10, geohash.length() - hashKeyLength);
    std::string hashKey(std::to_string(numerator / denominator));

    // return hash key
    Local<String> result = Nan::New(hashKey).ToLocalChecked();
    info.GetReturnValue().Set(result);
}

void Geohash::GetHashRanges(const Nan::FunctionCallbackInfo<Value>& info) {

    // fail if arguments are missing
    if (info.Length() != 1) {
        Nan::ThrowTypeError("Invalid number of arguments.");
        return;
    }

    // fail if geohash isn't a string
    if (!info[0]->IsObject()) {
        Nan::ThrowTypeError("Geohash bounds must be an Object.");
        return;
    }

    // get object and required properties
    Local<Object> bounds = info[0]->ToObject();
    Local<Object> boundsMin = Nan::Get(bounds, Nan::New("min").ToLocalChecked())
        .ToLocalChecked()->ToObject();
    Local<Object> boundsMax = Nan::Get(bounds, Nan::New("max").ToLocalChecked())
        .ToLocalChecked()->ToObject();

    // create bounds rect
    S2LatLngRect boundsRect(
        S2LatLng::FromDegrees(
            Nan::Get(boundsMin, Nan::New("lat").ToLocalChecked())
                    .ToLocalChecked()->NumberValue(),
            Nan::Get(boundsMin, Nan::New("lng").ToLocalChecked())
                    .ToLocalChecked()->NumberValue()),
        S2LatLng::FromDegrees(
            Nan::Get(boundsMax, Nan::New("lat").ToLocalChecked())
                    .ToLocalChecked()->NumberValue(),
            Nan::Get(boundsMax, Nan::New("lng").ToLocalChecked())
                    .ToLocalChecked()->NumberValue()));
    std::cout << "[GetGeohashRanges] calculating: " << boundsRect << std::endl;

    // intersect largest cells across worldspace with bounds and collect hits
    std::stack<S2CellId> parentCells;
    const S2CellId parentEnd = S2CellId::End(0);
    for(S2CellId c = S2CellId::Begin(0); c != parentEnd; c = c.next()) {
        if (boundsRect.Intersects(S2Cell(c))) {
            std::cout << "[GetGeohashRanges] parent: " << c.id() << std::endl;
            parentCells.push(c);
        }
    }

    // iterate over all matching parent cells
    std::vector<S2CellId> rangeCells;
    while(!parentCells.empty()) {

        // process next cell
        S2CellId parentCell = parentCells.top();
        parentCells.pop();

        // skip cell if it's invalid
        if (!parentCell.is_valid()) {
            continue;
        }

        // find all immediate children in range
        std::vector<S2CellId> childCells;
        for(S2CellId c = parentCell.child_begin(); c != parentCell.child_end(); c = c.next()) {
            if (boundsRect.Intersects(S2Cell(c))) {
                childCells.push_back(c);
            }
        }

        // drill into children if there is only 1 or two matches
        const int childCount = childCells.size();
        switch (childCount) {

            // evaluate individual children if the set seems to small
            case 1:
            case 2:
                for (std::vector<S2CellId>::iterator i = childCells.begin(); i != childCells.end(); ++i) {

                    // add the cell to range if it's a leaf
                    S2CellId &childCell = *i;
                    if (childCell.is_leaf()) {
                        std::cout << "[GetGeohashRanges] adding child leaf node: " << childCell.id() << std::endl;
                        rangeCells.push_back(childCell);
                    }

                    // or add to parents for later evaulation
                    else {
                        parentCells.push(childCell);
                    }
                }
                break;

            // add all children to range if there are 3 nodes
            case 3:
                for (std::vector<S2CellId>::iterator i = childCells.begin(); i != childCells.end(); ++i) {
                    S2CellId &childCell = *i;
                    std::cout << "[GetGeohashRanges] adding child node: " << childCell.id() << std::endl;
                    rangeCells.push_back(childCell);
                }
                break;

            // add parent to range if all children match
            case 4:
                std::cout << "[GetGeohashRanges] adding parent node: " << parentCell.id() << std::endl;
                rangeCells.push_back(parentCell);
                break;

            // a matching parent can only have 1-4 children, so this is invalid
            default:
                assert(false);
                break;
        }
    }

    // create union of all child cells
    S2CellUnion cellUnion;
    cellUnion.Init(rangeCells);

    // iterate over all children in union
    for (int i = 0; i < cellUnion.num_cells(); ++i) {
        const S2CellId &childCell = cellUnion.cell_id(i);
        std::cout << "[GetGeohashRanges] creating range for child: " << childCell.id() << std::endl;
    }

    info.GetReturnValue().Set(Nan::New(true));
}
