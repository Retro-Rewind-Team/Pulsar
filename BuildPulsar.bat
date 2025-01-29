```bash
@echo off
SETLOCAL EnableDelayedExpansion
cls

:: Clear old build files
echo Cleaning up old build files...
if exist build\*.o (
    del /q build\*.o
    if %ErrorLevel% neq 0 (
        echo Error: Failed to delete old object files in build directory.
        exit /b 1
    )
) else (
    echo No old object files to delete.
)

:: Check for debug flag and configure
SET "debug="
:: Uncomment the next two lines to enable debug settings based on arguments
:: SET "cwDWARF="
:: if "%1" equ "-d" SET "debug=-debug -map=^"Dolphin Emulator\Maps\RMCP01.map^" -readelf=^"C:\MinGW\bin\readelf.exe^"" && SET "cwDWARF=-g"

:: Define paths
SET "ENGINE=.\KamekInclude"
SET "GAMESOURCE=.\GameSource"
SET "PULSAR=.\PulsarEngine"
SET "CC=C:\CodeWarriors\Command_Line_Tools\mwcceppc.exe"
SET "RIIVO=C:\Users\patchzy\AppData\Roaming\Dolphin Emulator\Load\Riivolution\RetroRewind6\Custom\RetroRewind6"
SET "CFLAGS=-I- -i %ENGINE% -i %GAMESOURCE% -i %PULSAR% -opt all -inline auto -enum int -proc gekko -fp hard -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 %cwDWARF%"

:: Ensure the CodeWarrior path exists
if not exist "%CC%" (
    echo Error: CodeWarrior path not found. Please set the correct path for CC.
    exit /b 1
)

:: Ensure the necessary directories exist
if not exist "build" (
    mkdir build
    if %ErrorLevel% neq 0 (
        echo Error: Failed to create build directory.
        exit /b 1
    )
)

:: Collect .cpp files
SET CPPFILES=
for /R %PULSAR% %%f in (*.cpp) do (
    SET "CPPFILES=!CPPFILES! %%f"
)

:: Compile kamek.cpp
echo Compiling kamek.cpp...
%CC% %CFLAGS% -c -o "build/kamek.o" "%ENGINE%\kamek.cpp"
if %ErrorLevel% neq 0 (
    echo Error: Failed to compile kamek.cpp.
    exit /b 1
)

:: Compile each .cpp file and add to OBJECTS list
SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    echo Compiling %%H...
    %CC% %CFLAGS% %DEFINE% -c -o "build/%%~nH.o" "%%H"
    if %ErrorLevel% neq 0 (
        echo Error: Failed to compile %%H.
        exit /b 1
    )
    SET "OBJECTS=!OBJECTS! build/%%~nH.o"
)

:: Linking
echo Linking... %time%
".\KamekLinker\Kamek.exe" "build/kamek.o" %OBJECTS% %debug% -dynamic -externals="%GAMESOURCE%/symbols.txt" -versions="%GAMESOURCE%/versions.txt" -output-combined=build\Code.pul
if %ErrorLevel% neq 0 (
    echo Error: Linking failed.
    exit /b 1
)

:: Copy output to Riivolution folder
if defined RIIVO (
    echo Copying binaries to %RIIVO%\Binaries...
    xcopy /Y build\*.pul "%RIIVO%\Binaries" /i /q
    if %ErrorLevel% neq 0 (
        echo Error: Failed to copy binaries to %RIIVO%\Binaries.
        exit /b 1
    ) else (
        echo Binaries copied successfully.
    )
)

:: Clear .o files after build
echo Cleaning up .o files after build...
del /q build\*.o
if %ErrorLevel% neq 0 (
    echo Warning: Failed to delete .o files after build.
) else (
    echo .o files deleted successfully.
)

echo Build completed successfully.
:end
ENDLOCAL