#ifndef FILE_QUAD
#define FILE_QUAD

#include <iostream>
#include <fstream>
#include "../rotation/rotation.h"
#include "../coord/coord.h"


class Quad
{
 public:

    long double q[6];
    
    Quad(){
	q[0]=0.0;
	q[1]=0.0;
	q[3]=0.0;
	q[4]=0.0;
	q[5]=0.0;
    }

    Quad( double a, double b, double c, double d, double e, double f){
	q[0]=a;
	q[1]=b;
	q[2]=c;	
	q[3]=d;
	q[4]=e;
	q[5]=f;
    }

    Quad(const double &, const double &, const double &, const double &);
    Quad(const Quad &);
    Quad(double [6]); // store them in this order: Qxx, Qxy, Qxz, Qyy, Qyz, Qzz
    ~Quad(){
	}

    void set(const double &, const double &, const double &, const double &);
    void set ( const Quad &);
    void set (double a, double b, double c, double d, double e, double f){
	q[0]=a;
        q[1]=b;
        q[2]=c;
        q[3]=d;
        q[4]=e;
        q[5]=f;
    }

	
    void rotate(rot const& R){
	 double a= R.M[0]*R.M[0]*q[0] + R.M[0]*R.M[1]*q[1] + R.M[0]*R.M[2]*q[2] + 
	           R.M[1]*R.M[0]*q[1] + R.M[1]*R.M[1]*q[3] + R.M[1]*R.M[2]*q[4] +  
	           R.M[2]*R.M[0]*q[2] + R.M[2]*R.M[1]*q[4] + R.M[2]*R.M[2]*q[5];

	 double b= R.M[0]*R.M[3]*q[0] + R.M[0]*R.M[4]*q[1] + R.M[0]*R.M[5]*q[2] + 
	           R.M[1]*R.M[3]*q[1] + R.M[1]*R.M[4]*q[3] + R.M[1]*R.M[5]*q[4] +  
	           R.M[2]*R.M[3]*q[2] + R.M[2]*R.M[4]*q[4] + R.M[2]*R.M[5]*q[5];

	 double c= R.M[0]*R.M[6]*q[0] + R.M[0]*R.M[7]*q[1] + R.M[0]*R.M[8]*q[2] + 
	           R.M[1]*R.M[6]*q[1] + R.M[1]*R.M[7]*q[3] + R.M[1]*R.M[8]*q[4] +  
	           R.M[2]*R.M[6]*q[2] + R.M[2]*R.M[7]*q[4] + R.M[2]*R.M[8]*q[5];

	 double d= R.M[3]*R.M[3]*q[0] + R.M[3]*R.M[4]*q[1] + R.M[3]*R.M[5]*q[2] + 
	           R.M[4]*R.M[3]*q[1] + R.M[4]*R.M[4]*q[3] + R.M[4]*R.M[5]*q[4] +  
	           R.M[5]*R.M[3]*q[2] + R.M[5]*R.M[4]*q[4] + R.M[5]*R.M[5]*q[5];

	 double e= R.M[3]*R.M[6]*q[0] + R.M[3]*R.M[7]*q[1] + R.M[3]*R.M[8]*q[2] + 
	           R.M[4]*R.M[6]*q[1] + R.M[4]*R.M[7]*q[3] + R.M[4]*R.M[8]*q[4] +  
	           R.M[5]*R.M[6]*q[2] + R.M[5]*R.M[7]*q[4] + R.M[5]*R.M[8]*q[5];

	 double f= R.M[6]*R.M[6]*q[0] + R.M[6]*R.M[7]*q[1] + R.M[6]*R.M[8]*q[2] +
                   R.M[7]*R.M[6]*q[1] + R.M[7]*R.M[7]*q[3] + R.M[7]*R.M[8]*q[4] +
                   R.M[8]*R.M[6]*q[2] + R.M[8]*R.M[7]*q[4] + R.M[8]*R.M[8]*q[5];

	q[0]=a;
	q[1]=b;
	q[2]=c;
	q[3]=d;
	q[4]=e;
	q[5]=f;
    }

    double V( coord  &a){ //returns the voltage in mks
  	  double l=a.mod();
	  double l_2=a.mod_2();
	  double l_5=l_2*l_2*l;
    	  return (q[0] * a.x*a.x +
		  q[1] * 2.0 *a.x*a.y +
		  q[2] * 2.0 *a.x*a.z +
		  q[3] * a.y*a.y +
		  q[4] * 2.0 *a.y*a.z +
		  q[5] * a.z*a.z  ) /(l_5);
    }

    coord dV( coord  &a){ //returns the first derivative of the field due to a quadrupole
	double l=a.mod();
        double l_2=a.mod_2();
	double l_5=l_2*l_2*l;
        double l_7=l_5*l_2; 
	double A = -5.0 * ( q[0] * a.x*a.x + q[1] * 2.0 *a.x*a.y + q[2] * 2.0 *a.x*a.z + q[3] * a.y*a.y + q[4] * 2.0 *a.y*a.z + q[5] * a.z*a.z  ) / (l_7);
 
	return coord ( 2.0*(q[0]*a.x + q[1]*a.y+ q[2]*a.z)/l_5 + A * a.x  ,
		       2.0*(q[1]*a.x + q[3]*a.y+ q[4]*a.z)/l_5 + A * a.y  ,
                       2.0*(q[2]*a.x + q[4]*a.y+ q[5]*a.z)/l_5 + A * a.z );	
    }

    void  d2V( long double res[6] , coord &a) {
	double l=a.mod();
        double l_2=a.mod_2();
	double l_5=l_2*l_2*l;
        double l_7=l_5*l_2;

	double A_dV=-5.0 * ( q[0] * a.x*a.x + q[1] * 2.0 *a.x*a.y + q[2] * 2.0 *a.x*a.z + q[3] * a.y*a.y + q[4] * 2.0 *a.y*a.z + q[5] * a.z*a.z  ) / (l_7);	// this is equivalent to A from the previous function

	res[0]= 2.0*q[0]/l_5 + (q[0]*a.x + q[1]*a.y+ q[2]*a.z) * (-10.0/l_7) * (2.0*a.x) 
										+ a.x*a.x *(-7.0)*A_dV/l_2 +A_dV;//dxdx
	res[1]= 2.0*q[1]/l_5 + (q[0]*a.x + q[1]*a.y+ q[2]*a.z) * (-10.0/l_7) * (a.x) 
			 + (q[1]*a.x + q[3]*a.y +q[4]*a.z) * (-5.0/l_7) * (a.x) + a.x*a.y *(-7.0)*A_dV/l_2;	//dxdy
	res[2]= 2.0*q[2]/l_5 + (q[0]*a.x + q[1]*a.y+ q[2]*a.z) * (-10.0/l_7) * (a.x) 
			 + (q[2]*a.x + q[4]*a.y+ q[5]*a.z) * (-5.0/l_7) * (a.x) + a.x*a.z *(-7.0)*A_dV/l_2; 	//dxdz

	res[3]= 2.0*q[3]/l_5 + (q[1]*a.x + q[3]*a.y+ q[4]*a.z) * (-10.0/l_7) * (2.0*a.y) 
			 							+ a.y*a.y *(-7.0)*A_dV/l_2 +A_dV;//dydy
	res[4]= 2.0*q[4]/l_5 + (q[1]*a.x + q[3]*a.y+ q[4]*a.z) * (-10.0/l_7) * (a.y) 
			 + (q[2]*a.x + q[4]*a.y+ q[5]*a.z) * (-5.0/l_7) * (a.y) + a.y*a.z *(-7.0)*A_dV/l_2;       //dydz
	res[5]= 2.0*q[5]/l_5 + (q[2]*a.x + q[4]*a.y+ q[5]*a.z) * (-10.0/l_7) * (2.0*a.z) 
										+a.z*a.z *(-7.0)*A_dV/l_2 +A_dV;//dzdz
   }	

   	double n_quadquad( Quad &A, coord &r) {
	long double d_sq[6];
	A.d2V(d_sq, r);
	return (q[0]*d_sq[0]+q[1]*d_sq[1]*2.0 + q[2] *d_sq[2]* 2.0 + q[3] * d_sq[3] + q[4] *d_sq[4]*2.0 + d_sq[5] * q[5])/3.0;
	}

    void cp(const Quad &A){
	q[0]=A.q[0];
	q[1]=A.q[1];
	q[2]=A.q[2];
	q[3]=A.q[3];
	q[4]=A.q[4];
	q[5]=A.q[5];
	}
};

#endif //FILE_QUAD
