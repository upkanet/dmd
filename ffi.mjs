import { ALP } from './alp.mjs';
import http from 'http';
import open from 'open';
import fs from 'fs';
import atob from 'atob';

var alp = new ALP();
alp.init();

open('http://localhost:8080/');
http.createServer(function (req, res) {
    try {
        console.log(req.method);
        if(req.method === 'GET'){
            if (req.url == '/') {
                var d = fs.readFileSync('editor.htm', 'utf-8');
                res.write(d);
            }
            else if (req.url == '/favicon.ico') {
                var d = fs.readFileSync(req.url.substr(1));
                res.write(d);
            }
            else {
                var d = fs.readFileSync(req.url.substr(1), 'utf-8');
                res.write(d);
            }
        }
        else{
            if (req.url == '/launch') {
                req.on('data', (c) => {
                    var abuff = _base64ToArrayBuffer(c.toString());
                    fs.writeFileSync('bin/postflash.bin',abuff);
                });
            }
        }
    } catch (err) {
        console.log('error');
        res.write('error');
    }
    res.end();

}).listen(8080);


function _base64ToArrayBuffer(base64) {
    var binary_string = atob(base64);
    var len = binary_string.length;
    var bytes = new Uint8Array(len);
    for (var i = 0; i < len; i++) {
        bytes[i] = binary_string.charCodeAt(i);
    }
    return bytes;
}

function buf2hex(buffer) { // buffer is an ArrayBuffer
    return Array.prototype.map.call(new Uint8Array(buffer), x => ('00' + x.toString(16)).slice(-2)).join('');
  }