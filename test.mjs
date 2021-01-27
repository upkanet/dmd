import pyb from 'python-bridge'
var py = pyb.pythonBridge();

py.ex`from ALP4 import *`


//C:\Program Files\ALP-4.2\ALP-4.2 high-speed API
//py.ex`DMD = ALP4(version = '4.2', libDir = 'C:/Program Files/ALP-4.2/ALP-4.2 high-speed API')`
//py.ex`DMD.Initialize()`


py.end();