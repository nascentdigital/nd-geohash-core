const
    _ = require('lodash'),
    Geohash = require('bindings')('_nd-geohash-core.node');

const geohash = new Geohash({ hashKeyLength: 13 });
const hash = geohash.getHash(43.760782,-79.765609);
const hashKey = geohash.getHashKey(hash);
console.log('hash:    %s', hash);
console.log('hashKey: %s', hashKey);

const hashRanges = geohash.getHashRanges(43.75565128202533, -79.79070942346192, 43.77424693425796, -79.7563771480713);