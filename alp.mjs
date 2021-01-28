import ffi from 'ffi-napi';
import path from 'path';
const __dirname = path.resolve();
import fs from 'fs';

const ALP_DEFAULT  = 0;
const ALP_DEV_DMDTYPE = 2021;

/*var c = [];
c[1001] = "ALP_NOT_ONLINE";*/

//DMD type (probable) "ALP_DMDTYPE_1080P_095A"

//https://github.com/wavefrontshaping/ALP4lib

class ALP {
    constructor(){
        this.lib = ffi.Library(path.join(__dirname, './api/alp42'), {
            'AlpDevAlloc': ['long', ['long','long']],
            'AlpDevInquire': ['long', ['long','long']],
            'AlpSeqAlloc':  ['long',['long','long','long']]
        });

        this.nSizeX = 1920;
        this.nSizeY = 1080;
    }

    init(){
        this.alp_id = this.lib.AlpDevAlloc(ALP_DEFAULT,ALP_DEFAULT);
        this.dmd_type = this.lib.AlpDevAlloc(ALP_DEFAULT,ALP_DEV_DMDTYPE);

        //console.log(this);

    }

    storeImg(images, bitDepth = 1){
        //bitDepth : 1 = on/off and 8 = 256 pwm grayscale levels
        var nbImg = images.length;

        //Alloc Memory for Sequence
        this.lastDDRseq = this.lib.AlpSeqAlloc(ALP_DEFAULT,bitDepth,nbImg);


    }

}

class ALPimage {
    constructor(u8aImage){
        if(u8aImage.constructor !== Uint8Array){
            console.log("Image should be a Uint8Array");
            process.exit(1);
        }
        this.bin = u8aImage;
        this.header = new Int16Array([1920, 1080, 2, 1]);
    }

    genfile(path = 'bin/'){
        console.log("GenFile");
        fs.writeFileSync(path+"flash.bin",this.bin);
    }

    base64(){
        return btoa(this.bin);
    }
}

export { ALP, ALPimage };
