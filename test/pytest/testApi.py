import ctypes
import unittest
import math
import parfis as pfs
from parfis import Parfis

# from parfis import PyCfgData

class TestApi(unittest.TestCase):

    stateType = 'double'

    @classmethod
    def setUpClass(cls):
        print(f"setUpClass: {TestApi.stateType}")
        Parfis.load_lib(stateType=TestApi.stateType)

    def tearDown(self) -> None:
        Parfis.deleteAll()

    def test_info(self) -> None:
        '''No Parfis objects, just check general program settings.
        '''
        parInfo = Parfis.info()
        self.assertTrue("Parfis object count = 0" in parInfo)

    def test_new_parfis(self) -> None:
        '''Create new parfis object
        '''
        parId = Parfis.newParfis()
        parInfo = Parfis.info()
        self.assertTrue("Parfis object count = 1" in parInfo)
        self.assertTrue(f"Parfis object id = [{parId}]" in parInfo)

    def test_load_different_lib(self) -> None:
        '''Change lib and back again, check parfis.Type.state_t
        '''
        Parfis.newParfis()
        parInfo = Parfis.info()
        self.assertTrue(f"parfis::state_t = {TestApi.stateType}" in parInfo)
        Parfis.deleteAll()
        for i in range(2):
            if TestApi.stateType == 'float':
                TestApi.stateType = 'double'
            else: 
                TestApi.stateType = 'float'
            Parfis.load_lib(stateType=TestApi.stateType)            
            Parfis.newParfis()
            parInfo = Parfis.info()
            self.assertTrue(f"parfis::state_t = {TestApi.stateType}" in parInfo)
            id = Parfis.newParfis()
            Parfis.loadCfgData(id)
            Parfis.loadSimData(id)
            Parfis.runCommandChain(id, "create")
            Parfis.setPySimData(id)
            ptrSimData = Parfis.getPySimData(id)
            # Now check if state_t is set as it should
            if TestApi.stateType == 'float':
                self.assertTrue(pfs.Type.state_t, ctypes.c_float)
                self.assertEqual(pfs.Vec3D_float, type(ptrSimData[0].stateVec.ptr[0].pos))
            else:
                self.assertTrue(pfs.Type.state_t, ctypes.c_double)
                self.assertEqual(pfs.Vec3D_double, type(ptrSimData[0].stateVec.ptr[0].pos))
            Parfis.deleteAll()

    def test_delete_parfis(self) -> None:
        '''Create four new parfis objects and delete one of them
        '''
        parIds = []
        parIds.append(Parfis.newParfis())
        parIds.append(Parfis.newParfis())
        parIds.append(Parfis.newParfis())
        parIds.append(Parfis.newParfis())
        parInfo = Parfis.info()
        self.assertTrue("Parfis object count = 4" in parInfo)
        self.assertTrue(f"Parfis object id = {parIds}" in parInfo)
        Parfis.deleteParfis(parIds[1])
        parInfo = Parfis.info()
        self.assertTrue("Parfis object count = 3" in parInfo)
        self.assertTrue(f"Parfis object id = [{parIds[0]}, {parIds[2]}, {parIds[3]}]" in parInfo)

    def test_get_cfg_data(self) -> None:
        '''Check if cfg data getter works
        '''
        parId = Parfis.newParfis()
        Parfis.loadCfgData(parId)
        Parfis.setPyCfgData(parId)
        ptrCfgData = Parfis.getPyCfgData(parId)
        self.assertEqual(1.0,ptrCfgData.timestep)
        self.assertEqual((0.02, 0.02, 0.4),ptrCfgData.geometrySize[0].asTuple())
        self.assertEqual((0.001, 0.001, 0.001),ptrCfgData.cellSize[0].asTuple())
        self.assertEqual((20, 20, 400),ptrCfgData.cellCount[0].asTuple())
        self.assertEqual(1,ptrCfgData.specieNameVec.size)
        self.assertEqual(["a"],ptrCfgData.specieNameVec.asList())

    def test_reconfiguration(self) -> None:
        '''Check reconfiguration of specie data
        '''
        id = Parfis.newParfis()
        Parfis.loadCfgData(id)
        Parfis.setPyCfgData(id)
        ptrCfgData = Parfis.getPyCfgData(id)
        self.assertEqual(1,ptrCfgData.specieNameVec.size)
        self.assertEqual(["a"],ptrCfgData.specieNameVec.asList())
        Parfis.setConfig(id, "particle.specie = [electron, atom] <parfis::Param>")
        Parfis.setConfig(id, "particle.specie.electron = [statesPerCell, timestepRatio, amuMass, eCharge] <parfis::Param>")
        Parfis.setConfig(id, "particle.specie.electron.statesPerCell = 10 <int>")
        Parfis.setConfig(id, "particle.specie.electron.timestepRatio = 1 <int>")
        Parfis.setConfig(id, "particle.specie.electron.amuMass = 0.00054858 <double>")
        Parfis.setConfig(id, "particle.specie.electron.eCharge = -1 <int>")
        Parfis.setConfig(id, "particle.specie.atom = [statesPerCell, timestepRatio, amuMass, eCharge] <parfis::Param>")
        Parfis.setConfig(id, "particle.specie.atom.statesPerCell = 10 <int>")
        Parfis.setConfig(id, "particle.specie.atom.timestepRatio = 1 <int>")
        Parfis.setConfig(id, "particle.specie.atom.amuMass = 4 <double>")
        Parfis.setConfig(id, "particle.specie.atom.eCharge = 0 <int>")
        Parfis.loadCfgData(id)
        Parfis.setPyCfgData(id)
        ptrCfgData = Parfis.getPyCfgData(id)
        self.assertEqual(2,ptrCfgData.specieNameVec.size)
        self.assertEqual("electron",ptrCfgData.specieNameVec.ptr[0].decode())
        self.assertEqual("atom",ptrCfgData.specieNameVec.ptr[1].decode())
        # self.assertEqual(["electron", "atom"],ptrCfgData.specieNameVec.asList())

    def test_createCommandChain(self) -> None:
        '''Check creation of cells and states
        '''
        id = Parfis.newParfis()
        cfgStr = """
            particle.specie = [electron] <parfis::Param>
            particle.specie.electron = [statesPerCell, timestepRatio, amuMass, eCharge] <parfis::Param>
            particle.specie.electron.statesPerCell = 10 <int>
            particle.specie.electron.timestepRatio = 1 <int>
            particle.specie.electron.amuMass = 0.00054858 <double>
            particle.specie.electron.eCharge = -1 <int>
        """
        Parfis.setConfig(id, cfgStr)
        Parfis.loadCfgData(id)
        Parfis.loadSimData(id)
        Parfis.runCommandChain(id, "create")
        
        Parfis.setPyCfgData(id)
        ptrCfgData = Parfis.getPyCfgData(id)
        rSqPi = math.pi * (0.5*ptrCfgData[0].geometrySize[0].x)**2
        volCyl = rSqPi *ptrCfgData.geometrySize[0].z
        volCylAbs = (
           ptrCfgData.geometrySize[0].x*
           ptrCfgData.geometrySize[0].y*
           ptrCfgData.geometrySize[0].z
        )
        volRatio = volCyl/volCylAbs

        Parfis.setPySimData(id)
        ptrSimData = Parfis.getPySimData(id)
        self.assertEqual("electron",ptrSimData.specieVec.ptr[0].name.decode())
        numStatesCyl =ptrSimData.stateVec.size
        numCells =ptrSimData.cellIdVec.size
        numStatesCub = numCells *ptrSimData.specieVec.ptr[0].statesPerCell
        stateRatio = numStatesCyl/numStatesCub
        relativeDifference = abs(volRatio - stateRatio)/(0.5*(volRatio + stateRatio))
        precission = 5.0e-3
        self.assertLess(relativeDifference, precission)

    def test_evolveCommandChain(self) -> None:
        '''Push states for 100 times
        '''
        id = Parfis.newParfis()
        Parfis.loadCfgData(id)
        Parfis.loadSimData(id)
        Parfis.runCommandChain(id, "create")
        retval = 0
        for i in range(100):
            retval = Parfis.runCommandChain(id, "evolve")
            if retval != 0: 
                break
        self.assertEqual(retval, 0)

if __name__ == '__main__':

    unittest.main()