#pragma once

#include <string>

// For Windows

inline const std::string bannerScript{R"(@echo off
REM ================================================================
REM banner_grab.bat – Windows Batch Version
REM Usage:
REM    banner_grab.bat fast targets.txt
REM    banner_grab.bat full targets.txt
REM    banner_grab.bat stealth targets.txt
REM ================================================================

:: If nmap.exe is NOT in your PATH, set full path here, e.g.
:: set "NMAP=C:\Program Files (x86)\Nmap\nmap.exe"
set "NMAP=nmap.exe"

set "TYPE=%1"
set "TARGET_FILE=%2"

if "%TYPE%"=="" (
  echo Usage: banner_grab.bat [fast^|full^|stealth] targets.txt
  exit /b 1
)

if "%TARGET_FILE%"=="" (
  echo Please provide a target file.
  exit /b 1
)

if not exist "%TARGET_FILE%" (
  echo Target file %TARGET_FILE% not found.
  exit /b 1
)

if not exist scan_results mkdir scan_results

set "OUTPUT=banner_results_%TYPE%_.csv"
echo "IP","Hostname","OS","Role","ScanType","Port","State","Service","Banner" > "%OUTPUT%"

for /f "usebackq delims=" %%T in ("%TARGET_FILE%") do (
  echo [*] Scanning %%T with %TYPE%...
  if /i "%TYPE%"=="fast" (
    "%NMAP%" -T4 -F -sV -oG "scan_results\%%T_fast.gnmap" %%T
    call :ParseGNMAP "scan_results\%%T_fast.gnmap" "%%T" "%TYPE%"
  ) else if /i "%TYPE%"=="full" (
    "%NMAP%" -T4 -p- -sV -oG "scan_results\%%T_full.gnmap" %%T
    call :ParseGNMAP "scan_results\%%T_full.gnmap" "%%T" "%TYPE%"
  ) else if /i "%TYPE%"=="stealth" (
    "%NMAP%" -sS -sV -T2 -oG "scan_results\%%T_stealth.gnmap" %%T
    call :ParseGNMAP "scan_results\%%T_stealth.gnmap" "%%T" "%TYPE%"
  ) else (
    echo Unknown scan type %TYPE%
  )
)

echo.
echo [*] Done. Results appended to %OUTPUT%.
pause
exit /b 0

:ParseGNMAP
REM %~1 = GNMAP file, %~2 = IP, %~3 = ScanType
setlocal enabledelayedexpansion
set "GNFILE=%~1"
set "IP=%~2"
set "SCAN=%~3"

if not exist "%GNFILE%" (
  endlocal
  goto :eof
)

REM parse lines that contain "Ports:"
for /f "usebackq tokens=* delims=" %%L in (findstr /C:"Ports:" "%GNFILE%") do (
    REM example matched line:
    REM Host: 192.168.56.105 ()  Ports: 80/open/tcp//http///
    REM extract the Ports: field content
    for /f "tokens=2* delims=:" %%A in ("%%L") do (
        set "PORTSTRING=%%B"
        REM split comma/space-separated port entries by iterating tokens separated by space
        for %%P in (!PORTSTRING!) do (
            REM each %%P looks like 80/open/tcp//http///
            for /f "tokens=1,2 delims=/" %%x in ("%%P") do (
                set "Pnum=%%x"
                set "Pstate=%%y"
                if "!Pstate!"=="" set "Pstate=unknown"
                echo "!IP!","!IP!","Unknown","Unknown","!SCAN!","!Pnum!","!Pstate!","tcp","tcp" >> "%OUTPUT%"
            )
        )
    )
)

endlocal
goto :eof)"};