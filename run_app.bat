@echo off
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
rem Ensure commands run from the repository root regardless of how the script is started
cd /d "%SCRIPT_DIR%" || goto :error

set "STREAMLIT_CMD="

if exist ".venv\Scripts\activate.bat" (
    echo [INFO] Activating existing virtual environment...
    call ".venv\Scripts\activate.bat" || goto :error
    for %%P in (streamlit.exe streamlit.cmd streamlit.bat) do (
        if exist ".venv\Scripts\%%P" (
            set "STREAMLIT_CMD="%SCRIPT_DIR%.venv\Scripts\%%P""
            goto :launch
        )
    )
) else (
    echo [WARN] .venv not found. Attempting to use system Python.
)

where streamlit >nul 2>&1
if not errorlevel 1 (
    set "STREAMLIT_CMD=streamlit"
    goto :launch
)

for %%I in (python py) do (
    call :detect_interpreter %%I
    if defined STREAMLIT_CMD goto :launch
)

echo [ERROR] Streamlit executable not found.
echo        Please install dependencies by running:
echo            python -m pip install -r requirements.txt
echo        or create a virtual environment with:
echo            python -m venv .venv
echo            call .venv\Scripts\activate.bat
echo        then re-run this script.
goto :pause

:launch
echo [INFO] Launching Streamlit app...
call %STREAMLIT_CMD% run "app.py"
if errorlevel 1 goto :error
goto :eof

:detect_interpreter
set "CANDIDATE=%~1"
%CANDIDATE% -m streamlit --version >nul 2>&1
if not errorlevel 1 (
    set "STREAMLIT_CMD=%CANDIDATE% -m streamlit"
)
goto :eof

:error
echo.
echo [ERROR] Failed to launch the Streamlit app.

:pause
echo.
pause
