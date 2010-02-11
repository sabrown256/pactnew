@echo off
\mks\bin\date +%%m.%%d.%%y | sed 's/^0//' | sed 's/\.0/\./'
echo on