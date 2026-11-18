@echo #clean.bat
@echo Removes ...
@echo 1. build intermediate directories (.vs, build-obj, Release-zircon_beta-x64, ..)
@echo 2. built binaries, .pdb, etc
@echo 3. Visual Studio/CodeBlocks/makefile remnants (like .suo .depend .user .ncb .layout)
@echo 4. id1 folder in engine source folder (in case you start engine in source folder on accident)
@echo 5. zircon_history.txt (in case you start engine in source folder on accident)
@echo 6. (DOES NOT DELETE packages.config - Visual Studio 2022 file)
@echo 7. (DOES NOT DELETE packages folder - Visual Studio 2022 where SDL2 libraries installed )
@echo 8. (You can delete packages.config and packages folder manually, those are beyond scope of "clean")

@echo.
@echo == id1 and Visual Studio 2022 dirs (.vs)
@echo == the id1 is in case engine is mistakenly started in the source folder
@echo == which can easily happen in Visual Studio if you don't set the debug directory.
rmdir id1 /s /Q
rmdir .vs /s /Q
del zircon_history.txt /Q

@echo.
@echo == Linux (or WSL) or CodeBlocks Win64 dirs
rmdir build-obj /s /Q

@echo.
@echo == Visual Studio 2008 dirs
rmdir Debug_SDL2-zircon_qualker_32_steam /s /Q

@echo.
@echo == Visual Studio 2022 dirs
rmdir Debug-libre_life_steam-x64 /s /Q
rmdir Debug-objectn_steam-x64 /s /Q
rmdir Debug-qualker_steam-x64 /s /Q
rmdir Debug-zircon_beta_steam-x64 /s /Q
rmdir Release-libre_life_steam-x64 /s /Q
rmdir Release-objectn_steam-x64 /s /Q
rmdir Release-qualker_steam-x64 /s /Q
rmdir Release-zircon_beta_steam-x64 /s /Q

@rem === these do not happen because they are remarks ...
@rem === Baker these are pure Windows 32 bit (not SDL) build folders and Android folders
@rem rmdir xtra\SDK\dxsdk /s /Q
@rem rmdir xtra\SDK\SDL2-2.0.8 /s /Q
@rem rmdir xtra\SDK\SDL2-devel-2.0.8-mingw /s /Q
@rem rmdir xtra\SDK\SDL2-devel-2.0.8-VC /s /Q
@rem del xtra\SDK\SDL2-devel-2.28.5-mingw.zip /Q

@echo.
@echo == Deleting built binaries and SDL2.dll
del darkplaces-sdl /Q
del invaders-dedicated /Q
del invaders-sdl /Q

del librelife-dedicated /Q
del librelife-sdl /Q
del libre_life_steam.exe /Q
del libre_life_steam.exp /Q
del libre_life_steam.lib /Q
del libre_life_steam.pdb /Q

del objectn-dedicated /Q
del objectn-sdl /Q
del objectn_steam.exe /Q
del objectn_steam.exp /Q
del objectn_steam.lib /Q
del objectn_steam.pdb /Q

del qualker-dedicated /Q
del qualker-sdl /Q
del qualker_steam.exe /Q
del qualker_steam.exp /Q
del qualker_steam.lib /Q
del qualker_steam.pdb /Q

del SDL2.dll /Q
del zircon_beta_steam.exe /Q
del zircon_beta_steam.exp /Q
del zircon_beta_steam.lib /Q
del zircon_beta_steam.pdb /Q
del zircon_beta_linux_dedicated_server /Q
del zircon_beta_linux_sdl /Q

@echo.
@echo == Removing Visual Studio 20008 unnecessary files
del /A:H #zircon_32_vs_2008.suo /Q
del /A:H #zircon_32_vs_2008_steam.suo /Q
del /A:H #zircon_32_vs_2008_steam.sln /Q
del #zircon_32_vs_2008_steam.sln /Q

@echo.
@echo == Removing Visual Studio and CodeBlocks unnecessary files
del #*.layout /Q
del #*.depend /Q
del #*.user /Q
del #*.suo /Q
del #*.ncb /Q
del #*.filters /Q
del zircon.aps /Q

pause
