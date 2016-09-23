#ifndef ND_GEOHASH_GEOHASH_RANGE_H
#define ND_GEOHASH_GEOHASH_RANGE_H

#include <nan.h>
#include "S2CellId.h"


namespace nascent {

class GeohashRange {

    public:
        explicit GeohashRange(uint64_t rangeMin, uint64_t rangeMax) {
            if (rangeMin <= rangeMax) {
                rangeMin_ = rangeMin;
                rangeMax_ = rangeMax;
            }
            else {
                rangeMin_ = rangeMax;
                rangeMax_ = rangeMin;
            }
        }
        explicit GeohashRange(const S2CellId &cellId) {
            uint64_t rangeMin = cellId.range_min().id();
            uint64_t rangeMax = cellId.range_max().id();
            if (rangeMin <= rangeMax) {
                rangeMin_ = rangeMin;
                rangeMax_ = rangeMax;
            }
            else {
                rangeMin_ = rangeMax;
                rangeMax_ = rangeMin;
            }
        }
        ~GeohashRange();

        uint64_t min() const { return rangeMin_; }
        uint64_t max() const { return rangeMax_; }

        bool tryMerge(const GeohashRange &range);


    private:
        uint64_t rangeMin_;
        uint64_t rangeMax_;

};  // class GeohashRange

}  // namespace nascent

#endif // ND_GEOHASH_GEOHASH_RANGE_H



