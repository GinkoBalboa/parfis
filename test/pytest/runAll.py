import unittest
import sys
from testApi import TestApi
from testTools import TestTools

def suite(stateType: (str) = "double", testClass: (str) = None):
    """Testing suite creates test groups.

    Args:
        stateType (str): Testing with "double" or "float" lib.
        testGrup (str): If test class is defined use only that test class.
            Test groups are named as files ("TestApi", "TestTools")
    """
    suite = unittest.TestSuite()

    if testClass == None or testClass == "TestApi":
        testList = []
        for dirStr in dir(TestApi):
            if "test_" in dirStr:
                testList.append(dirStr)
        for testName in testList:
            suite.addTest(TestApi(testName))
        TestApi.stateType = stateType

    if testClass == None or testClass == "TestTools":
        testList = []
        for dirStr in dir(TestTools):
            if "test_" in dirStr:
                testList.append(dirStr)
        for testName in testList:
            suite.addTest(TestTools(testName))

    return suite

if __name__ == '__main__':

    runner = unittest.TextTestRunner(verbosity=2)
    ret = 0
    if len(sys.argv) == 1:
        ret = not runner.run(suite('double')).wasSuccessful()
    elif len(sys.argv) == 2:
        ret = not runner.run(suite(sys.argv[1])).wasSuccessful()
    elif len(sys.argv) == 3:
        ret = not runner.run(suite(sys.argv[1], sys.argv[2])).wasSuccessful()

    sys.exit(ret)