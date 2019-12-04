:: Batch file for generating CMSIS-FreeRTOS pack
:: This batch file uses:
::    7-Zip for packaging
::    Doxygen version 1.8.2 and Mscgen version 0.20 for generating html documentation.
:: The generated pack and pdsc file are placed in folder %RELEASE_PATH% (../Local_Release)
@ECHO off

SETLOCAL

:: Tool path for zipping tool 7-Zip
SET ZIPPATH=C:\Program Files\7-Zip

:: Tool path for doxygen
SET DOXYGENPATH=C:\Program Files\doxygen\bin

:: Tool path for mscgen utility
SET MSCGENPATH=C:\Program Files (x86)\Mscgen

:: These settings should be passed on to subprocesses as well
SET PATH=%ZIPPATH%;%DOXYGENPATH%;%MSCGENPATH%;%PATH%

:: Pack Path (where generated pack is stored)
SET RELEASE_PATH=..\Local_Release

:: !!!!!!!!!!!!!!!!!
:: DO NOT EDIT BELOW
:: !!!!!!!!!!!!!!!!! 

:: Remove previous build
IF EXIST %RELEASE_PATH% (
  ECHO removing %RELEASE_PATH%
  RMDIR /Q /S  %RELEASE_PATH%
)

:: Generate Documentation 
PUSHD ..\DoxyGen
CALL genDoc.bat
POPD


:: Create build output directory
MKDIR %RELEASE_PATH%

:: Copy PDSC file
COPY .\..\ARM.CMSIS-FreeRTOS.pdsc %RELEASE_PATH%\ARM.CMSIS-FreeRTOS.pdsc

:: Copy LICENSE file
XCOPY /Q /S /Y .\..\License\*.* %RELEASE_PATH%\License\*.*

:: Copy various root files
COPY .\..\readme.txt %RELEASE_PATH%\readme.txt
COPY .\..\links_to_doc_pages_for_the_demo_projects.url %RELEASE_PATH%\links_to_doc_pages_for_the_demo_projects.url

:: Copy CMSIS folder
XCOPY /Q /S /Y .\..\CMSIS\*.* %RELEASE_PATH%\CMSIS\*.*

:: Copy Config folder
XCOPY /Q /S /Y .\..\Config\*.* %RELEASE_PATH%\Config\*.*

:: Copy Demo folder
XCOPY /Q /S /Y .\..\Demo\*.* %RELEASE_PATH%\Demo\*.*

:: Copy Source folder
XCOPY /Q /S /Y .\..\Source\*.* %RELEASE_PATH%\Source\*.*

:: Remove generated doxygen files
RMDIR /S /Q ..\CMSIS\Documentation

:: Checking 
:: Silencing warnings that are irrelevant in the context (M324, M382, M363)
Win32\PackChk.exe %RELEASE_PATH%\ARM.CMSIS-FreeRTOS.pdsc -n %RELEASE_PATH%\PackName.txt -x M324 -x M382 -x M363 -x M362

:: --Check if PackChk.exe has completed successfully
IF %errorlevel% neq 0 GOTO ErrPackChk

:: Packing 
PUSHD %RELEASE_PATH%

:: -- Pipe Pack's Name into Variable
SET /P PackName=<PackName.txt
DEL /Q PackName.txt

:: Pack files
ECHO Creating pack file ...
7z.exe a %PackName% -tzip > zip.log
ECHO Packaging complete
POPD
GOTO End

:ErrPackChk
ECHO PackChk.exe has encountered an error!
EXIT /b

:End
ECHO Removing temporary files and folders
PUSHD %RELEASE_PATH%
FOR %%A IN (CMSIS Config Demo License Source) DO IF EXIST %%A (RMDIR /S /Q %%A)
DEL links_to_doc_pages_for_the_demo_projects.url
DEL readme.txt
DEL zip.log
POPD

ECHO gen_pack.bat completed successfully
