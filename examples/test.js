const
    _ = require('lodash'),
    Geohash = require('bindings')('_nd-geohash-core.node');

const geohash = new Geohash({ hashKeyLength: 10 });
const hash = geohash.getHash(43.7091270,-79.7841510);
const hashKey = geohash.getHashKey(hash);
console.log('hash:    %s', hash);
console.log('hashKey: %s', hashKey);

const hashKeyHints = new Set();
hashKeyHints.add(hashKey);

const hashRanges = geohash.getHashRanges(43.700620654952886, -79.8008670390625,
    43.719233402578766, -79.76653476367187, hashKeyHints);

console.log('hashRanges:');
_.forEach(hashRanges, (hashRange) => {
    console.log('   [%s, %s, %s]', hashRange.hashKey, hashRange.min, hashRange.max);
});