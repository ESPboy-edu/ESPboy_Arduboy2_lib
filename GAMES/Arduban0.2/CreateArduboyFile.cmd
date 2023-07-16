@echo off

copy /Y Assets\Install\* build

if not exist package goto MKDIR
rmdir /S /Q package

:MKDIR
mkdir package

pushd build

"C:\Program Files\7-Zip\7z.exe" a -aou -tZip arduban.arduboy @filelist.txt

copy /Y arduban.arduboy ..\package\arduban.arduboy
copy /Y Arduban.ino.hex ..\package\Arduban.hex

popd