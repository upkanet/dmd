var ffi = require('ffi-napi');
var path = require('path');

var lib = ffi.Library(path.join(__dirname, './api/alp42'), {
    'AlpDevAlloc': ['long', ['long','long']],
    'AlpDevInquire': ['long', ['long','long']]
});

var alp_id = lib.AlpDevAlloc(0,0);

console.log(alp_id);
console.log(path.join(__dirname, './api/alp42'));
