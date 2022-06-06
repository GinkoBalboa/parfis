import ctypes
import unittest
import math
import parfis as pfs

class TestTools(unittest.TestCase):

    stateType = 'double'

    def test_generateConstantCrossSection(self) -> None:
        '''Use tools to generate simple collisional cross section'''

        def sigma_e(x):
            return 6

        def sigma_i(x):
            if x<0.2:
                return 0
            else:
                return 3
                
        ranges = [   1,   10,  100, 1000, 10000, 342000]
        nbins =  [1000, 1000, 1000, 1000,  1000,   3000]
        pfs.generateCrossSection(sigma_e, ranges, nbins, file="./data/cross_sections/simple_e.csv")
        pfs.generateCrossSection(sigma_i, ranges, nbins, file="./data/cross_sections/simple_i.csv")
    
if __name__ == '__main__':

    unittest.main()