@echo off
if not "%2"=="" goto work
echo.
echo Usage: supdate 'system' 'site'
echo.
echo    'system'    - the system under which PACT is to be built
echo                - current systems are:
echo                -    dos   for plain DOS systems
echo                -    win   for Microsoft Windows systems
echo.
echo    'site'      - the site where the PACT installation is
echo                - (to keep track of paths and quirks)
echo                - sample sites are:
echo                -    bc    Borland C compiler
echo                -    ic    Intel 32 bit C compiler
echo.
goto end

:work

echo Uncompressing the distribution
uncompress pact.trz
mv pact.trz pact.tar

echo Unarchiving the distribution
tar xvf pact.tar
rem cpio -icqvI sdos.arc

echo Configuring the distribution
cd pact/pact
sconfig %1 %2

echo Compiling the distribution
make pact

:end
echo on