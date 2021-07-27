#ifndef FILE_MON
#define FILE_MON

#include <iostream>
#include <fstream>
#include "../rotation/rotation.h"
#include "../coord/coord.h"
#include "dipole.h"
#include "Quadrupole.h"

class Mon
{
    public:
	double ch;
	
	Mon(){
		ch=0.0;
	}
	
	Mon(double const & a) {
		ch=a;
	}	
	
	double n_monmon( Mon  & b, coord & r) { //all energies returned in au
		return (ch*b.ch/r.mod());
	}
	
	double n_mondip( Dip  & b, coord & r) {
		return ch * b.V(r);
	}
	
	double n_monquad( Quad & b, coord & r) {
		return ch * b.V(r);
	}
	

};


#endif // FILE_MON 
