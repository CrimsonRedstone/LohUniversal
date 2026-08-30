@echo off
REM ============================================================================
REM  Loh Universal  /  Лох Universal
REM  One-click Windows build. Always writes a log you can paste back to Grok.
REM
REM  Double-click this file, or from a terminal:
REM      build.bat
REM      build.bat nopause
REM ============================================================================
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"

REM UTF-8 so Cyrillic in the log does not turn into garbage
chcp 65001 >nul 2>&1

set "ROOT=%cd%"
set "LOGDIR=%ROOT%\logs"
if not exist "%LOGDIR%" mkdir "%LOGDIR%"

for /f %%I in ('powershell -NoProfile -Command "Get-Date -Format yyyyMMdd-HHmmss" 2^>nul') do set "STAMP=%%I"
if not defined STAMP set "STAMP=%DATE:~-4%%DATE:~-10,2%%DATE:~-7,2%-%TIME:~0,2%%TIME:~3,2%%TIME:~6,2%"
set "STAMP=%STAMP: =0%"

set "LOG=%LOGDIR%\build-%STAMP%.log"
set "LATEST=%ROOT%\build.log"
set "STATUS=%ROOT%\BUILD_STATUS.txt"

call :banner
call :log "==============================================================="
call :log "  Loh Universal  /  Лох Universal"
call :log "  Crimson Redstone  ·  freeware"
call :log "  Started: %DATE% %TIME%"
call :log "  Folder : %ROOT%"
call :log "  Log    : %LOG%"
call :log "==============================================================="
call :log ""

REM ---- tools -----------------------------------------------------------------
call :log "[1/5] Checking tools..."
where cmake >nul 2>&1
if errorlevel 1 (
    call :fail "CMake is not on PATH. Install https://cmake.org/download/ and tick 'Add CMake to the system PATH'."
    goto :end
)
for /f "delims=" %%V in ('cmake --version 2^>^&1') do (
    call :log "  %%V"
    goto :after_cmake_ver
)
:after_cmake_ver
call :log "  git:"
where git >nul 2>&1
if errorlevel 1 (
    call :log "  WARNING: git not found. First configure will fail because JUCE is fetched via git."
    call :log "  Install Git from https://git-scm.com/download/win and re-run."
) else (
    for /f "delims=" %%V in ('git --version 2^>^&1') do call :log "  %%V"
)

call :log ""
call :log "  Compiler / generator probe:"
if defined VSINSTALLDIR call :log "  VSINSTALLDIR=%VSINSTALLDIR%"
where cl >nul 2>&1
if errorlevel 1 (
    call :log "  cl.exe not on PATH (normal if you did not open an x64 Native Tools prompt)."
    call :log "  CMake will try the Visual Studio generator instead."
) else (
    for /f "delims=" %%V in ('cl 2^>^&1') do (
        call :log "  %%V"
        goto :after_cl
    )
)
:after_cl

REM ---- configure -------------------------------------------------------------
call :log ""
call :log "[2/5] Configuring CMake (this downloads JUCE 9.0.1 on the first run, ~a few minutes)..."

set "GEN_ARGS="
REM Prefer VS 2022 x64 if that generator exists
cmake -G "Visual Studio 17 2022" -A x64 -B "%ROOT%\build" -S "%ROOT%" >> "%LOG%" 2>&1
if errorlevel 1 (
    call :log "  VS 2022 generator failed — retrying with CMake's default generator..."
    cmake -B "%ROOT%\build" -S "%ROOT%" -DCMAKE_BUILD_TYPE=Release >> "%LOG%" 2>&1
    if errorlevel 1 (
        call :fail "CMake configure failed. Scroll the log for the first error (often: git missing, no compiler, or network blocked fetching JUCE)."
        goto :end
    )
) else (
    call :log "  Configured with Visual Studio 17 2022 x64."
)

REM ---- build -----------------------------------------------------------------
call :log ""
call :log "[3/5] Compiling Release (go make tea — first build is long because it compiles JUCE too)..."
cmake --build "%ROOT%\build" --config Release --parallel >> "%LOG%" 2>&1
if errorlevel 1 (
    call :fail "Compile failed. Search the log for 'error C' or 'error:' — that is the line Grok needs."
    goto :end
)

REM ---- locate artefacts ------------------------------------------------------
call :log ""
call :log "[4/5] Locating plugin files..."
set "ART="
for /d %%D in ("%ROOT%\build\LohUniversal_artefacts*") do set "ART=%%~fD"
if not defined ART (
    for /d %%D in ("%ROOT%\build\*\LohUniversal_artefacts*") do set "ART=%%~fD"
)

if not defined ART (
    call :log "  Built, but could not find LohUniversal_artefacts. Dumping build\ :"
    dir /s /b "%ROOT%\build\*.vst3" "%ROOT%\build\*.exe" "%ROOT%\build\*.clap" >> "%LOG%" 2>&1
) else (
    call :log "  Artefacts: %ART%"
    if exist "%ART%\Release" (
        dir /s /b "%ART%\Release" >> "%LOG%" 2>&1
    ) else (
        dir /s /b "%ART%" >> "%LOG%" 2>&1
    )
)

call :log ""
call :log "[5/5] SUCCESS"
call :log "  Copy the .vst3 folder to:  C:\Program Files\Common Files\VST3"
call :log "  Standalone .exe is next to it under Standalone\"
call :log "  Full log also saved as: %LATEST%"
call :log "==============================================================="

(
    echo SUCCESS
    echo time=%DATE% %TIME%
    echo log=%LOG%
    echo artefacts=%ART%
) > "%STATUS%"

copy /y "%LOG%" "%LATEST%" >nul
echo.
echo  ============================================================
echo   BUILD OK   —  Лох Universal
echo   Log:      %LATEST%
echo   Status:   %STATUS%
if defined ART echo   Output:   %ART%
echo  ============================================================
echo.
goto :end

REM ---- helpers ---------------------------------------------------------------
:banner
echo.
echo   Лох Universal  —  one-click build
echo   Log will be written even if this fails.
echo.
goto :eof

:log
echo %~1
echo %~1>> "%LOG%"
goto :eof

:fail
echo.
echo  ============================================================
echo   BUILD FAILED
echo   %~1
echo   Paste THIS FILE to Grok:  %LATEST%
echo  ============================================================
echo.
call :log ""
call :log "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
call :log "FAILED: %~1"
call :log "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
(
    echo FAILED
    echo time=%DATE% %TIME%
    echo reason=%~1
    echo log=%LOG%
    echo.
    echo Paste build.log ^(this folder^) to Grok so the compile errors can be fixed.
) > "%STATUS%"
copy /y "%LOG%" "%LATEST%" >nul
exit /b 1

:end
copy /y "%LOG%" "%LATEST%" >nul 2>nul
if /I not "%~1"=="nopause" if /I not "%~1"=="--nopause" (
    echo.
    pause
)
endlocal
exit /b %ERRORLEVEL%
