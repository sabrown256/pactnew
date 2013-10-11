#
# Makefile for the PACT Distribution
#

SYS_Base   = d:\users\sabrown\pact
SYS_MngDir = ${SYS_Base}\manager
DevDir     = ${SYS_Base}\dev

pact :
	@echo Building Pact ...
	make score
	make pml
	make pdb
	make ppc
	make pgs
	make panacea
	make scheme
	make sx
	make ultra
	make install
	@echo Installation Complete

score :
	@echo Building SCORE ...
	cd ..\score
	..\manager\bmake
	make install
	cd ..\manager
	@echo SCORE installed
pml :
	@echo Building Portable Mathematics Library ...
	cd ..\pml
	..\manager\bmake
	make install
	cd ..\manager
	@echo PML installed
ppc :
	@echo Building Portable Process Control Library ...
	cd ..\ppc
	..\manager\bmake
	make install
	cd ..\manager
	@echo PPC installed
pdb :
	@echo Building Portable Data Base Library ...
	cd ..\pdb
	..\manager\bmake
	make install
	cd ..\manager
	@echo PDB installed
pgs :
	@echo Building Portable Graphics Library ...
	cd ..\pgs
	..\manager\bmake
	make install
	cd ..\manager
	@echo PGS installed
panacea :
	@echo Building Portable Code Development Library ...
	cd ..\panacea
	..\manager\bmake
	make install
	cd ..\manager
	@echo PANACEA installed
scheme :
	@echo Building Portable SCHEME Library ...
	cd ..\scheme
	..\manager\bmake
	make install
	cd ..\manager
	@echo SCHEME installed
sx :
	@echo Building Scheme with Extensions ...
	cd ..\sx
	..\manager\bmake
	make install
	cd ..\manager
	@echo SX installed
ultra :
	@echo Building ULTRA II ...
	cd ..\ultra
	..\manager\bmake
	make install
	cd ..\manager
	@echo ULTRA installed

install:
	cp scconfig.h cpyright.h ${DevDir}\include
	cp pupdate.bat ${DevDir}\bin

makes :
	@echo Making Pact Makefiles ...
	set MANAGER=${SYS_MngDir}
	for-pact ${SYS_MngDir}\manager\bmake
	@echo Pact Makefiles made

remove :
	@echo Removing Pact ...
	for-pact make remove
	@echo Pact removed

clean :
	@echo Cleaning up Pact directories ...
	for-pact make -i clean
	@echo Pact cleaned

