@echo off

REM 
REM RebuildLibsForWildcard Libname ARopts Wildcard
REM

FOR %%F IN (%3) do (
  REM ECHO %GNU_TOOLS_BIN%\%GNU_TARGET%-ar %1 %2 %%F
  %GNU_TOOLS_BIN%\%GNU_TARGET%-ar %1 -v %2 %%F
)
