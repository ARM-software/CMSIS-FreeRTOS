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

IF EXIST ..\CMSIS\Documentation (
  ECHO -- Documenation folder already exists
  PUSHD ..\CMSIS\Documentation
  FOR %%A IN (General) DO IF EXIST %%A (RMDIR /S /Q %%A)
  POPD
) ELSE (
  ECHO -- Create Documentation folder
  MKDIR ..\CMSIS\Documentation
)

REM -- Generate New HTML Files ---------------------
  ECHO.
  ECHO Generate New HTML Files

PUSHD General
CALL doxygen_general.bat
POPD

REM -- Copy search style sheet ---------------------
  ECHO.
  ECHO Copy search style sheets
COPY /Y Doxygen_Templates\search.css ..\CMSIS\Documentation\General\html\search\. 
  
:END
  ECHO.
REM done
