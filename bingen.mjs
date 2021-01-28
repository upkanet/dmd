import fs from 'fs';

const screenWidth = 1920;
const screenHeight = 1080;
const nFrames = 2;
const binPath = 'bin/flash.bin';

//Header
const header = new Int16Array([screenWidth, screenHeight, nFrames, 1]);

const frameBlack = prefill(0,screenWidth,screenHeight);
const frameWhite = prefill(255,screenWidth,screenHeight);

function prefill(v,cols,rows){
    return new Uint8Array(Array(rows * cols).fill(v));
}

fs.writeFileSync(binPath,header);
fs.appendFileSync(binPath,frameWhite);
fs.appendFileSync(binPath,frameBlack);