#ifndef FILE_COORD
#define FILE_COORD

#include <iostream>
#include <fstream>
#include <math.h>
#include "../rotation/rotation.h"

using namespace std;

const double PI = 4 * atan(1.0);

class coord
{
public:

	double x;
	double y;
	double z;

	coord(){ // constructors
		x=0.0; y=0.0; z=0.0;
	}
	coord(double a[3]){
	    x=a[0];y=a[1];z=a[2];
	}

	coord(const char &,const double &,const double &,const double &);
	coord ( double a, double b, double c){
		x=a;
		y=b;
		z=c;
	}

	void set(const char &,const double &,const double &,const double &);
	void set(  double &a, double &b, double &c)
	{
		x=a; y=b; z=c;
	}
	
	double getx();
	double gety();
	double getz();	
	double getph(); //accessors
	double getth();
	double mod(){
	    return sqrt (x*x +y*y +z*z);
	}
	double mod_2(){
	    return (x*x+y*y+z*z);
	}

	coord cross(  const coord &b ){
	    return coord( y * b.z - z * b.y, 
		          z * b.x - x * b.z, 
			  x * b.y - y * b.x);
	}

	void print();
	void print(ofstream &);
	void Rot(const double &,const double &,const double &); // euler rotations by theta and phy about the x axis and the z axis.
	void rotlean(double  &,  double &,double &, double &, double &, double &); // same as above, only with predetermined values for the coss and sines of the euler angles
	void xrot(const double &);
	void yrot(const double &);
	void zrot(const double &);
	void xrot_l(const double &, const double &);
	void yrot_l(const double &, const double &);
	void zrot_l(const double &, const double &);
	void rot_about_axis( const double &, const double &, const double &);

	void rotate( rot const& a ){
		double x_t, y_t, z_t;
    		x_t = a.M[0] * x + a.M[1] * y + a.M[2] * z;
    		y_t = a.M[3] * x + a.M[4] * y + a.M[5] * z;
    		z_t = a.M[6] * x + a.M[7] * y + a.M[8] * z;
    		x = x_t;
    		y = y_t;
    		z = z_t;
	}

	void add( coord &);

	coord operator+( coord &a){
		return coord (x+a.x, y+a.y, z+a.z);
	}
	coord operator-( const coord & a){
		return coord (x-a.x, y-a.y, z-a.z);
	}
	coord operator-(){
		return coord (-x,-y,-z);
	}
	double operator*( const coord & a){
		return a.x*x + a.y *y + a.z *z;
	}
	coord operator*( double & a){
		return coord (a*x, a*y, a*z);
	}
	coord operator/( double & a){
		return coord (x/a, y/a, z/a);
	}
	

	void operator=(const coord & a){
		x=a.x;
		y=a.y;
		z=a.z;
	}
	bool operator==(const coord & a ){
		if (x == a.x && y == a.y && z == a.z) return true;
		else return false;
	}
};

ostream& operator<<(ostream&, coord);
#endif // FILE_COORD



