import ctypes
from sre_constants import ASSERT
import unittest
import math
from parfis import Parfis
import parfis as pfs

class TestTools(unittest.TestCase):

    stateType = 'double'
    
    @classmethod
    def setUpClass(cls):
        print(f"setUpClass: {TestTools.stateType}")
        Parfis.load_lib(stateType=TestTools.stateType)

    def tearDown(self) -> None:
        Parfis.deleteAll()

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
        id = Parfis.newParfis()
        Parfis.setConfigFromFile(id, "./data/config_files/test_physics_gasCollisionDefinition.ini")
        # Set and load cfg data, set and load sim data
        Parfis.loadCfgData(id)
        Parfis.loadSimData(id)
        Parfis.setPySimData(id)
        Parfis.setPyCfgData(id)
        ptrCfgData = Parfis.getPyCfgData(id)
        ptrSimData = Parfis.getPySimData(id)
        self.assertEqual("a", ptrCfgData.specieNameVec.ptr[0].decode())
        self.assertEqual("bck", ptrCfgData.gasNameVec.ptr[0].decode())
        self.assertEqual(["a.elastic", "a.inelastic"], ptrCfgData.gasCollisionNameVec.asList())
        self.assertEqual([
            "./data/cross_sections/simple_e.csv", 
            "./data/cross_sections/simple_i.csv"], ptrCfgData.gasCollisionFileNameVec.asList())
        

    
if __name__ == '__main__':

    unittest.main()