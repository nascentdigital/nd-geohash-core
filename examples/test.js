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

const items = [
    {"id":"g","location":{"lat":43.733355,"lng":-79.50675},"address":"26 Troutbrooke Dr, Toronto, ON M3M 1S5","hash":"98119908","name":"Beverley Heights","type":"public","geohash":"9811990841357583331"},
    {"id":"j","location":{"lat":43.731364,"lng":-79.434129},"address":"18 Neptune Dr, North York, ON M6A 1X1","hash":"98119915","name":"Posluns","type":"private","geohash":"9811991579379019679"},
    {"id":"b","location":{"lat":43.645748,"lng":-79.404765},"address":"20 Portugal Square, Toronto, ON M6J 3P2","hash":"98119943","name":"St Mary","type":"catholic","geohash":"9811994353128341811"},
    {"id":"e","location":{"lat":43.707511,"lng":-79.703979},"address":"57 Aloma Cres., Brampton, ON L6T2N8","hash":"98120042","name":"Aloma Crescent","type":"public","geohash":"9812004210389708569"},
    {"id":"i","location":{"lat":43.617996,"lng":-79.587622},"address":"3255 Havenwood Dr, Mississauga, ON L4X 2M2","hash":"98120148","name":"Brian W Fleming","type":"public","geohash":"9812014878945791991"}
];
const filteredItems = geohash.filterItems(items, 'location', 43.60311967129431, -79.72756247167979,
    43.752101797181005, -79.45290426855479);
console.log('filtered %d items:', filteredItems.length);
_.forEach(filteredItems, (item) => console.log('   %s', item.name));
