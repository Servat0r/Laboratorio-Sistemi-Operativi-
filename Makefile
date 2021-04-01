#Proviamo a vedere se riusciamo a compilare gli eseguibili delle lezioni di 
#laboratorio con make

CC = gcc

CFLAGS = -Wall -pedantic -g

#Cartelle delle esercitazioni
fld1 = ./Lab01
fld2 = ./Lab02
fld3 = ./Lab03
fld4 = ./Lab04
fld5 = ./Lab05

.PHONY : clearall
.PHONY : clear1
.PHONY : clear2
.PHONY : clear3
.PHONY : clear4
.PHONY : clear5

#Tutte le esercitazioni

buildall : build1 build2 build3 build4 build5

clearall : clear1 clear2 clear3 clear4 clear5

# Esercitazione 1

build1 : $(fld1)/*
	cd $(fld1); make

clear1 :
	cd $(fld1); make clear

# Esercitazione 2

build2 : $(fld2)/*
	cd $(fld2); make

clear2 :
	cd $(fld2); make clear

# Esercitazione 3

build3 : $(fld3)/*
	cd $(fld3); make

clear3 :
	cd $(fld3); make clear

#Esercitazione 4

build4 : $(fld4)/*
	cd $(fld4); make

clear4 : 
	cd $(fld4); make clear

#Esercitazione 5

build5 : $(fld5)/*
	cd $(fld5); make

clear5 :
	cd $(fld5); make clear

