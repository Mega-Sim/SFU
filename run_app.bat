@echo off
call %~dp0.venv\Scripts\activate
streamlit run %~dp0app.py
