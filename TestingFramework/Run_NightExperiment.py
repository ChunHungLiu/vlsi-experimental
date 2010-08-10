import HPWLPlacementChecker
from HPWLPlacementChecker import *

import LRPlacementChecker
from LRPlacementChecker import *

def NightExperiment():
    exp_HPWL_ISPD04 = HPWLPlacementChecker('ISPD04 HPWL experiment', 'hpwl_ispd04.cfg', 'ISPD04.list', "../master logs/HPWL/ispd")
    exp_HPWL_IWLS05 = HPWLPlacementChecker('IWLS05 HPWL experiment', 'hpwl_iwls05.cfg', 'IWLS05.list', "../master logs/HPWL/iwls")
    exp_LR_IWLS05GP = LRPlacementChecker('IWLS05GP LR experiment', 'LR.cfg', 'IWLS_GP_r1511/IWLS_GP.list', "../master logs/LR")

    testRunner = TestRunner()
    testRunner.parameters.experiments.append(exp_HPWL_IWLS05)
    testRunner.parameters.experiments.append(exp_HPWL_ISPD04)
    testRunner.parameters.experiments.append(exp_LR_IWLS05GP)
    testRunner.Run()

NightExperiment()
