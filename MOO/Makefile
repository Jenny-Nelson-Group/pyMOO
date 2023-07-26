all: dymer
cc = g++
#cc = CC
#cc = cc 
#options = -fast
#options = -O3 -funroll-all-loops -ffast-math
options = -g -fPIC 
home_dir = ~
bin=~/pyMOO/bin/
pythoninc=/usr/include/python3.6m/

see_geom.o: see_geom.cpp 
	${cc} ${options} -c see_geom.cpp
	
see_geom: see_geom.o mol_and_orb/molecules_and_orbitals.o coord/coord.o charges/Quadrupole.o charges/charges.o 
	${cc} ${options} -o ${bin}/see_geom see_geom.o molecules_and_orbitals.o coord.o Quadrupole.o charges.o

coord.o: coord/coord.cpp
	${cc} ${options} -c coord/coord.cpp

dymer.o: dymer.cpp
	${cc} ${options} -c dymer.cpp

charges.o: charges/charges.cpp
	${cc} ${options} -c charges/charges.cpp

molecules_and_orbitals.o: mol_and_orb/molecules_and_orbitals.cpp
	${cc} ${options} -c mol_and_orb/molecules_and_orbitals.cpp

Quadrupole.o: charges/Quadrupole.cpp	
	${cc} ${options} -c charges/Quadrupole.cpp

abject.o: abject.cpp
	${cc} ${options} -c abject.cpp

dymer: dymer.o charges.o molecules_and_orbitals.o coord.o Quadrupole.o
	${cc} ${options} -o ${bin}/J_dymer_MOO dymer.o charges.o molecules_and_orbitals.o coord.o Quadrupole.o
	
matrix.o: matrix.cpp
	${cc} ${options} -c matrix.cpp

matrix: matrix.o molecules_and_orbitals.o coord.o charges.o
	${cc} ${options} -o ${bin}/J_matrix_MOO  matrix.o molecules_and_orbitals.o coord.o charges.o

jarv_matrix: jarv_matrix.o molecules_and_orbitals.o coord.o charges.o
	${cc} ${options} -o ${bin}/J_jarv_matrix_MOO  jarv_matrix.o molecules_and_orbitals.o coord.o charges.o

pyMOOmodule: pyMOOmodule.o molecules_and_orbitals.o coord.o charges.o
	${cc} ${options} -c pyMOOmodule_wrap.c -I ${pythoninc}
	${cc} ${options} -c pyMOOmodule.cpp -c pyMOOmodule_wrap.c -I ${pythoninc}
	${cc} -shared -o _pyMOO.so pyMOOmodule_wrap.o pyMOOmodule.o molecules_and_orbitals.o coord.o charges.o

abject: abject.o molecules_and_orbitals.o coord.o charges.o
	${cc} ${options} -o ${bin}/abject abject.o charges.o molecules_and_orbitals.o coord.o

clean:
	rm *.o 
