import unittest
from parfis import Parfis

class TestApi(unittest.TestCase):

    def setUp(self) -> None:
        self.parfis = Parfis()
        Parfis.load_lib()    

    def test_info(self):
        parInfo = Parfis.info()
        print(parInfo)
        self.assertEqual(1, "Parfis object count = 0" in parInfo)

if __name__ == '__main__':
    unittest.main()