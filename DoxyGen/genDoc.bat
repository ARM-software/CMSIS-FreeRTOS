@echo off

REM ====================================================================================
REM Batch file for generating
REM
REM Author  : 
REM Date    :  26th January 2017
REM Version : 1.0.0
REM Company : ARM 
REM
REM 
REM Command syntax: genDoc.bat
REM
REM  Version: 1.0 Initial Version.
REM ====================================================================================

SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

REM -- Delete previous generated HTML files ---------------------
  ECHO.
  ECHO Delete previous generated HTML files

REM -- Remove generated doxygen files ---------------------
PUSHD ..\Documentation
RMDIR /S /Q General
REM FOR %%A IN (Core, DAP, Driver, DSP, General, Pack, RTOS, RTOS2, SVD) DO IF EXIST %%A (RMDIR /S /Q %%A)
POPD

REM -- Generate New HTML Files ---------------------
  ECHO.
  ECHO Generate New HTML Files

pushd General
CALL doxygen_general.bat
popd

REM -- Copy search style sheet ---------------------
  ECHO.
  ECHO Copy search style sheets
copy /Y Doxygen_Templates\search.css ..\Documentation\General\html\search\. 
  
:END
  ECHO.
REM done
