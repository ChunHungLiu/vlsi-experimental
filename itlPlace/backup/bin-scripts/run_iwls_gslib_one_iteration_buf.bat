@echo off
for /f %%b in (.\IWLS05f\original_list.txt) do (
itlPlacerelease.exe Config_for_night_experiments.cfg --params.def=.\IWLS05f\%%b.def --params.techname=IWLS --DesignFlow.nMacroIterations=0 --DesignFlow.LoopRandomPlacement=false --DesignFlow.LoopGlobalPlacement=false --DesignFlow.LoopLR=false --DesignFlow.LoopLegalization=false --DesignFlow.LoopDetailedPlacement=false  --DesignFlow.RandomPlacement=false --DesignFlow.GlobalPlacement=true --DesignFlow.LR=false --DesignFlow.Legalization=true --DesignFlow.DetailedPlacement=true --DesignFlow.Buffering=true --DesignFlow.BufRandomPlacement=false --DesignFlow.BufGlobalPlacement=true --DesignFlow.BufLR=false --DesignFlow.BufLegalization=true --DesignFlow.BufDetailedPlacement=true
set hh=%time:~0,2%
if "%time:~0,1%"==" " set hh=0%hh:~1,1%
copy /Y Replicated_Config_for_night_experiments.cfg .\cfg\%date:~-4,4%%date:~-7,2%%date:~-10,2%%hh%%time:~-8,2%%time:~-5,2%%time:~-2,2%_oneb_%%b.cfg >nul
echo %date% %time% %%b.def completed
)