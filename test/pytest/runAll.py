import unittest
import sys
from testApi import TestApi

def suite(stateType):
    suite = unittest.TestSuite()
    testList = []
    for dirStr in dir(TestApi):
        if "test_" in dirStr:
            testList.append(dirStr)
    TestApi.stateType = stateType
    for testName in testList:
        suite.addTest(TestApi(testName))
    return suite

if __name__ == '__main__':

    runner = unittest.TextTestRunner(verbosity=2)
    if len(sys.argv) > 1:
        runner.run(suite(sys.argv[1]))
    else:
        runner.run(suite('double'))