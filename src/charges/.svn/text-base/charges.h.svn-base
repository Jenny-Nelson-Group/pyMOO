#ifndef FILE_CHARGES
#define FILE_CHARGES

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "../coord/coord.h"
#include "../rotation/rotation.h"
#include "../global.h"
#include "monopole.h"
#include "dipole.h"
#include "Quadrupole.h"

class multipoles
{
    public: 
 	double mpls [NAT_MAX]; // bear charges for the molecules
    	double dpls [NAT_MAX][3]; // next term in the DMA
    	double qdrpls [NAT_MAX][3][3]; // last term we will include

    void cp_crg( const multipoles &A, int &N)
    {
	int i=0;
	while (i<N)
	{
	    dpls[i][0] = ( A.dpls[i][0] );
	    dpls[i][1] =  A.dpls[i][1];
	    dpls[i][2] = A.dpls[i][2];
	    qdrpls[i][0][0] = A.qdrpls[i][0][0] ;
	    qdrpls[i][0][1] = A.qdrpls[i][0][1] ;
	    qdrpls[i][0][2] = A.qdrpls[i][0][2] ;
	    qdrpls[i][1][0] = A.qdrpls[i][1][0] ;
	    qdrpls[i][1][1] = A.qdrpls[i][1][1] ;
	    qdrpls[i][1][2] = A.qdrpls[i][1][2] ;
	    qdrpls[i][2][0] = A.qdrpls[i][2][0] ;
	    qdrpls[i][2][1] = A.qdrpls[i][2][1] ;
	    qdrpls[i][2][2] = A.qdrpls[i][2][2] ;

	    i++;
	}
    }
    
    void rotate(const rot &a, int &N)
    {
	int i=0;
	static double tmpd[3];
	static double tmpqdrpls[3][3];
	while ( i < N )
	{
	    tmpd[0] = dpls[i][0]*a.M[0] + dpls[i][1]*a.M[1] + dpls[i][2]*a.M[2];
	    tmpd[1] = dpls[i][0]*a.M[3] + dpls[i][1]*a.M[4] + dpls[i][2]*a.M[5];
	    tmpd[2] = dpls[i][0]*a.M[6] + dpls[i][1]*a.M[7] + dpls[i][2]*a.M[8];
	    
	    dpls[i][0] = tmpd[0];
	    dpls[i][1] = tmpd[1];
	    dpls[i][2] = tmpd[2];
	    
	    tmpqdrpls[0][0] = a.M[0]*a.M[0]*qdrpls[i][0][0] + a.M[0]*a.M[1]*qdrpls[i][0][1] + a.M[0]*a.M[2]*qdrpls[i][0][2] +
                              a.M[1]*a.M[0]*qdrpls[i][1][0] + a.M[1]*a.M[1]*qdrpls[i][1][1] + a.M[1]*a.M[2]*qdrpls[i][1][2] +
                              a.M[2]*a.M[0]*qdrpls[i][2][0] + a.M[2]*a.M[1]*qdrpls[i][2][1] + a.M[2]*a.M[2]*qdrpls[i][2][2];

            tmpqdrpls[0][1] = a.M[0]*a.M[3]*qdrpls[i][0][0] + a.M[0]*a.M[4]*qdrpls[i][0][1] + a.M[0]*a.M[5]*qdrpls[i][0][2] +
                              a.M[1]*a.M[3]*qdrpls[i][1][0] + a.M[1]*a.M[4]*qdrpls[i][1][1] + a.M[1]*a.M[5]*qdrpls[i][1][2] +
                              a.M[2]*a.M[3]*qdrpls[i][2][0] + a.M[2]*a.M[4]*qdrpls[i][2][1] + a.M[2]*a.M[5]*qdrpls[i][2][2];

            tmpqdrpls[0][2] = a.M[0]*a.M[6]*qdrpls[i][0][0] + a.M[0]*a.M[7]*qdrpls[i][0][1] + a.M[0]*a.M[8]*qdrpls[i][0][2] +
                              a.M[1]*a.M[6]*qdrpls[i][1][0] + a.M[1]*a.M[7]*qdrpls[i][1][1] + a.M[1]*a.M[8]*qdrpls[i][1][2] +
                              a.M[2]*a.M[6]*qdrpls[i][2][0] + a.M[2]*a.M[7]*qdrpls[i][2][1] + a.M[2]*a.M[8]*qdrpls[i][2][2];

            tmpqdrpls[1][1] = a.M[3]*a.M[3]*qdrpls[i][0][0] + a.M[3]*a.M[4]*qdrpls[i][0][1] + a.M[3]*a.M[5]*qdrpls[i][0][2] +
                              a.M[4]*a.M[3]*qdrpls[i][1][0] + a.M[4]*a.M[4]*qdrpls[i][1][1] + a.M[4]*a.M[5]*qdrpls[i][1][2] +
                              a.M[5]*a.M[3]*qdrpls[i][2][0] + a.M[5]*a.M[4]*qdrpls[i][2][1] + a.M[5]*a.M[5]*qdrpls[i][2][2];

            tmpqdrpls[1][2] = a.M[3]*a.M[6]*qdrpls[i][0][0] + a.M[3]*a.M[7]*qdrpls[i][0][1] + a.M[3]*a.M[8]*qdrpls[i][0][2] +
                              a.M[4]*a.M[6]*qdrpls[i][1][0] + a.M[4]*a.M[7]*qdrpls[i][1][1] + a.M[4]*a.M[8]*qdrpls[i][1][2] +
                              a.M[5]*a.M[6]*qdrpls[i][2][0] + a.M[5]*a.M[7]*qdrpls[i][2][1] + a.M[5]*a.M[8]*qdrpls[i][2][2];

            tmpqdrpls[2][2] = a.M[6]*a.M[6]*qdrpls[i][0][0] + a.M[6]*a.M[7]*qdrpls[i][0][1] + a.M[6]*a.M[8]*qdrpls[i][0][2] +
                              a.M[7]*a.M[6]*qdrpls[i][1][0] + a.M[7]*a.M[7]*qdrpls[i][1][1] + a.M[7]*a.M[8]*qdrpls[i][1][2] +
	                      a.M[8]*a.M[6]*qdrpls[i][2][0] + a.M[8]*a.M[7]*qdrpls[i][2][1] + a.M[8]*a.M[8]*qdrpls[i][2][2];
	    qdrpls[i][0][0] = tmpqdrpls[0][0];
	    qdrpls[i][0][1] = tmpqdrpls[0][1];
	    qdrpls[i][1][0] = tmpqdrpls[0][1];
	    qdrpls[i][0][2] = tmpqdrpls[0][2];
	    qdrpls[i][2][0] = tmpqdrpls[0][2];
	    
	    qdrpls[i][1][1] = tmpqdrpls[1][1];
	    qdrpls[i][1][2] = tmpqdrpls[1][2];
	    qdrpls[i][2][1] = tmpqdrpls[1][2];
	   
	    qdrpls[i][2][2] = tmpqdrpls[2][2];
	    i++;
	}
    }
    
    int read_crg(char * , char * , int & );
    int read_crg_eps(char *, char *, int &);
};

#endif //FILE_CHARGES

