import unittest
from parfis import Parfis
# from parfis import PyCfgData

class TestApi(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        Parfis.load_lib()

    # def setUp(self) -> None:
    #     Parfis.load_lib()

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
        Parfis.deleteParfis(parId)

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
        self.assertEqual(1.0, ptrCfgData[0].timestep)
        self.assertEqual((0.02, 0.02, 0.4), ptrCfgData[0].geometrySize[0].asTuple())
        self.assertEqual((0.001, 0.001, 0.001), ptrCfgData[0].cellSize[0].asTuple())
        self.assertEqual((20, 20, 400), ptrCfgData[0].cellCount[0].asTuple())
        self.assertEqual(1, ptrCfgData[0].specieNameVec.size)
        self.assertEqual(["a"], ptrCfgData[0].specieNameVec.asList())

    def test_reconfiguration(self) -> None:
        '''Check reconfiguration of specie data
        '''
        id = Parfis.newParfis()
        Parfis.loadCfgData(id)
        Parfis.setPyCfgData(id)
        ptrCfgData = Parfis.getPyCfgData(id)
        self.assertEqual(1, ptrCfgData[0].specieNameVec.size)
        self.assertEqual(["a"], ptrCfgData[0].specieNameVec.asList())
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
        self.assertEqual(2, ptrCfgData[0].specieNameVec.size)
        self.assertEqual(["electron", "atom"], ptrCfgData[0].specieNameVec.asList())

if __name__ == '__main__':
    unittest.main()