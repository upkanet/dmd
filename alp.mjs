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
        this.buffer = u8aImage;
        this.bin = this.gammaOnly();
        this.header = new Int16Array([1920, 1080, 2, 1]);
        this.blackFrame = new Uint8Array(Array(1920 * 1080).fill(0));
        this.path = 'bin/flash.bin';
    }

    base64(){
        return btoa(this.bin);
    }

    gammaOnly(){
        var g = [];
        this.buffer.forEach((e,i) => {
            if(i%4 == 3){
                g.push(e);
            }
        });
        return new Uint8Array(g);
    }

    save(){
        fs.writeFileSync(this.path,this.header);
        fs.appendFileSync(this.path,this.bin);
        fs.appendFileSync(this.path,this.blackFrame);
    }
}

export { ALP, ALPimage };
