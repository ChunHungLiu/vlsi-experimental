import CoreScripts
from CoreScripts import *

import Parameters
from Parameters import *

class Experiment_Buffering(BaseExperiment):
    def CreateEmptyTable(self, reportTable):
        po = open(reportTable, 'w')
        po.write('Benchmark;bbHPWL;abHPWL;bbTWL;abTWL;bbTNS;abTNS;bbWNS;abWNS\n')
        po.close()

    def ParseLogAndFillTable(self, logName, benchmark, reportTable):
        #get metrics
        parser = LogParser(logName)
        bbHPWL = parser.GetFromTable('INIT', 4)
        abHPWL = parser.GetFromTable('LEGB', 4)
        bbTWL = parser.GetFromTable('INIT', 5)
        abTWL = parser.GetFromTable('LEGB', 5)
        bbTNS = parser.GetFromTable('INIT', 6)
        abTNS = parser.GetFromTable('LEGB', 6)
        bbWNS = parser.GetFromTable('INIT', 7)
        abWNS = parser.GetFromTable('LEGB', 7)

        #print metrics
        printStr = benchmark + ';'
        printStr += str(bbHPWL) + ';'
        printStr += str(abHPWL) + ';'
        printStr += str(bbTWL) + ';'
        printStr += str(abTWL) + ';'
        printStr += str(bbTNS) + ';'
        printStr += str(abTNS) + ';'
        printStr += str(bbWNS) + ';'
        printStr += str(abWNS) + ';'
        printStr += '\n'
        print(printStr)

        #write metrics
        table = open(reportTable, 'a')
        table.write(printStr)
        table.close()

def test():
    testRunner = TestRunner()

    e = Experiment_Buffering('IWLS05 buffering experiment (GBL)', 'kirill_buffering.cfg', 'IWLS05_GP.list')
    testRunner.parameters.experiments.append(e)

    testRunner.Run()

#test()