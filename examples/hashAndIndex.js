'use strict';

/**
 * Imports.
 */
const
    _ = require('lodash'),
	Geohash = require('bindings')('_nd-geohash-core.node');


const geohash = Geohash();

function hash(lat, lng, name) {

    // generate geo hash for lat/lng
    const geohash = ndGeohash.generateGeohash(lat, lng);
    //console.log('generated geohash: %s', _.toString(geohash));

    // generate database hashkey from geohash
    const precision = 10;
    const hashKey = ndGeohash.generateHashKey(geohash, precision);
    console.log('[%s] %s', name, hashKey);
}
function getRanges(name, south, west, north, east) {
    console.log('geoquery: %d, %d, %d, %d', south, west, north, east);
	var ranges = ndGeohash.getGeohashRanges(south, west, north, east);

    // vector<S2CellId> parentCells;
    // vector<S2CellId> childCells;
    // S2LatLngRect boundingRect(S2LatLng min(bounds.min), S2LatLng max(bounds.max))

    // foreach (S2CellId parentCell in S2CellIds.all())
        // if boundingRect.intersects(S2Cell(parentCell))
            // parentCells.add(parentCell)

    // foreach (S2CellId parentCell in parentCells)
        // foreach(S2CellId childCell in parentCell.children())
            // if boundingRect.intersects(S2Cell(childCell))
                // childCells.add(childCell)

    // vector<GeohashRange> ranges;
    // S2CellUnion cellUnion(childCells);
    // foreach(S2CellId cellId in cellUnion.cellIds())
        // GeohashRange range(cellId.rangeMin(), cellId.rangeMax())
            // bool merged = false;
            // foreach(GeohashRange r in ranges)
                // if (r.tryMerge(range)) { merged = true; break; }
                // if (!merged) { ranges.addRange(range); }

    // vector<GeohashRange> hashedRanges;
    // foreach (GeohashRange range in ranges)
        // hashedRanges.addAll(range.splitByGranularity(granularity))

    // return hashedRanges (queried by HASH = hashedRange.min.getHashKey(), hashedRange.min() < RANGE < hashedRange.max())
}

// TODO: Log the answer in a database
console.log('starting hash...');

hash(43.760782,-79.765609, 'Animal Hospital');
hash(43.759465,-79.769965, 'Richardson');
hash(43.755311,-79.756854, 'McDonalds');
hash(43.744368,-79.745718, 'Temple');
hash(43.720801,-79.642206, 'Malton');
hash(43.647932,-79.389177, 'Nascent');
hash(43.643336,-79.380895, 'TELUS');

getRanges('brampton', 43.75565128202533, -79.79070942346192, 43.77424693425796, -79.7563771480713);
getRanges('gta', 43.50703245678626, -79.94717876855469, 44.10169354914478, -78.84854595605469);


