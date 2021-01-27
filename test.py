import numpy as np
from ALP4 import *

# Load the Vialux .dll
DMD = ALP4(version = '4.2', libDir = 'api/')
# Initialize the device
#DMD.Initialize()