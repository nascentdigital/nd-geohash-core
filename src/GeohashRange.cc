#include "GeohashRange.h"
#include <iostream>


using nascent::GeohashRange;


const uint64_t MERGE_THRESHOLD = 2;


GeohashRange::~GeohashRange() {
}

bool GeohashRange::tryMerge(const GeohashRange &range) {

    // merge if it overlaps on the low end
    if (range.rangeMin_ - rangeMin_ <= MERGE_THRESHOLD
        && range.rangeMin_ - rangeMax_ > 0) {

        rangeMax_ = range.rangeMax_;
        return true;
    }

    // merge if it overlaps on the high end
    if (rangeMin_ - range.rangeMax_ <= MERGE_THRESHOLD
        && rangeMin_ - range.rangeMax_ > 0) {

        rangeMin_ = range.rangeMin_;
        return true;
    }

    // indicate no merge
    return false;
}
