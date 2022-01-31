import unittest
from parfis import Parfis
# from parfis import PyCfgData

class TestApi(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        Parfis.load_lib()

    def setUp(self) -> None:
        Parfis.load_lib()

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
        '''Check cfg data getter works
        '''
        parId = Parfis.newParfis()
        ptrCfgData = Parfis.getCfgData(parId)
        self.assertEqual(1.0, ptrCfgData[0].timestep)
        self.assertEqual(0.02, ptrCfgData[0].geometrySize[0].x)
        self.assertEqual(0.02, ptrCfgData[0].geometrySize[0].y)
        self.assertEqual(0.4, ptrCfgData[0].geometrySize[0].z)

if __name__ == '__main__':
    unittest.main()