#include "Quadrupole.h"

Quad::Quad(const double & qx, const double & qy, const double & qz, const double & s){
    double s_2 =s*s;
    q[0]=2.0*s_2* (-2.0*qx + qy + qz) ;
    q[1]=0.0;
    q[2]=0.0;
    q[3]=2.0*s_2* (-2.0*qy + qz + qx) ;
    q[4]=0.0;
    q[5]=2.0*s_2* (-2.0*qz + qx + qy) ;
}

void Quad::set(const double & qx, const double & qy, const double & qz, const double & s){
        double s_2 =s*s;
        q[0]=2.0*s_2* (-2.0*qx + qy + qz) ;
        q[1]=0.0;
	q[2]=0.0;
	q[3]=2.0*s_2* (-2.0*qy + qz + qx) ;
	q[4]=0.0;
	q[5]=2.0*s_2* (-2.0*qz + qx + qy) ;
}

Quad::Quad (const Quad & A ){
    for (int i=0;i<6;i++)
    {
	q[i] = A.q[i]; 
    }
}

void Quad::set (const Quad & A){
    for (int i=0;i<6;i++)
    {
        q[i] = A.q[i];
    }
}
Quad::Quad (double  inp [6]){
    for (int i=0;i<6;i++)
    {
	    q[i] = inp[i];
    }
}

//Quad Quad::rotate( rot const& a){
//      Quad res;
//    for (int i=0;i<3;i++){
//	for (int j=0 ; j<3; j++){
//	    res.q[i*3+j]=0.0;
//	    for (int k=0;k<3;k++){
//		for ( int l=0;l<3;l++){
//		    res.q[j+i*3] += a.M[k+i*3] * a.M[l+j*3] * q[k*3+l]; 
//		}
//	    }
//	}
//      }
//    return res;
//    
//}
//
//
    

