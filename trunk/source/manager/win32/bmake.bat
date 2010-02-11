@echo off
if exist Makefile rm Makefile
cat %MANAGER%\make-def pre-Make.dos %MANAGER%\make-mac > Makefile
echo on
