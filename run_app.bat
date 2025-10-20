@echo off
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
rem Ensure commands run from the repository root regardless of how the script is started
cd /d "%SCRIPT_DIR%" || goto :error

set "VENV_DIR=.venv"
set "VENV_ACTIVATE=%VENV_DIR%\Scripts\activate.bat"
set "STREAMLIT_CMD="

if not exist "%VENV_ACTIVATE%" (
    echo [INFO] No virtual environment detected. Preparing one now...
    call :bootstrap_env
    if errorlevel 1 goto :pause
)

if exist "%VENV_ACTIVATE%" (
    echo [INFO] Activating virtual environment...
    call "%VENV_ACTIVATE%" || goto :error
    call :ensure_streamlit
    if errorlevel 1 goto :pause
    set "STREAMLIT_CMD=streamlit"
    goto :launch
)

call :locate_streamlit
if defined STREAMLIT_CMD goto :launch

echo [ERROR] Streamlit executable not found.
echo        Please ensure Python is installed and reachable from your PATH.
echo        You can install dependencies manually by running:
echo            python -m pip install -r requirements.txt

goto :pause

:launch
echo [INFO] Launching Streamlit app...
call %STREAMLIT_CMD% run "app.py"
if errorlevel 1 goto :error
goto :eof

:bootstrap_env
for %%I in ("py -3" python py) do (
    call :create_env %%I
    if not errorlevel 1 exit /b 0
)
echo [ERROR] Unable to find Python. Please install Python 3.9+ and rerun.
exit /b 1

:create_env
set "INTERPRETER=%~1"
%INTERPRETER% --version >nul 2>&1 || goto :fail

echo [INFO] Using %INTERPRETER% to create a virtual environment...
%INTERPRETER% -m venv "%VENV_DIR%" >nul 2>&1
if errorlevel 1 goto :fail

if not exist "%VENV_ACTIVATE%" goto :fail

call "%VENV_ACTIVATE%" || goto :fail
python -m pip install --upgrade pip >nul 2>&1
if errorlevel 1 goto :fail
python -m pip install -r requirements.txt
if errorlevel 1 goto :fail
echo [INFO] Virtual environment ready.
exit /b 0

:fail
echo [WARN] Failed to bootstrap environment with %INTERPRETER%.
exit /b 1

:ensure_streamlit
streamlit --version >nul 2>&1
if not errorlevel 1 exit /b 0

echo [INFO] Installing Streamlit and dependencies...
python -m pip install -r requirements.txt
exit /b %errorlevel%

:locate_streamlit
if exist "%VENV_DIR%\Scripts\streamlit.exe" (
    set "STREAMLIT_CMD="%SCRIPT_DIR%%VENV_DIR%\Scripts\streamlit.exe""
    exit /b 0
)
if exist "%VENV_DIR%\Scripts\streamlit.cmd" (
    set "STREAMLIT_CMD="%SCRIPT_DIR%%VENV_DIR%\Scripts\streamlit.cmd""
    exit /b 0
)
if exist "%VENV_DIR%\Scripts\streamlit.bat" (
    set "STREAMLIT_CMD="%SCRIPT_DIR%%VENV_DIR%\Scripts\streamlit.bat""
    exit /b 0
)

where streamlit >nul 2>&1
if not errorlevel 1 (
    set "STREAMLIT_CMD=streamlit"
    exit /b 0
)

for %%I in (python py) do (
    %%I -m streamlit --version >nul 2>&1
    if not errorlevel 1 (
        set "STREAMLIT_CMD=%%I -m streamlit"
        exit /b 0
    )
)
exit /b 1

:error
echo.
echo [ERROR] Failed to launch the Streamlit app.

goto :pause

:pause
echo.
pause
