#
# Makefile for the PACT Distribution
#

PSY_Base   = d:\users\sabrown\pact
PSY_MngDir = ${PSY_Base}\manager
DevDir     = ${PSY_Base}\dev

pact :
	@echo Building Pact ...
	make score
	make pml
	make pdb
	make pgs
	make panacea
	make scheme
	make sx
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

install:
	cp scconfig.h cpyright.h ${DevDir}\include
	cp pupdate.bat ${DevDir}\bin

makes :
	@echo Making Pact Makefiles ...
	set MANAGER=${PSY_MngDir}
	for-pact ${PSY_MngDir}\manager\bmake
	@echo Pact Makefiles made

remove :
	@echo Removing Pact ...
	for-pact make remove
	@echo Pact removed

clean :
	@echo Cleaning up Pact directories ...
	for-pact make -i clean
	@echo Pact cleaned

