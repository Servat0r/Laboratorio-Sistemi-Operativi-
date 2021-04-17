#Makefile centralizzato per TUTTI gli eseguibili di laboratorio

#Variabili
numbers := 01 02 03 04 05 06 07

folders := $(numbers:%=./Lab%)

builds := $(numbers:%=build%)

clears := $(numbers:%=clear%)


#Tutte le clear* sono phony
.PHONY : $(clears)

#Tutte le esercitazioni

buildall : $(builds)
	@echo All files built!

clearall : $(clears)
	@echo All files cleared!

$(builds) : build% : ./Lab%
	cd $<; make

$(clears) : clear% : ./Lab%
	cd $<; make clear
