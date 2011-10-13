@echo off
cd\scifair
echo Results of test for %1 format: > results.dat
echo ÿ >> results.dat
c:\dos\superpck /d > nul
if "%1"=="fractal" goto fractal
if "%1"=="gif" goto gif
timer %1load watfall.%1 >> results.dat
timer %1load dinosaur.%1 >> results.dat
timer %1load raytrace.%1 >> results.dat
:retrn
c:\dos\superpck /e > nul
type results.dat
for %%h in (1 1 1 1 1 1 1 1 1 1) do echo ÿ
pause
cd\demo
goto bottom
:fractal
key-fake 0 13
timer codload watfall.cod >> results.dat
goto retrn
:gif
timer vpic /r watfall.gif >> results.dat
timer vpic /r dinosaur.gif >> results.dat
timer vpic /r raytrace.gif >> results.dat
mode co80
goto retrn
:bottom
