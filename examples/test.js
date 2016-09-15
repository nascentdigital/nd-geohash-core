const
    _ = require('lodash'),
    Geohash = require('bindings')('_nd-geohash-core.node');

const geohashA = Geohash();
const geohashB = Geohash({});
const geohashC = new Geohash();
const geohashD = new Geohash({});

console.log('done!');