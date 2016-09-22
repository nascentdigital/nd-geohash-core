#include "Geohash.h"
#include "GeohashRange.h"
#include <string>
#include <vector>
#include <stack>
#include <utility>
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
        Nan::MaybeLocal<Value> hashKeyLengthOption = Nan::Get(options, Nan::New("hashKeyLength").ToLocalChecked());
        if (!hashKeyLengthOption.IsEmpty()) {
            Local<Value> hashKeyLength = hashKeyLengthOption.ToLocalChecked();
            if (hashKeyLength->IsNumber()) {
                instance->hashKeyLength_ = hashKeyLength->IntegerValue();
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

    // resolve hash string
    Nan::Utf8String geohash(info[0]->ToString());
    const uint64_t hash = std::stoull(std::string(*geohash));

    // invoke instance method
    Geohash* instance = ObjectWrap::Unwrap<Geohash>(info.This());
    const uint64_t hashKey = instance->GetHashKey(hash);

    // return hash key
    Local<String> result = Nan::New(std::to_string(hashKey)).ToLocalChecked();
    info.GetReturnValue().Set(result);
}

void Geohash::GetHashRanges(const Nan::FunctionCallbackInfo<Value>& info) {

    // fail if arguments are missing
    if (info.Length() != 5) {
        Nan::ThrowTypeError("Invalid number of arguments.");
        return;
    }

    // fail if geohash isn't a string
    if (!info[0]->IsNumber()
        || !info[1]->IsNumber()
        || !info[2]->IsNumber()
        || !info[3]->IsNumber()) {
        Nan::ThrowTypeError("Expected bounds to be numeric positional coordinates (south, west, north, east).");
        return;
    }

    // capture hash key hints if specified (for performance)
    Set *hashKeyHints = NULL;
    if (info[4]->IsSet()) {
        hashKeyHints = Set::Cast(*info[4]);
    }

    // or execute without hints
    else if (info[4]->IsNull() || info[4]->IsUndefined()) {
    }

    // or throw if arugment is unrecognized
    else {
        Nan::ThrowTypeError("Hash key hints must be a Set if specified.");
        return;
    }

    // create bounds rect
    S2LatLngRect boundsRect(
        S2LatLng::FromDegrees(
            info[0]->NumberValue(),
            info[1]->NumberValue()),
        S2LatLng::FromDegrees(
            info[2]->NumberValue(),
            info[3]->NumberValue()));

    // intersect largest cells across worldspace with bounds and collect hits
    std::stack<S2CellId> parentCells;
    const S2CellId parentEnd = S2CellId::End(0);
    for(S2CellId c = S2CellId::Begin(0); c != parentEnd; c = c.next()) {
        if (boundsRect.Intersects(S2Cell(c))) {
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
                    rangeCells.push_back(childCell);
                }
                break;

            // add parent to range if all children match
            case 4:
                rangeCells.push_back(parentCell);
                break;

            // a matching parent can only have 1-4 children, so this is invalid
            default:
                assert(false);
                break;
        }
    }

    // stop processing if there are no cells to union
    const int resultsLength = rangeCells.size();
    if (resultsLength == 0) {
        info.GetReturnValue().Set(Nan::New<Array>(0));
        return;
    }

    // create union of all child cells
    S2CellUnion cellUnion;
    cellUnion.Init(rangeCells);

    // transform cells into a collection of merged ranges
    std::vector<GeohashRange> outerRanges;
    for (int i = 0; i < cellUnion.num_cells(); ++i) {

        // get next child cell and transform to range
        const S2CellId &childCell = cellUnion.cell_id(i);
        GeohashRange range(childCell);

        // try to merge range
        bool merged = false;
        for (std::vector<GeohashRange>::iterator i = outerRanges.begin(); i != outerRanges.end(); ++i) {
            GeohashRange &existingRange = *i;
            if (existingRange.tryMerge(range)) {
                merged = true;
                break;
            }
        }

        // add range to collection if it wasn't merged
        if (!merged) {
            outerRanges.push_back(range);
        }
    }

    // massage outer ranges into more granular ranges, for unique hash keys
    const Geohash* instance = ObjectWrap::Unwrap<Geohash>(info.This());
    const Local<Context> context = Nan::GetCurrentContext();
    std::vector<std::pair<Local<String>, GeohashRange>> ranges;
    for (std::vector<GeohashRange>::iterator i = outerRanges.begin(); i != outerRanges.end(); ++i) {

        // capture next outer range and min/max hash keys
        GeohashRange &outerRange = *i;

        // simply add item if hashed range is narrow
        uint64_t rangeMin = outerRange.min();
        uint64_t minHashKey = instance->GetHashKey(rangeMin);
        uint64_t rangeMax = outerRange.max();
        uint64_t maxHashKey = instance->GetHashKey(rangeMax);
        if (minHashKey == maxHashKey) {

            // add range if it satisfies the hints (if any)
            Local<String> rangeHashKey = Nan::New(std::to_string(minHashKey)).ToLocalChecked();
            if (!hashKeyHints ||
                hashKeyHints->Has(context, rangeHashKey).FromJust()) {

                ranges.push_back(std::pair<Local<String>, GeohashRange>(rangeHashKey, outerRange));
            }
        }

        // or create ranges for hashed values
        else {

            // calculate range denominator
            uint64_t denominator = pow(10, std::to_string(rangeMin).length()
                - std::to_string(minHashKey).length());

            for (uint64_t hashKey = minHashKey; hashKey <= maxHashKey; ++hashKey) {

                // create range
                GeohashRange range = hashKey > 0
                    ? GeohashRange(hashKey == minHashKey ? rangeMin : hashKey * denominator,
                        hashKey == maxHashKey ? rangeMax : (hashKey + 1) * denominator - 1)
                    : GeohashRange(hashKey == minHashKey ? rangeMin : (hashKey - 1) * denominator + 1,
                        hashKey == maxHashKey ? rangeMax : hashKey * denominator);

                // add range if it's satisfies the hints (if any)
                Local<String> rangeHashKey = Nan::New(std::to_string(instance->GetHashKey(range.min())))
                    .ToLocalChecked();
                if (!hashKeyHints ||
                    hashKeyHints->Has(context, rangeHashKey).FromJust()) {

                    ranges.push_back(std::pair<Local<String>, GeohashRange>(rangeHashKey, range));
                }
            }
        }
    }

    // return ranges as an array of objects
    Local<Array> rangesArray = Nan::New<Array>(ranges.size());
    int rangeIndex = 0;
    for (std::vector<std::pair<Local<String>, GeohashRange>>::iterator i = ranges.begin(); i != ranges.end(); ++i) {

        // get range
        std::pair<Local<String>, GeohashRange> &entry = *i;

        // create object for range
        Local<Object> rangeObject = Nan::New<Object>();
        rangeObject->Set(Nan::New("hashKey").ToLocalChecked(), entry.first);
        rangeObject->Set(Nan::New("min").ToLocalChecked(),
            Nan::New(std::to_string(entry.second.min())).ToLocalChecked());
        rangeObject->Set(Nan::New("max").ToLocalChecked(),
            Nan::New(std::to_string(entry.second.max())).ToLocalChecked());

        // add object to array
        rangesArray->Set(rangeIndex++, rangeObject);
    }

    // return results
    info.GetReturnValue().Set(rangesArray);
}

uint64_t Geohash::GetHashKey(const uint64_t hash) const {

    // resolve key length (need to mutate it later)
    int hashKeyLength = hashKeyLength_;

    // compensate for negative values by increasing hash key length
    if (hash < 0) {
        ++hashKeyLength;
    }

    // calculate hash key
    uint64_t denominator = pow(10, std::to_string(hash).length() - hashKeyLength);
    uint64_t hashKey = hash / denominator;

    // return hash key
    return hashKey;
}