import subprocess
import sys
import os

import datetime
from datetime import date
import time

from Emailer import *
from SvnWorker import *
from ReportCreator import *

import CoreFunctions
from CoreFunctions import *

import Parameters
from Parameters import *

import BaseExperiment
from BaseExperiment import *

TERMINATED = 'Terminated'

class TestRunner:
    parameters = ''
    errors    = ''
    experimentResult = OK

    experimentsToCompare = {} #Group of experiments. Theit results will be compared

    newBenchmarks        = ''
    failedBenchmarks     = ''
    terminatedBenchmarks = ''

    nOkBenchmarks         = 0
    nNewBenchmarks        = 0
    nFailedenchmarks      = 0
    nChangedBenchmarks    = 0
    nTerminatedBenchmarks = 0

    def __init__(self, parameters = TestRunnerParameters()):
        self.parameters = parameters
        self.parameters.experiments = []

    def SendErrorMessageAndExit(self, subject, text, attachmentFiles = []):
        emailer = Emailer()
        emailer.PrepareAndSendMailIfRequired(subject, text, attachmentFiles)
        exit(1)

    def BuildSln(self, slnPath, mode = "Rebuild"):
        print('Building solution...')
        res  = 0
        args = [Tools.MSBuild, slnPath, '/t:' + mode, '/p:Configuration=Release']

        try:
            res = subprocess.call(args)

        except WindowsError:
            self.SendErrorMessageAndExit('Night experiments', 'Error: can not call' + Tools.MSBuild)

        if (res != 0):
            print('Build failed!')
            buildLog = GeneralParameters.buildLog
            self.SendErrorMessageAndExit('Night experiments', 'build failed', [buildLog])

    def ExtractBenchmarkList(self, benchmarksListPath):
        benchmarks = (open(benchmarksListPath).read()).split('\n')

        # Perform filtering of empty lines and commented by # benchmarks
        benchmarks = [x for x in benchmarks if not x.strip().startswith('#')]
        benchmarks = [x for x in benchmarks if len(x.strip())]

        return benchmarks

    def PrintXXXBenchmarks(self, status, nXXXBenchmarks, benchmarks = ''):
        if (nXXXBenchmarks == 0):
            return ('')

        printStr = status + str(nXXXBenchmarks)

        if (benchmarks != ''):
            printStr += '(' + benchmarks + ')'

        return (printStr + '\n')

    def Append(self, newExperiment):
         self.parameters.experiments.append(newExperiment)
         return 0

    def AddExperimentToGroup(self, newExperiment):
        if (self.Append(newExperiment) != 0):
            return

        if (self.experimentsToCompare != {}):
            groupExp = list(self.experimentsToCompare.keys())[0]

            if (newExperiment.benchmarks != groupExp.benchmarks):
                print("list files are not equal")
                exit(1)

            if (newExperiment.metrics != groupExp.metrics):
                print("metrics are not equal")
                exit(1)

        self.experimentsToCompare[newExperiment] = {}

    def CompareExperimentsInGroup(self, resultFileName):
        groupExp = list(self.experimentsToCompare.keys())[0]
        metrics  = groupExp.metrics

        #Create header of the table
        #---First string of the header------------------
        cols = []
        cols.append(END_OF_COLUMN)
        cols.append('INIT')

        for col in range(len(metrics)):
            cols.append(END_OF_COLUMN)

        for experiment in self.experimentsToCompare.keys():
            cols.append(END_OF_COLUMN)
            cols.append(experiment.name)

            for col in range(len(metrics)):
                cols.append(END_OF_COLUMN)

        WriteStringToFile(cols, resultFileName)

        #---Second string of the header------------------
        cols = ['benchmark']
        cols.append(END_OF_COLUMN)

        for row in range(len(metrics)):
            cols.append(metrics[row])
            cols.append(END_OF_COLUMN)

        cols.append(END_OF_COLUMN)

        for col in range(len(self.experimentsToCompare.keys())):
            for row in range(len(metrics)):
                cols.append(metrics[row] + '%')
                cols.append(END_OF_COLUMN)

            cols.append(END_OF_COLUMN)

        WriteStringToFile(cols, resultFileName)

        #------Print results-------------------------------------
        for benchmark in self.experimentsToCompare[groupExp].keys():
            initialMetrics = []
            bestMetrics    = [1000000 for i in range(len(metrics))]
            bestMetricsIdx = [0 for i in range(len(metrics))]

            cols = [benchmark]
            cols.append(END_OF_COLUMN)
            initialIdx = len(cols) #index for metrics on 'INIT' stage

            #Reserve positions for each metric on 'INIT' stage
            for col in range(len(metrics)):
                cols.append('N/A')
                cols.append(END_OF_COLUMN)

            cols.append(END_OF_COLUMN)

            #TODO: mark experiment with best result

            for experiment in self.experimentsToCompare.keys():
                resultValues  = self.experimentsToCompare[experiment][benchmark]
                finalStageIdx = len(experiment.stages) - 1

                if (resultValues == []):
                    for col in range(len(metrics)):
                        cols.append('N/A')
                        cols.append(END_OF_COLUMN)

                else:
                    #if 'INITIAL' metrics haven't been printed yet
                    if (initialMetrics == []):
                        #take them from the table of this experiment
                        for col in range(len(metrics)):
                            cols[initialIdx + 2 * col] = str(resultValues[0][col])
                            initialMetrics.append(resultValues[0][col])

                    #else compare 'INITIAL' metrics
                    else:
                        for col in range(len(metrics)):
                            cmpResult = CompareValues(initialMetrics[col], resultValues[0][col])

                            if (cmpResult == 'notEqual'):
                                print('Error: not equal Init metrics')

                    for col in range(len(metrics)):
                        percent = 100 * resultValues[finalStageIdx][col] / initialMetrics[col]
                        percentStr = "%.2f" % percent

                        if (percent < bestMetrics[col]):
                            bestMetrics[col]    = percent   #remember best result
                            bestMetricsIdx[col] = len(cols) #and index

                        cols.append(percentStr)
                        cols.append(END_OF_COLUMN)

                cols.append(END_OF_COLUMN)

            for idx in bestMetricsIdx:
                cols[idx] = MarkResultAsBest(cols[idx])

            WriteStringToFile(cols, resultFileName)

    def RunExperiment(self, experiment):
        #self.ExtractBenchmarkList(experiment.benchmarks)

        print('Config: %s' % experiment.cfg)
        print('List:   %s' % experiment.benchmarks)

        reportCreator = ReportCreator(experiment.cfg)
        logFolder     = reportCreator.CreateLogFolder()
        reportTable   = reportCreator.GetReportTableName()
        experiment.CreateEmptyTable(reportTable)

        (areExperimentParametersOk, errors, benchmarks) = experiment.CheckParametersAndPrepareBenchmarks()

        if (errors != ''):
            self.errors += 'Error: ' + errors + '\n'

        if (not areExperimentParametersOk):
            self.experimentResult = FAILED
            return (reportTable)

        self.experimentResult = OK

        for benchmark in benchmarks:
            logFileName = logFolder + "/" + os.path.basename(benchmark) + ".log"
            fPlacerOutput = open(logFileName, 'w')
            resultValues = []

            defFile = "--params.def=" + os.path.dirname(os.path.abspath(experiment.benchmarks)) + "/" + benchmark + ".def"
            params = [GeneralParameters.binDir + "itlPlaceRelease.exe", os.path.abspath(experiment.cfg), defFile, experiment.cmdLine]
            #HACK: ugly hack for ISPD04 benchmarks
            if experiment.cfg.find("ispd04") != -1:
                lefFile = "--params.lef=" + os.path.dirname(os.path.abspath(experiment.benchmarks)) + "/" + benchmark + ".lef"
                params.append(lefFile)

            benchmarkResult = ''

            try:
                p = subprocess.Popen(params, stdout = fPlacerOutput, cwd = GeneralParameters.binDir)

            except WindowsError:
                print('Error: can not call ' + GeneralParameters.binDir + 'itlPlaceRelease.exe')
                self.SendErrorMessageAndExit('Night experiments', 'can not start itlPlaceRelease.exe')

            t_start = time.time()
            seconds_passed = 0

            while(not p.poll() and seconds_passed < self.parameters.maxTimeForBenchmark):
                seconds_passed = time.time() - t_start

            retcode = p.poll()

            if (retcode == None):
                print("Time out on " + benchmark)
                benchmarkResult            = TERMINATED
                self.terminatedBenchmarks  += ' ' + benchmark + ';'
                self.nTerminatedBenchmarks += 1
                self.experimentResult      = TERMINATED
                p.terminate()

                if (self.nTerminatedBenchmarks >= 3):
                    self.errors += 'Reached maximum number of terminated benchmarks\n'
                    return (reportTable)

            else:
                (benchmarkResult, resultValues) = experiment.ParseLogAndFillTable(logFileName, benchmark, reportTable)

            fPlacerOutput.close()
            print(benchmark + ' DONE')

            if (experiment in self.experimentsToCompare):
                self.experimentsToCompare[experiment][benchmark] = resultValues

            if (benchmarkResult == FAILED):
                self.experimentResult  = FAILED
                self.failedBenchmarks  += ' ' + benchmark + ';'
                self.nFailedBenchmarks += 1

            elif (benchmarkResult == CHANGED):
                self.nChangedBenchmarks += 1

                if ((self.experimentResult != FAILED) and (self.experimentResult != TERMINATED)):
                    self.experimentResult = CHANGED

            elif (benchmarkResult == OK):
                self.nOkBenchmarks += 1

            elif (benchmarkResult == NEW):
                self.newBenchmarks  += ' ' + benchmark + ';'
                self.nNewBenchmarks += 1

            #else TERMINATED - do nothing

        return reportTable

    def Run(self):
        cp      = CoolPrinter()
        svn     = SvnWorker()
        emailer = Emailer()

        subject = 'Night experiments'
        text    = ''
        attachmentFiles = []

        cp.CoolPrint('Start')

        if self.parameters.doCheckout:
            cp.CoolPrint('Delete sources and Checkout')
            svn.DeleteSources(GeneralParameters.checkoutPath)
            retcode = 1

            for i in range(10):
                #TODO: implement non HEAD revision
                try:
                    retcode = svn.CheckOut(RepoParameters.srcRepoPath, GeneralParameters.checkoutPath)

                except WindowsError:
                    self.SendErrorMessageAndExit('Night experiments', 'Error: can not call svn.exe')

                if retcode == 0:
                    break

            if retcode != 0:
                text = 'svn error: checkout failed!'
                self.SendErrorMessageAndExit(subject, text, attachmentFiles)

        if self.parameters.doBuild:
            cp.CoolPrint('Build')
            self.BuildSln(GeneralParameters.slnPath)

        for experiment in self.parameters.experiments:
            startTime = GetTimeStamp()
            print('Start time: ' + startTime)
            self.experimentResult = OK
            self.errors = ''

            self.newBenchmarks        = ''
            self.failedBenchmarks     = ''
            self.terminatedBenchmarks = ''

            self.nOkBenchmarks         = 0
            self.nNewBenchmarks        = 0
            self.nChangedBenchmarks    = 0
            self.nFailedBenchmarks     = 0
            self.nTerminatedBenchmarks = 0

            cp.CoolPrint(experiment.name)
            reportTable = self.RunExperiment(experiment)
            #cp.CoolPrint("Sending mail with " + reportTable)

            #subject += ' ' + experiment.name
            #text += experiment.name + ': ' + self.experimentResult
            text += experiment.name + ', ' + os.path.basename(experiment.cfg)
            text += ', ' + os.path.basename(experiment.benchmarks) + ' ('
            nBenchmarks = self.nOkBenchmarks + self.nChangedBenchmarks + self.nFailedBenchmarks + self.nTerminatedBenchmarks + self.nNewBenchmarks
            text += str(nBenchmarks) + ' benchmark(s)):\n'
            text += 'Start time: ' + startTime + '\n'
            text += self.PrintXXXBenchmarks('Ok:         ', self.nOkBenchmarks)
            text += self.PrintXXXBenchmarks('Changed:    ', self.nChangedBenchmarks)
            text += self.PrintXXXBenchmarks('Failed:     ', self.nFailedBenchmarks, self.failedBenchmarks)
            text += self.PrintXXXBenchmarks('Terminated: ', self.nTerminatedBenchmarks, self.terminatedBenchmarks)
            text += self.PrintXXXBenchmarks('NEW:        ', self.nNewBenchmarks, self.newBenchmarks)

            text += self.errors
            text += '\n'

            if (self.experimentResult == CHANGED):
                attachmentFiles.append(reportTable)
            #emailer.SendResults(experiment, reportTable, self.experimentResult)

        text += 'Finished at ' + GetTimeStamp()
        print(text)
        emailer.PrepareAndSendMailIfRequired(subject, text, attachmentFiles)

        if (len(self.experimentsToCompare) > 1):
            cp.CoolPrint('Comaparing experiments')
            reportCreator = ReportCreator('Comaparing')
            logFolder     = reportCreator.CreateLogFolder()
            cmpFileName   = reportCreator.GetReportTableName()
            self.CompareExperimentsInGroup(cmpFileName)

        cp.CoolPrint('Finish')
