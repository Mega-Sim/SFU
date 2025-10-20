@echo off
setlocal

rem Ensure commands run from the repository root regardless of how the script is started
cd /d "%~dp0" || goto :error

if exist ".venv\Scripts\activate.bat" (
    echo [INFO] Activating existing virtual environment...
    call ".venv\Scripts\activate.bat" || goto :error
) else (
    echo [WARN] .venv not found. Attempting to use system Python.
)

where streamlit >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Streamlit executable not found.
    echo        Please install dependencies by running:
    echo            python -m pip install -r requirements.txt
    echo        or create a virtual environment with:
    echo            python -m venv .venv
    echo            call .venv\Scripts\activate.bat
    echo        then re-run this script.
    goto :pause
)

echo [INFO] Launching Streamlit app...
streamlit run "app.py"
if errorlevel 1 goto :error
goto :eof

:error
echo.
echo [ERROR] Failed to launch the Streamlit app.

:pause
echo.
pause
