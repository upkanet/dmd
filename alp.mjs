import ffi from 'ffi-napi';
import path from 'path';
const __dirname = path.resolve();

const ALP_DEFAULT  = 0;
const ALP_DEV_DMDTYPE = 2021;

/*var c = [];
c[1001] = "ALP_NOT_ONLINE";

//DMD type (probable) "ALP_DMDTYPE_1080P_095A"

//DLL Library
var alplib = ffi.Library(path.join(__dirname, './api/alp42'), {
    'AlpDevAlloc': ['long', ['long','long']],
    'AlpDevInquire': ['long', ['long','long']]
});

//Initialize
//get Device ID
var ALP_ID = alplib.AlpDevAlloc(ALP_DEFAULT,ALP_DEFAULT);
var AlpIdName = c[ALP_ID];
//get Device Type
var DMDType = alplib.AlpDevInquire(ALP_ID,ALP_DEV_DMDTYPE);
console.log(AlpIdName,DMDType);*/

class ALP {
    constructor(){
        this.lib = ffi.Library(path.join(__dirname, './api/alp42'), {
            'AlpDevAlloc': ['long', ['long','long']],
            'AlpDevInquire': ['long', ['long','long']]
        });
    }

    init(){
        this.alp_id = this.lib.AlpDevAlloc(ALP_DEFAULT,ALP_DEFAULT);
        this.dmd_type = this.lib.AlpDevAlloc(ALP_DEFAULT,ALP_DEV_DMDTYPE);

        console.log(this);

    }

}

export { ALP };
