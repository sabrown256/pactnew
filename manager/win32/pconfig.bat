@echo off
if not "%2"=="" goto work
echo.
echo Usage: pconfig 'system' 'site'
echo.
echo    'system'    - the system under which PACT is to be built
echo                - current systems are:
echo                -    dos   for plain DOS systems
echo                -    dosx  for extended DOS systems
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

if not exist ..\dev mkdir ..\dev
if not exist ..\dev\bin mkdir ..\dev\bin
if not exist ..\dev\lib mkdir ..\dev\lib
if not exist ..\dev\include mkdir ..\dev\include
if not exist ..\dev\scheme mkdir ..\dev\scheme

if not exist make-mac.%1 goto error1
echo Building make-mac
rm -f make-mac
cp make-mac.%1 make-mac

if not exist make-def.%2 goto error2
echo Building make-def
rm -f make-def
cp make-def.%2 make-def

if not exist scconfig.%2 goto error5
echo Building scconfig.h
rm -f ..\scconfig.h
cp scconfig.%2 scconfig.h

if not exist trconfig.dst goto error6
echo Building trconfig.h
cp trconfig.dst ..\sx\spokes\trconfig.h

if not exist lib.%2 goto error3
echo Building lib
rm -f lib.bat
cp lib.%2 lib.bat

if not exist ld.%2 goto error4
echo Building ld
rm -f ld.bat
cp ld.%2 ld.bat

nmake makes
rem make makes
nmake install
rem make install
goto end

:error1
echo There is no make-mac.%1 for operating system %1
goto end

:error2
echo There is no make-def.%2 for site %2
goto end

:error3
echo There is no lib.%2 for site %2
goto end

:error4
echo There is no ld.%2 for site %2
goto end

:error5
echo There is no scconfig.%2 for site %2
goto end

:error6
echo There is no trconfig.dst
goto end

:end
echo on