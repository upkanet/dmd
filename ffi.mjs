import { ALP, ALPimage, loadBIN, playDMD, stopDMD } from './alp.mjs';
import http from 'http';
import open from 'open';
import fs from 'fs';

//var alp = new ALP();
//alp.init();
var data = [];
var alpimg;

var opbrowser = (process.argv.length > 2) && (process.argv[2] == "o");
if(opbrowser){
    open('http://localhost:8080/');
}
http.createServer(async (req, res) => {
    try {
        console.log(req.method);
        if (req.method === 'GET') {
            if (req.url == '/') {
                res.writeHead(200, {"Content-Type": "text/html; charset=utf-8"});
                var d = fs.readFileSync('editor.htm', 'utf8');
                res.write(d);
            }
            else if (req.url == '/favicon.ico') {
                var d = fs.readFileSync(req.url.substr(1));
                res.write(d);
            }
            else if (req.url == '/save') {
                alpimg.save();
            }
            else if (req.url == '/loadbin') {
                var d = await loadBIN();
                res.write(JSON.stringify(d));
            }
            else if (req.url == '/play') {
                playDMD();
            }
            else if (req.url == '/stop') {
                stopDMD();
            }
            else {
                var d = fs.readFileSync(req.url.substr(1), 'utf-8');
                res.write(d);
            }
        }
        else {
            if (req.url == '/load') {
                req.on('data', (chunk) => {
                    data.push(chunk);
                }).on('end', () => {
                    console.log("Load Image");
                    var u8a = new Uint8Array(Buffer.concat(data));
                    data = [];
                    alpimg = new ALPimage(u8a);
                });
            }
        }
    } catch (err) {
        console.log(err);
        res.write('error');
    }
    res.end();

}).listen(8080);

