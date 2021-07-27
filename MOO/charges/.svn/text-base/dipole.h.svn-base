#ifndef FILE_DIP
#define FILE_DIP
#include <iostream>
#include <fstream>
#include "../rotation/rotation.h"
#include "../coord/coord.h"
#include "Quadrupole.h"

class Dip
{
   public:
	
	coord p ;	

	Dip(){
	    p.x=0.0; p.y=0.0; p.z=0.0;
	}
	
	Dip(double a[3]){
	 p.x=a[0]; p.y=a[1]; p.z=a[2];	
	}
	
	Dip( const Dip &a ){
		p=a.p;
	}
	Dip(double x, double y , double z){
		p.x=x;
		p.y=y;
		p.z=z;	
	}
	void set(double x, double y, double z){
		p.set( x,y,z);
	}
	void rotate(rot const & R){
		p.rotate(R);
	}
	
	double V(coord  &a){
		return (a*p) / (a.mod_2()*a.mod());		
	}
	
	coord dV ( coord  &a){ //return the gradient of the potetntial (- the elec Field)
		double r = a.mod();
		double r2= a.mod_2();
		double r3= r2 * r;
		double A = -3.0*(a*p)/(r2*r2);	
		return coord ( p.x/r3 + A * a.x / r,
			 p.y/r3 + A * a.y / r,
			 p.z/r3 + A * a.z /r  );
	}	
		
	double n_dipdip( Dip  &a, coord  &r)
	{
		coord tmp1= a.dV(r);	
		return  ( tmp1 * p );	
	}	
	
	double n_dipquad (Quad  &a, coord  &r){
		coord tmp1 = a.dV(r);
		return  (tmp1 * p );
	}
	
	void cp(const Dip &a){
		p=a.p;
	}
};
#endif // FILE_DIP 
