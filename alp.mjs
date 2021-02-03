import ffi from 'ffi-napi';
import path from 'path';
const __dirname = path.resolve();
import fs from 'fs';
import Jimp from 'jimp';
import { spawn } from 'child_process';

const ALP_DEFAULT = 0;
const ALP_DEV_DMDTYPE = 2021;

/*var c = [];
c[1001] = "ALP_NOT_ONLINE";*/

//DMD type (probable) "ALP_DMDTYPE_768P_095A"
//https://github.com/wavefrontshaping/ALP4lib/blob/master/src/ALP4.py#L396

//https://github.com/wavefrontshaping/ALP4lib

class ALP {
    constructor() {
        this.lib = ffi.Library(path.join(__dirname, './api/alp42'), {
            'AlpDevAlloc': ['long', ['long', 'long', 'pointer']],
            'AlpDevInquire': ['long', ['long', 'long']],
            'AlpSeqAlloc': ['long', ['long', 'long', 'long']]
        });

        this.nSizeX = 1024;
        this.nSizeY = 768;
    }

    init() {
        this.alp_id = this.lib.AlpDevAlloc(ALP_DEFAULT, ALP_DEFAULT);
        this.dmd_type = this.lib.AlpDevAlloc(ALP_DEFAULT, ALP_DEV_DMDTYPE);

        //console.log(this);

    }

    storeImg(images, bitDepth = 1) {
        //images : array of base64
        //bitDepth : 1 = on/off and 8 = 256 pwm grayscale levels
        var nbImg = images.length;

        //Alloc Memory for Sequence
        this.lastDDRseq = this.lib.AlpSeqAlloc(ALP_DEFAULT, bitDepth, nbImg);


    }

}

class ALPimage {
    constructor(u8aImage) {
        if (u8aImage.constructor !== Uint8Array) {
            console.log("Image should be a Uint8Array");
            process.exit(1);
        }
        this.buffer = u8aImage;
        this.bin = this.gammaOnly();
        this.header = new Int16Array([1024, 768, 2, 1]);
        this.blackFrame = new Uint8Array(Array(1024 * 768).fill(0));
        this.path = 'bin/flash.bin';
    }

    base64() {
        return btoa(this.bin);
    }

    gammaOnly() {
        var g = [];
        this.buffer.forEach((e, i) => {
            if (i % 4 == 3) {
                g.push(e);
            }
        });
        return new Uint8Array(g);
    }

    save() {
        fs.writeFileSync(this.path, this.header);
        console.log("Header:", this.header.length);
        fs.appendFileSync(this.path, this.blackFrame);
        console.log("Black Frame:", this.blackFrame.length);
        fs.appendFileSync(this.path, this.bin);
        console.log("Image:", this.bin.length);
    }
}

async function loadBIN(binpath = "bin/flash.bin") {
    var buff = fs.readFileSync(binpath);
    var u8a = new Uint8Array(buff);
    var a = [];
    var b = [];
    u8a.forEach((e, i) => {
        if (i < 8) {
            //console.log("out",i,e);
        }
        else if (i < 1024 * 768 + 8) {
            /*a.push(e);//R
            a.push(e);//G
            a.push(e);//B
            a.push(e);//A*/
        }
        else {
            b.push(e);//R
            b.push(e);//G
            b.push(e);//B
            b.push(e);//A
        }
    });

    console.log(a.length, b.length);

    //var imAbuff = new Uint8Array(a);
    var imBbuff = new Uint8Array(b);
    var jb = new Jimp({ data: imBbuff, width: 1024, height: 768 });
    var b64 = await jb.getBase64Async(Jimp.MIME_PNG);
    return b64;
}

var axodmd;

function playDMD() {
    console.log('\x1b[36m%s\x1b[0m',"Launch AxoDMD player");
    axodmd = spawn('exe/axodmd.exe',['bin',1000,1000,8], {
        stdio: 'inherit' // Will use process .stdout, .stdin, .stderr
    });
    axodmd.on('error',(e) => {
        console.err('[ERROR] Axorus DMD',e);
    });
    axodmd.on('exit',() => {
        console.log('\x1b[36m%s\x1b[0m','Axorus DMD has finished');
    });
}

function stopDMD(){
    axodmd.kill('SIGINT');
    axodmd.kill('SIGINT');
}


export { ALP, ALPimage, loadBIN, playDMD, stopDMD };
