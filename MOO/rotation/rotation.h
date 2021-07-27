#ifndef FILE_ROTATIONS
#define FILE_ROTATIONS
 #include <cmath>

class rot
{
    public:

	double M[9];

	rot(){
	    M[0]=1.0;
	    M[3]=1.0;
	    M[6]=1.0;
	}

	rot( double a[3][3]){
		M[0]=a[0][0];
		M[1]=a[0][1];
		M[2]=a[0][2];
		M[3]=a[1][0];
		M[4]=a[1][1];
		M[5]=a[1][2];
		M[6]=a[2][0];
		M[7]=a[2][1];
		M[8]=a[2][2];
	}

	rot(double phi, double theta, double psi){
	    double cosphi = cos(phi);
	    double sinphi = sin(phi);
	    double costheta = cos(theta);
	    double sintheta = sin(theta);
	    double cospsi = cos(psi);
	    double sinpsi = sin(psi);
	    
	    M[0] = cosphi*cospsi- costheta*sinphi*sinpsi;
	    M[1] = cospsi*sinphi+costheta*cosphi*sinpsi;
	    M[2] = sinpsi*sintheta;
	    M[3] = -sinpsi*cosphi-costheta*sinphi*cospsi;
	    M[4] = -sinpsi*sinphi+costheta*cosphi*cospsi;
	    M[5] = cospsi*sintheta;
	    M[6] = sintheta*sinphi;
	    M[7] = -sintheta*cosphi;
	    M[8] = costheta;
	}

	~rot(){
	}

	rot inv(){ //returns the inverse rotation 
	    double a[3][3]={{M[0], M[3], M[6]}, {M[1], M[4], M[7]} , M[2], M[5], M[8]}; 
	    rot res(a);
	    return res;
	}

	
};

#endif // FILE_ROTATIONS
