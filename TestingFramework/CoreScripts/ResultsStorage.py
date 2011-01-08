import BaseExperiment
from BaseExperiment import *

import Emailer
from Emailer import *

class ResultsStorage:
    experimentResults = {} #experiment: result

    def __init__(self):
        pass

    def AddExperimentResult(self, experiment, result):
        if (self.experimentResults.has_key(experiment)):
            print("Error: the result for experiment %s is already stored" % (experiment.name))

        self.experimentResults[experiment] = result

    def AsString(self):
        result = ""

        for experiment in list(self.experimentResults.keys()):
            result += ("%s:\n%s\n" % (experiment.name,\
                                      self.experimentResults[experiment].AsString()))

        return result

    def PrintResults(self):
        print("Results:\n%s" % (self.AsString()))

    def SendResults(self, emailer):
        attachmentFiles = []

        for experiment in list(self.experimentResults.keys()):
            resultFile = self.experimentResults[experiment].resultFile

            if (resultFile != ""):
                attachmentFiles.append(resultFile)

        emailer.PrepareAndSendMailIfRequired(self.AsString(), attachmentFiles)