import fs from 'fs';
import Jimp from 'jimp';

/*
var buff = fs.readFileSync('flash_toutImplant.bin.example');

var u8a = new Uint8Array(buff);

fs.writeFileSync('verif.txt', '');

var k = 0;

u8a.forEach((e) => {
    if (e == 0) {
        //fs.appendFileSync('verif.txt', '.');
        k++;
    }
    else if (e == 255) {
        //fs.appendFileSync('verif.txt', '#');
        k++;
    }
});

console.log(k, k/1024);
*/
var buff = fs.readFileSync('bin/flash_toutImplant.bin');
var u8a = new Uint8Array(buff);
var a = [];
var b = [];

u8a.forEach( (e,i) => {
    if(i<8){
        console.log("out",i,e);
    }
    else if(i < 1024*768+8){
        a.push(e);//R
        a.push(e);//G
        a.push(e);//B
        a.push(e);//A
    }
    else{
        b.push(e);//R
        b.push(e);//G
        b.push(e);//B
        b.push(e);//A
    }
});

console.log(a.length,b.length);

var imAbuff = new Uint8Array(a);
var imBbuff = new Uint8Array(b);

var ja = new Jimp({data: imBbuff, width:1024, height: 768});
ja.write('test.png');
