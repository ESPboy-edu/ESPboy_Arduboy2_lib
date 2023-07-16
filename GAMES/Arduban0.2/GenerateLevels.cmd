@echo off
pushd %~dp0\ParseLevels

dotnet run -- "%~dp0\Levels\Microban.txt" "%~dp0\Assets\Levels\Microban II.txt" "%~dp0\Assets\Levels\Microban III.txt" "%~dp0\Assets\Levels\Microban IV.txt" "%~dp0\Assets\Levels\Sasquatch.txt" "%~dp0\Assets\Levels\Sasquatch II.txt" "%~dp0\Assets\Levels\Sasquatch III.txt" "%~dp0\Assets\Levels\Sasquatch IV.txt" "%~dp0\Assets\Levels\Sasquatch V.txt" "%~dp0\Assets\Levels\Sasquatch VI.txt" "%~dp0\Assets\Levels\Sasquatch VII.txt" "%~dp0\Assets\Levels\Sasquatch VIII.txt" "%~dp0\Assets\Levels\Sasquatch IX.txt" "%~dp0\Assets\Levels\Sasquatch X.txt" "%~dp0\Assets\Levels\Sasquatch XI.txt" "%~dp0\Assets\Levels\YoshioMurase.txt" > "%~dp0\Arduban\levels.h"

popd