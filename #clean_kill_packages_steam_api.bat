@echo #clean.bat
@echo Removes ...
@echo 1. steam_api64.dll
@echo 2. steam_api.dll
@echo 3. steam_api.lib
@echo 4. KEEPS packages.config
@echo 5. packages folder
@echo 6. xtra folder

@echo.
@echo ===================== READY?
pause

@echo.
@echo Removing steam api dlls
del steam_api64.dll /Q
del steam_api.dll /Q
del steam_api.lib /Q

@echo.
@echo Removing Visual Studio 2022 packages
@rem NO --- this makes setting up Visual Studio 2022 for someone else annoying ... DON'T del packages.config /Q
rmdir packages /s /Q

pause

call #clean_xtra_total_remove


pause
