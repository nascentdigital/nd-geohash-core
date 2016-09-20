#ifndef ND_GEOHASH_GEOHASH_RANGE_H
#define ND_GEOHASH_GEOHASH_RANGE_H

#include <nan.h>
#include "S2CellId.h"


namespace nascent {

class GeohashRange {

    public:
        explicit GeohashRange(uint64 rangeMin, uint64 rangeMax)
            : rangeMin_(rangeMin), rangeMax_(rangeMax) {}
        explicit GeohashRange(const S2CellId &cellId);
        ~GeohashRange();

        uint64 min() { return rangeMin_; }
        uint64 max() { return rangeMax_; }

        bool tryMerge(const GeohashRange &range);


    private:
        uint64 rangeMin_;
        uint64 rangeMax_;

};  // class GeohashRange

}  // namespace nascent

#endif // ND_GEOHASH_GEOHASH_RANGE_H



