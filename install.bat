mkdir build
cd build
cmake -G "MinGW Makefiles" ..
make
#mkdir deploy
#copy .\FallPointCalculator.exe .\deploy\
#copy .\gui\MainWindow.dll .\deploy\
#cd deploy
windeployqt .
make Installer
pause

