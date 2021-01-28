import { ALP } from './alp.mjs';
import http from 'http';
import open from 'open';
import fs from 'fs';

var alp = new ALP();
alp.init();
var data = [];

open('http://localhost:8080/');
http.createServer(function (req, res) {
    try {
        console.log(req.method);
        if (req.method === 'GET') {
            if (req.url == '/') {
                var d = fs.readFileSync('editor.htm', 'utf-8');
                unescape(d);
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
        else {
            if (req.url == '/launch') {
                req.on('data', (chunk) => {
                    data.push(chunk);
                }).on('end', () => {
                    var u8a = new Uint8Array(Buffer.concat(data));
                    data = [];
                    console.log(u8a);
                });

            }
        }
    } catch (err) {
        console.log(err);
        res.write('error');
    }
    res.end();

}).listen(8080);

