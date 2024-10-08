#include "molecules_and_orbitals.h"

const bool use_hash=false;

const int HASH_S = 100000; // resolution in hash table to STO overlap of AOs
static double _1S1S [HASH_S+1];
static double _2S2S [HASH_S+1];
static double _2S2Sig [HASH_S+1];
static double _2Sig2Sig [HASH_S+1];
static double _2Pi2Pi [HASH_S+1];
static double _1S2S_hc [HASH_S+1];
static double _1S2Sig_hc [HASH_S +1];

const static double PMAX=1000.0; // this defines the maximum p - refer to Mulliken paper for more detail
const static double PMIN=0.0;

//ZINDO/S parameters for first three rows
static double Beta[18] = {-12.000000, 0.000000, 0.000000, 0.000000, 0.000000, -17.000000, -26.000000, -34.000000, -44.000000, 0.000000, 0.000000, -6.000000, 0.000000, 0.000000, 0.000000, -15.000000, -19.000000, 0.00000 } ;
// copies these values from HyperChem's data files!!!
static double Mu[18] = { 1.200000, 0.000000, 0.000000, 0.000000, 0.000000, 1.625000, 1.950000, 2.275000, 2.600000, 0.000000, 0.000000, 1.103000, 0.000000, 0.000000, 0.000000, 1.816670, 2.033330, 0.000000 }; 
// slater exponents (notice I assyme the are the same for s/p : this is only the case up to row 3, including transition metals would require quite a re-write. 

rot get_eulerrot(coord & , coord & , coord & , coord  & );

inline int rot_orb( double  orb[NBASIS_MAX], char *bs, int const & nb, rot const & R){
	int i=0;
	double x_t,y_t,z_t;
	while (i<nb)
	{
		switch( bs[i] ){
			case 's':
			i++;
			break;
			case 'S':
			i++;
			break;
			case 'x':
			x_t = orb[i]*R.M[0] + orb[i+1] * R.M[1] + orb[i+2] * R.M[2];
			y_t = orb[i]*R.M[3] + orb[i+1] * R.M[4] + orb[i+2] * R.M[5];
			z_t = orb[i]*R.M[6] + orb[i+1] * R.M[7] + orb[i+2] * R.M[8] ;
			orb[i]=x_t;
			orb[i+1]=y_t;
			orb[i+2]=z_t;
			i=i+3;
			break;

			case 'y':
			cout << " Error in  rot about axis" <<endl;
                        return 1;
			case 'z':
			cout << " Error in  rot about axis" <<endl;
                        return 1;
			default :
			cout << " Error in  rot about axis" <<endl;
			return 1;
		}
	}
	return 0;
}

inline void rotorbs(double orbs [NBASIS_MAX][NBASIS_MAX], char *bs, int const & nb, rot const & R){
	for (int i =0; i< nb;i++){
		rot_orb( orbs[i], bs, nb , R);
	}
}

int mol_and_orb::read_orb( int const& debug, char * nameorbs)
{
 ifstream in(nameorbs); // read in from the average file
 if (!in){
     cout << "Error file  nameorbs  containg the orbitals non existant " <<endl;
     return 1;
 }

 string line;
 string number;

 int i,j;

 vector <string> file;

 int n_lin_in_wt = (mol_and_orb::NBasis-1)/5+2;
 int k=0;

 //skip the fortran specification line
 getline (in, line);
 while (in){
	getline (in, line);
        if (debug ==1) cout << "output thw the progressive destruction of strings! " <<k%n_lin_in_wt<<  endl;
        if(k  % (n_lin_in_wt) != 0)
        {
                while (line.size() > 1 )
                {
                        number.assign(line,0,15);
                        file.push_back(number);
                        line.erase(0,15);
                        if ( debug == 1) {cout << "number :" << number <<'\t'
                                 << file.size() << endl; cout << line <<endl;}
                }
        }
        k++;
   }

   if (debug == 1)
   {
       cout << " having read orb2.inp the number of lines we've got is " << file.size() << endl;
   }
   for(i=0,j=0,k=0;i<file.size();i++,k++)
   {

        file[i].replace(file[i].size()-4,1,1,'e');
        sscanf(file[i].c_str(), "%lf" , &psi[j][k]);
        if (debug==1) {cout << " Just written out psi " << j << " " << k << " " << psi[j][k] << endl;}
        if (i%NBasis==NBasis-1){k=-1;j++;}
    }
    k=0;
    file.clear();
    return 0;
}

// Here I insert the stuff to calculate overlap of STOs


inline double Abs (double  a)
{
    if (a< 0.0) return -a;
    return a;
}
inline double A6(double *p_p, double & EXP){
    	return EXP *(p_p[0] + 6.0*p_p[1] + 30.0*p_p[2] + 120.0 *p_p[3] + 360.0*p_p[4] + 720.0*p_p[5] + 720*p_p[6]);
}
	
inline double A5(double *p_p, double & EXP){
        return EXP*(p_p[0] + 5.0*p_p[1] + 20.0 *p_p[2] + 60*p_p[3] + 120.0 * p_p[4] + 120.0 *p_p[5]);
}
inline double A4(double * p_p, double & EXP)
{
	return EXP*(p_p[0] + 4.0*p_p[1] + 12.0*p_p[2] + 24.0*p_p[3] + 24.0*p_p[4] );
}
inline double A3(double * p_p, double & EXP)
{
	return EXP*( p_p[0] + 3.0 * p_p[1] + p_p[2] *6.0 + p_p[3]*6.0);
}
inline double A2(double * p_p, double & EXP)
{
	return EXP*( 2.0*p_p[2] + 2.0*p_p[1] + p_p[0]);
}

inline double A1(double * p_p, double & EXP)
{
	return EXP*(p_p[0]+p_p[1]);
}
inline double A0(double * p_p, double & EXP)
{
//	cout << EXP << " " << p_p[0]
	return EXP*p_p[0];
}

inline double B6(double & cos_h, double &sin_h, double *p_pt){
    sin_h *(2.0*p_pt[0] + 60.0 *p_pt[2] + 720.0 * p_pt[4] +1440.0 * p_pt[6]) - cos_h * (12.0*p_pt[1] + 240.0 *p_pt[3] +1440.0 * p_pt[5]  );
}

inline double B5(double & cos_h, double &sin_h, double *p_pt ){
    return sin_h * ( 10.0*p_pt[1] + 120.0 *p_pt[3]+240.0*p_pt[5]  ) - cos_h * (2.0*p_pt[0] + 40*p_pt[2] + 240.0 * p_pt[4]);
}

inline double B4(double & cos_h, double & sin_h, double *p_pt)
{
	return sin_h * (2.0*p_pt[0] + 24.0*p_pt[2] + 48.0*p_pt[4] ) - cos_h * (8.0*p_pt[1] + 48.0*p_pt[3]) ;
}
inline double B3(double & cos_h, double & sin_h, double *p_pt)
{
	return sin_h * ( 6.0*p_pt[1] + 12.0 *p_pt[3] ) - cos_h * (2.0*p_pt[0] + 12.0 *p_pt[2]);
}
inline double B2(double & cos_h, double & sin_h, double *p_pt)
{
//	cout << sin_h*(2.0*p_pt[0]+4.0*p_pt[2]) << " " << -cos_h*4.0*p_pt[1] << endl;
	return sin_h*(2.0*p_pt[0]+4.0*p_pt[2])-cos_h*4.0*p_pt[1];
}
inline double B1(double & cos_h, double & sin_h, double *p_pt)
{
//	cout << sin_h*2.0 << " " << p
	return sin_h*2.0*p_pt[1] - 2.0*cos_h*p_pt[0];
}

inline double B0(double &cos_h, double & sin_h, double *p_pt){
    	return sin_h*2.0*p_pt[0];
}
inline double B0(double & sin_h, double *p_pt)
{
//	cout << sin_h * 2.0 << " " << p_pt[0] << endl; //checked
	return sin_h*2.0*p_pt[0];
}
/////////////////////////////////////////////////////////////////////////
int init_HASH(){
    double p;
    double t, pt;
    double p_int=(PMAX-PMIN)/HASH_S;
    int i=0;
    double p_p[5];
    double p_pt[5];
    double EXP, cos_h, sin_h;
    p=p_int/2.0;
    t=(-0.425/2.825);
    for ( i=0; i< HASH_S; i++)
    {
	pt =p*t;
	EXP = exp( -p);
	p_p[0]=1.0/p;
	p_pt[0]=1.0/pt;
	for(int j=1;j<5;j++) p_pt[j] = p_pt[j-1] /pt;
	for(int j=1;j<5;j++) p_p[j] = p_p[j-1] / p;
	cos_h = cosh(pt);
	sin_h = sinh(pt);
	_1S1S[i]     = p*p*p / 6.0 * (A2(p_p, EXP) *3.0 - A0(p_p, EXP));
	_2S2S[i]     = EXP * (1.0 + p + 4.0 *p*p / 9.0 + p*p*p / 9.0 + p*p*p*p/45.0);
	_2S2Sig[i]   = p*p*p*p*p / ( 60.0 * sqrt(3.0) ) * ( 5*A3(p_p, EXP) - A1( p_p, EXP) );
        _2Sig2Sig[i] = p*p*p*p*p / 120.0 * ( 5.0 * A4(p_p, EXP) - 18.0 * A2(p_p, EXP) + 5.0 * A0(p_p, EXP) );
	_2Pi2Pi[i]   =  p*p*p*p*p / 120.0 * ( 5.0 * A4(p_p, EXP) -  6.0 * A2(p_p, EXP) + A0(p_p, EXP) );
	_1S2S_hc[i]  =  p*p*p*p / ( 8.0 * sqrt(3.0 )) * pow(1-t, 2.5) * pow(1+t, 1.5) *
             ( A3(p_p,EXP) * B0(sin_h, p_pt)        - A2(p_p, EXP) * B1(cos_h, sin_h, p_pt) -
               A1(p_p,EXP) * B2(cos_h, sin_h, p_pt) + A0(p_p, EXP) * B3(cos_h, sin_h, p_pt) )  ;
	_1S2Sig_hc[i] = p*p*p*p / 8.0 * pow(1-t, 2.5) * pow(1+t, 1.5) *
          (-A3(p_p,EXP) * B1(cos_h, sin_h, p_pt) + A2(p_p, EXP) * B0(sin_h, p_pt)       +
           A1(p_p,EXP) * B3(cos_h, sin_h, p_pt) - A0(p_p, EXP) * B2(cos_h, sin_h, p_pt) ) ;
	p+=p_int;
    }
    _1S1S[HASH_S]=0.0;
    _2S2S[HASH_S]=0.0;
    _2S2Sig[HASH_S]=0.0;
    _2Sig2Sig[HASH_S]=0.0;
    _2Pi2Pi[HASH_S]=0.0;
    _1S2S_hc[HASH_S]=0.0;
    _1S2Sig_hc[HASH_S]=0.0;
    return 0;
}

int mol_and_orb::calc_F_el_with_HASH( double & p,  double & t,  double & beta, double & x, double & y, double & z, int & c1, int & c2, int &lbl1, int &lbl2)  {

static int i,j;
static int P;
if ( p > PMAX) P=HASH_S;
else P=int ( ((p-PMIN)/(PMAX-PMIN))*double (HASH_S) );
//these are the sto overlap factors//
static double IsIs, IsIIsig,IsIIs, IsIIIsig, IsIIIs;
static double IIsIIs, IIsIIsig, IIsigIIs, IIsIIIs, IIsIIIsig;
static double IIsigIIsig, IIpiIIpi, IIsigIIIsig, IIsigIIIpi, IIpiIIIsig, IIpiIIIpi, IIsigIIIs, IIIsigIIs, IIIsIIsig;
static double IIIsigIIIsig, IIIpiIIIpi, IIIsIIIs, IIIsigIIIs, IIIsIIIsig;
/////////////////////////////////////

static const double f_sig=1.267;
static const double f_pi  =0.585; // these two factors are the orbital overlap fudge factors

///these are variables that i will use to make calculations faster/////////////////
static double EXP;
static double sin_h;
static double cos_h;
static double pt;

double p_pt[7]; // this will contain the powers of 1/pt^[i+1]
double p_p[7]; // this will contain the powers of 1/p^[i+1] -where i is the index!
///////////////////////////////////////////////////////////////////////////////////


static const double TOLL=1E-10; //for checking t ;0)

if (lbl1==0 && lbl2==0){
	if(t<TOLL){
	   IsIs = _1S1S[P];
	}
	else{
		cout << "There is an error if you are here... this should only do H-H!" << endl;
		if(t<0) t=-t;
		pt = p*t;
		for(i=0;i<5;i++)p_pt[i]=pow(pt,-i-1);
		for(i=0;i<5;i++)p_p[i] =pow(p ,-i-1);
		cos_h = cosh(pt);
		sin_h = sinh(pt);
		EXP = exp(-p);
		IsIs = p*p*p/4*pow( (1-t*t), 1.5 ) * (A2(p_p, EXP) * B0(sin_h,p_pt) - B2(cos_h,sin_h,p_pt)*A0(p_p,EXP));
	//	out_pt_1s1s << pt << '\t'<< p << '\t' << IsIs <<endl;
	}

	F[c1][c2]=beta*IsIs;

}

else if (lbl1>1 && lbl2>1 &&lbl1<9 && lbl2<9 ){ //both row two
	if ( t < TOLL && t > -TOLL){ 
	        if ( lbl1 == 5 && use_hash==true){
			IIsIIs = _2S2S[P];

			IIsIIsig = _2S2Sig[P] ;

			IIsigIIsig = _2Sig2Sig[P];

			IIpiIIpi = _2Pi2Pi[P];
			IIsigIIs = IIsIIsig;
		}
		else {
		        EXP = exp(-p);
			p_p[0]=1.0/p;
			for(i=1;i<5;i++) p_p[i] = p_p[i-1] / p;

		    	IIsIIs     = p*p*p*p*p/360.0 * (15.0 * A4( p_p, EXP) - 10.0 * A2( p_p, EXP) + 3.0 * A0( p_p, EXP) ) ;
			IIsIIsig   =  p*p*p*p*p / ( 60.0 * sqrt(3.0) ) * ( 5*A3(p_p, EXP) - A1( p_p, EXP) );
			IIsigIIsig =  p*p*p*p*p / 120.0 * ( 5.0 * A4(p_p, EXP) - 18.0 * A2(p_p, EXP) + 5.0 * A0(p_p, EXP) );
			IIpiIIpi   =  p*p*p*p*p / 120.0 * ( 5.0 * A4(p_p, EXP) -  6.0 * A2(p_p, EXP) + A0(p_p, EXP) );
			IIsigIIs   = IIsIIsig;
		}
		//out_p_2s2s << p << '\t' << IIsIIs << endl;
		//out_p_2s2sig << p << '\t' <<  IIsIIsig <<endl;
	        //out_p_2sig2sig<< p<<'\t' << IIsigIIsig <<endl;
	        //out_p_2pi2pi << p <<'\t' << IIpiIIpi << endl;
	}
	else{

                pt = p*t;
//		for(i=0;i<5;i++)p_pt[i]=pow(pt,-i-1);
//		for(i=0;i<5;i++)p_p[i] =pow(p ,-i-1);

		p_p[0]=1.0/p;
		for(i=1;i<5;i++) p_p[i] = p_p[i-1] / p;

		p_pt[0]=1.0/pt;
		for(i=1;i<5;i++) p_pt[i] = p_pt[i-1] / pt;

		cos_h = cosh(pt);
		sin_h = sinh(pt);
		EXP = exp(-p);



		IIsIIs= p*p*p*p*p / 48.0 * pow( 1-t*t, 2.5) * ( A4(p_p,EXP) * B0(sin_h, p_pt) - 2.0 *
				A2(p_p, EXP) * B2(cos_h,sin_h,p_pt) + A0(p_p, EXP) * B4(cos_h, sin_h, p_pt) )  ;

                IIsIIsig= p*p*p*p*p / (16.0*sqrt(3.0)) * pow( 1-t*t, 2.5) * (
			  A3(p_p,EXP) * ( B0(sin_h, p_pt)      - B2(cos_h,sin_h,p_pt) ) +
			  A1(p_p,EXP) * ( B4(cos_h,sin_h,p_pt) - B2(cos_h,sin_h,p_pt) ) +
			  B1(cos_h,sin_h,p_pt) * ( A2(p_p, EXP) - A4(p_p, EXP) ) 	+
			  B3(cos_h,sin_h,p_pt) * ( A2(p_p, EXP) - A0(p_p, EXP) ) ) ;

                IIpiIIpi= p*p*p*p*p / 32.0 * pow( 1-t*t, 2.5) * (
			  A4(p_p, EXP) * ( B0(sin_h,p_pt) - B2(cos_h,sin_h,p_pt)) +
			  A2(p_p, EXP) * ( B4(cos_h,sin_h,p_pt) - B0(sin_h,p_pt)) +
			  A0(p_p, EXP) * ( B2(cos_h,sin_h,p_pt) - B4(cos_h,sin_h,p_pt)) );

                IIsigIIsig= p*p*p*p*p / 16.0 * pow( 1-t*t, 2.5) * (
			    B2(cos_h,sin_h,p_pt) * ( A0(p_p, EXP) + A4(p_p, EXP) ) -
			    A2(p_p, EXP) * ( B0(sin_h,p_pt) + B4(cos_h,sin_h,p_pt) ));
		for(i=0;i<5;i+=2) p_pt[i]=-p_pt[i];
		pt=-pt;
		cos_h = cosh(pt);
		sin_h = sinh(pt);
		IIsigIIs =  p*p*p*p*p / (16.0*sqrt(3.0)) * pow( 1-t*t, 2.5) * (
                          A3(p_p,EXP) * ( B0(sin_h, p_pt)      - B2(cos_h,sin_h,p_pt) ) +
                          A1(p_p,EXP) * ( B4(cos_h,sin_h,p_pt) - B2(cos_h,sin_h,p_pt) ) +
                          B1(cos_h,sin_h,p_pt) * ( A2(p_p, EXP) - A4(p_p, EXP) )        +
                          B3(cos_h,sin_h,p_pt) * ( A2(p_p, EXP) - A0(p_p, EXP) ) ) ;

		//out_pt_2s2s << pt << '\t'<< p <<'\t' << IIsIIs << endl;
		//out_pt_2s2sig << pt << '\t'<< p <<'\t' << IIsIIsig << endl;
		//out_pt_2sig2sig << pt << '\t'<< p <<'\t' << IIsigIIsig << endl;
		//out_pt_2pi2pi << pt << '\t'<< p <<'\t' << IIpiIIpi << endl;

	}

///////set the value of those elements that do not require calculation of geometric factors
        F[c1][c2]=beta * IIsIIs; // <2s|2s>
        F[c1+1][c2]=beta * IIsigIIs * (x); // <2px|2s>
        F[c1+2][c2]=beta * IIsigIIs * (y); // <2py|2s>
        F[c1+3][c2]=beta * IIsigIIs * (z); // <2pz|2s>

	F[c1][c2+1]=beta * IIsIIsig * (-x); // <2s| 2px>
	F[c1][c2+2]=beta * IIsIIsig * (-y);  //  <2s |2py> etc.
	F[c1][c2+3]=beta * IIsIIsig * (-z);
////////////////////////////////////////////////////////////////////////////////////////////

///////////in this section we calculate the geometric factors///////////
	if ( Abs(x) <= TOLL && Abs(y) <= TOLL){
		F[c1+1][c2+1]= beta * f_pi * IIpiIIpi  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_pi * IIpiIIpi  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_sig * IIsigIIsig; // <2pz|2pz>

	}
	else if (Abs(y) <= TOLL && Abs(z)<=TOLL){
		F[c1+1][c2+1]= beta * f_sig * IIsigIIsig  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_pi * IIpiIIpi  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_pi * IIpiIIpi; // <2pz|2pz>
	}
	else if (Abs(x)<=TOLL && Abs(z) <= TOLL){
		F[c1+1][c2+1]= beta * f_pi * IIpiIIpi  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_sig * IIsigIIsig  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_pi * IIpiIIpi; // <2pz|2pz>
	}
	else{
		double px, py, pz, p2x, p2y, p2z ;
		px = y*z; // these three are one perpendicular vector 
		py = -2*x*z;
		pz = x*y;
		p2x = py * z - pz * y; // these is  another
		p2y = pz * x - px * z;
		p2z = px * y - py * x;
		double n_p, n_p2; // variables to help normalise p1, p2
		n_p = sqrt ( px*px + py*py + pz*pz);
		n_p2 = sqrt ( p2x*p2x + p2y*p2y + p2z*p2z);
		if (n_p == 0.0 || n_p2 == 0.0) {cout << "Error with making the geometrical factors " <<endl;return 1;}
		px = px / n_p;
		py = py / n_p;
		pz = pz / n_p;
		p2x = p2x / n_p2;
       		p2y = p2y / n_p2;
	        p2z = p2z / n_p2;

		///////////////////////////////////////////////////////////////////////
//		cout << " the pi pi overlap is: " << IIpiIIpi << " and the sigma: " << IIsigIIsig << " The 2s sigma is: " << IIsIIsig << " and the sigma 2s is: " << IIsigIIs << endl;

      		F[c1+1][c2+1]= beta * ( f_sig*x*x * IIsigIIsig +  f_pi * IIpiIIpi * (px * px + p2x * p2x ) )  ; //<2px| 2px
		F[c1+2][c2+1]= beta * ( f_sig*x*y * IIsigIIsig +  f_pi * IIpiIIpi * (px * py + p2x * p2y ) ) ; //<2px | 2py>
		F[c1+3][c2+1]= beta * ( f_sig*x*z * IIsigIIsig +  f_pi * IIpiIIpi * (px * pz + p2x * p2z ) );//<2px | 2pz>

        	F[c1+1][c2+2] = F[c1+2][c2+1] ; // <2py| 2px>
        	F[c1+2][c2+2] = beta * (f_sig*y*y * IIsigIIsig +  f_pi * IIpiIIpi * (py * py + p2y * p2y ) ) ;//<2py | 2py>
        	F[c1+3][c2+2] = beta * (f_sig*y*z * IIsigIIsig +  f_pi * IIpiIIpi * (py * pz + p2y * p2z ) ) ;// <2py| 2pz>

	        F[c1+1][c2+3] = F[c1+3][c2+1] ; // <2pz|2px>
       		F[c1+2][c2+3] = F[c1+3][c2+2] ;// <2pz|2py>
	        F[c1+3][c2+3] = beta * (f_sig*z*z * IIsigIIsig +  f_pi * IIpiIIpi * (pz * pz + p2z * p2z) ) ; // <2pz|2pz>
	}

}

else if (lbl1>1 && lbl1 <9 && lbl2==0){ //row 2 vs row 1
	if( t < TOLL &&  t > -TOLL){ // not going to happen really guv!
		EXP = exp(-p);
		p_p[0]=1.0/p;
		for(i=1;i<5;i++) p_p[i] = p_p[i-1] / p;

		IsIIs = p*p*p*p/( 12.0 * sqrt(3.0) ) * ( 3.0*A3(p_p,EXP) - A1(p_p,EXP) ) ; // could write sqrt(3)?
		IsIIsig = p*p*p*p/12.0*(3.0*A2(p_p,EXP) - A0(p_p,EXP) ) ;
		//out_p_1s2s << p << '\t' << IsIIs << endl;
		//out_p_1s2sig << p << '\t' << IsIIsig <<endl;
	}
	else{
		t=-t; //Jo Dog! this line is WELL important, we can only calculate <1s|2s> , not <2sig|1s>: this latter overlap will be - the previous 
		if ( Abs(t + 0.15044247787610619469026548672566 ) < TOLL && use_hash==true){
		    IsIIs = _1S2S_hc[P];
		    IsIIsig = _1S2Sig_hc[P];
		}
		else {

			pt = p*t;
//		for(i=0;i<5;i++)p_pt[i]=pow(pt,-i-1);
//		for(i=0;i<5;i++)p_p[i] =pow(p ,-i-1);

			p_p[0]=1.0/p;
        	        for(i=1;i<5;i++) p_p[i] = p_p[i-1] / p;
			p_pt[0]=1.0/pt;
                	for(i=1;i<5;i++) p_pt[i] = p_pt[i-1] / pt;

			cos_h = cosh(pt);
			sin_h = sinh(pt);
			EXP = exp(-p);


			IsIIs =   p*p*p*p / ( 8.0 * sqrt(3.0 )) * pow(1-t, 2.5) * pow(1+t, 1.5) *
			( A3(p_p,EXP) * B0(sin_h, p_pt)        - A2(p_p, EXP) * B1(cos_h, sin_h, p_pt) -
			  A1(p_p,EXP) * B2(cos_h, sin_h, p_pt) + A0(p_p, EXP) * B3(cos_h, sin_h, p_pt) )  ;

			IsIIsig = p*p*p*p / 8.0 * pow(1-t, 2.5) * pow(1+t, 1.5) *
			(-A3(p_p,EXP) * B1(cos_h, sin_h, p_pt) + A2(p_p, EXP) * B0(sin_h, p_pt)       +
                          A1(p_p,EXP) * B3(cos_h, sin_h, p_pt) - A0(p_p, EXP) * B2(cos_h, sin_h, p_pt) ) ;
		}
//		cout << " A3: " << A3(p_p,EXP) << " B0 " << B0(sin_h, p_pt) << " A2 " << A2(p_p, EXP) << " B1: " << B1(cos_h, sin_h, p_pt) << " A1: " << A1(p_p,EXP) <<  " B2: " << B2(cos_h, sin_h, p_pt) << " A0: " << A0(p_p, EXP) << " B3: " << B3(cos_h, sin_h, p_pt) << endl;
       		//out_pt_1s2s  << pt << '\t'<< p <<'\t' << IsIIs << endl ;
		//out_pt_1s2sig << pt <<'\t'<< p <<'\t' << IsIIsig <<endl;
	}

	F[c1][c2]=beta*IsIIs; // <2s|1s>
        F[c1+1][c2]=beta*(IsIIsig)*(x); // <2px|1s>
        F[c1+2][c2]=beta*(IsIIsig)*(y); // <2py|1s>
        F[c1+3][c2]=beta*(IsIIsig)*(z); // <2pz|1s>
}
else if (lbl1==0 && lbl2>1 && lbl2 <9){ //row 1 and row 2
//	cout << "nc2==1 && nc1==4"<< endl;
	if( t < TOLL &&  t > -TOLL){
                EXP = exp(-p);
                p_p[0]=1.0/p;
                for(i=1;i<5;i++) p_p[i] = p_p[i-1] / p;
                p_pt[0]=1.0/pt;
                for(i=1;i<5;i++) p_pt[i] = p_pt[i-1] / pt;


		IsIIs = p*p*p*p/( 12.0 * sqrt(3.0) ) * ( 3.0*A3(p_p,EXP) - A1(p_p,EXP) ); // could write sqrt(3)?
                IsIIsig = p*p*p*p/12.0*(3.0*A2(p_p,EXP) - A0(p_p,EXP) ) ;

		//out_p_1s2s << p << '\t' << IsIIs << endl;
		//out_p_1s2sig << p << '\t' << IsIIsig <<endl;
        }
        else{
                if ( Abs(t + 0.1504424778761061946902654872566 ) < TOLL && use_hash==true){
		   IsIIs = _1S2S_hc[P];
		   IsIIsig = _1S2Sig_hc[P];
		}
		else{
			pt = p*t;
//              for(i=0;i<5;i++)p_pt[i]=pow(pt,-i-1);
//              for(i=0;i<5;i++)p_p[i] =pow(p ,-i-1);
			p_pt[0]=1.0/pt;
			for(i=1;i<5;i++) p_pt[i] = p_pt[i-1] / pt;
			p_p[0]=1.0/p;
			for(i=1;i<5;i++) p_p[i] = p_p[i-1] / p;

        	        cos_h = cosh(pt);
                	sin_h = sinh(pt);
            		EXP = exp(-p);

              		IsIIs =   p*p*p*p / ( 8.0 * sqrt(3.0 )) * pow( 1.0 + t, 1.5) * pow (1.0 - t, 2.5) *
                        ( A3(p_p,EXP) * B0(sin_h, p_pt)        - A2(p_p, EXP) * B1(cos_h, sin_h, p_pt) -
                          A1(p_p,EXP) * B2(cos_h, sin_h, p_pt) + A0(p_p, EXP) * B3(cos_h, sin_h, p_pt) )  ;

                	IsIIsig = p*p*p*p / 8.0 * sqrt(1.0-t*t) * (1-t*t) * (1-t) *
                        (-A3(p_p,EXP) * B1(cos_h, sin_h, p_pt) + A2(p_p, EXP) * B0(sin_h, p_pt)       +
                          A1(p_p,EXP) * B3(cos_h, sin_h, p_pt) - A0(p_p, EXP) * B2(cos_h, sin_h, p_pt) ) ;
		}

//		cout << " A3: " << A3(p_p,EXP) << " B0 " << B0(sin_h, p_pt) << " A2 " << A2(p_p, EXP) << " B1: " << B1(cos_h, sin_h, p_pt) << " A1: " << A1(p_p,EXP) <<  " B2: " << B2(cos_h, sin_h, p_pt) << " A0: " << A0(p_p, EXP) << " B3: " << B3(cos_h, sin_h, p_pt) << endl;
		//out_pt_1s2s  << pt << '\t'<< p <<'\t' << IsIIs <<endl;
	        //out_pt_1s2sig << pt << '\t'<< p <<'\t' << IsIIsig <<endl;
        }

	F[c1][c2]=beta*IsIIs; //<1s|2s>
	F[c1][c2+1]=beta*IsIIsig*(-x);//<1s|2px>
	F[c1][c2+2]=beta*IsIIsig*(-y);//<1s|2py>
	F[c1][c2+3]=beta*IsIIsig*(-z);
}
else if(lbl1 > 9 && lbl2 > 9){ //row 3 row3
    if (t<TOLL && t > -TOLL){
	    EXP = exp(-p);	
	    p_p[0] =1.0/p;
	    for (i =1;i<7;i++) p_p[i] = p_p[i-1]/p;
	    IIIsIIIs      = p*p*p*p*p*p*p/25200.0 *(35.0 * A6(p_p, EXP) -35.0 *A4(p_p,EXP) +21.0 * A2(p_p, EXP) -5.0 *A0(p_p,EXP) );
	    IIIsIIIsig    = p*p*p*p*p*p*p/(sqrt(3.0) * 12600.0) * (35.0*A5(p_p,EXP) -14.0*A3(p_p,EXP) +3.0*A1(p_p,EXP)   );
	    IIIsigIIIsig  = p*p*p*p*p*p*p/25200.0 *(35.0 * A6(p_p,EXP) -147.0 * A4(p_p, EXP) +85.0*A2(p_p, EXP)-21.0*A0(p_p,EXP) );
	    IIIpiIIIpi    = p*p*p*p*p*p*p/25200.0 *(35.0*A6(p_p,EXP) - 49.0*A4(p_p,EXP) + 17.0*A2(p_p, EXP) -3.0*A0(p_p,EXP));
	    IIIsigIIIs  = IIIsIIIsig;
    }
    else {
	    pt = p*t;
	    p_pt[0]=1.0/pt;
            for(i=1;i<7;i++) p_pt[i] = p_pt[i-1] / pt;
            cos_h = cosh(pt);
	    sin_h = sinh(pt);

	    EXP = exp(-p);	
	    p_p[0] =1.0/p;
	    for (i =1;i<7;i++) p_p[i] = p_p[i-1]/p;

	    IIIsIIIs   =  p*p*p*p*p*p*p/1440.0 * (1-t)*(1-t)*(1-t)*sqrt(1-t) *
				(A6(p_p,EXP) *B0(cos_h,sin_h, p_pt) -3.0*A4(p_p,EXP) *B2(cos_h, sin_h, p_pt)
			       +3.0* A2(p_p,EXP) *B4(cos_h, sin_h, p_pt) - A0(p_p,EXP) *B6(cos_h,sin_h, p_pt)	)  ;
	    IIIsIIIsig = p*p*p*p*p*p*p/(sqrt(3.0) *480.0 ) * (1-t)*(1-t)*(1-t)*sqrt(1-t)*
	       		   (- A6(p_p, EXP)*B1(cos_h,sin_h,p_pt) + A5(p_p, EXP) *(B0(cos_h,sin_h,p_pt)-B2(cos_h,sin_h,p_pt)) 
			    + A4(p_p, EXP)*(B1(cos_h,sin_h,p_pt)+2.0*B3(cos_h,sin_h,p_pt)) + 2.0 *A3(p_p, EXP) *(B4(cos_h,sin_h,p_pt)-B2(cos_h,sin_h,p_pt))
			    - A2(p_p, EXP) * (2.0*B3(cos_h,sin_h,p_pt)+B5(cos_h,sin_h,p_pt)) + A1(p_p, EXP) * (B4(cos_h,sin_h,p_pt)-B6(cos_h,sin_h,p_pt)) 
			    + A0(p_p, EXP) * B5(cos_h,sin_h,p_pt));
	    IIIpiIIIpi   = p*p*p*p*p*p*p/(960.0 ) * (1-t)*(1-t)*(1-t)*sqrt(1-t) *
			   (A6(p_p, EXP) * (B0(cos_h,sin_h,p_pt)-B2(cos_h,sin_h,p_pt)) + 
			    A4(p_p, EXP) * (2.0*B4(cos_h,sin_h,p_pt)-B0(cos_h,sin_h,p_pt)-B2(cos_h,sin_h,p_pt)) +
			    A2(p_p, EXP) * (2.0*B2(cos_h,sin_h,p_pt)-B6(cos_h,sin_h,p_pt)-B4(cos_h,sin_h,p_pt)) +
			    A0(p_p, EXP) * (B6(cos_h,sin_h,p_pt)-B4(cos_h,sin_h,p_pt)) );
	    IIIsigIIIsig =  p*p*p*p*p*p*p/(480.0 ) * (1-t)*(1-t)*(1-t)*sqrt(1-t) * 
			    (A6(p_p, EXP) * B2(cos_h,sin_h,p_pt) - A4(p_p, EXP) *(B0(cos_h,sin_h,p_pt)+2.0*B4(cos_h,sin_h,p_pt)) 
			     + A2(p_p, EXP)*(B6(cos_h,sin_h,p_pt)+2.0*B2(cos_h,sin_h,p_pt)) - A0(p_p, EXP)*B4(cos_h,sin_h,p_pt) ) ;
	    t=-t;
            pt = p*t;
            p_pt[0]=1.0/pt;
            for(i=1;i<7;i+=2) p_pt[i] = -p_pt[i];
            cos_h = cosh(pt);
            sin_h = sinh(pt);
	    IIIsigIIIs   =  p*p*p*p*p*p*p/(sqrt(3.0) *480.0 ) * (1-t)*(1-t)*(1-t)*sqrt(1-t)*
	       		   (- A6(p_p, EXP)*B1(cos_h,sin_h,p_pt) + A5(p_p, EXP) *(B0(cos_h,sin_h,p_pt)-B2(cos_h,sin_h,p_pt)) 
			    + A4(p_p, EXP)*(B1(cos_h,sin_h,p_pt)+2.0*B3(cos_h,sin_h,p_pt)) + 2.0 *A3(p_p, EXP) *(B4(cos_h,sin_h,p_pt)-B2(cos_h,sin_h,p_pt))
			    - A2(p_p, EXP) * (2.0*B3(cos_h,sin_h,p_pt)+B5(cos_h,sin_h,p_pt)) + A1(p_p, EXP) * (B4(cos_h,sin_h,p_pt)-B6(cos_h,sin_h,p_pt)) 
			    + A0(p_p, EXP) * B5(cos_h,sin_h,p_pt));
    }
    F[c1][c2]=beta * IIIsIIIs; // <3s|3s>
    F[c1+1][c2]=beta * IIIsIIIsig * (x); // <3px|3s>
    F[c1+2][c2]=beta * IIIsIIIsig * (y); // <3py|3s>
    F[c1+3][c2]=beta * IIIsIIIsig * (z); // <3pz|3s>

    F[c1][c2+1]=beta * IIIsigIIIs * (-x); // <3s| 3px>
    F[c1][c2+2]=beta * IIIsigIIIs * (-y);  //  <3s |3py> etc.
    F[c1][c2+3]=beta * IIIsigIIIs * (-z);
////////////////////////////////////////////////////////////////////////////////////////////

///////////in this section we calculate the geometric factors///////////
    if ( Abs(x) <= TOLL && Abs(y) <= TOLL){
		F[c1+1][c2+1]= beta * f_pi * IIIpiIIIpi  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_pi * IIIpiIIIpi  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_sig * IIIsigIIIsig; // <2pz|2pz>

     }
     else if (Abs(y) <= TOLL && Abs(z)<=TOLL){
		F[c1+1][c2+1]= beta * f_sig * IIIsigIIIsig  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_pi * IIIpiIIIpi  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_pi * IIIpiIIIpi; // <2pz|2pz>
     }
     else if (Abs(x)<=TOLL && Abs(z) <= TOLL){
		F[c1+1][c2+1]= beta * f_pi * IIIpiIIIpi  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_sig * IIIsigIIIsig  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_pi * IIIpiIIIpi; // <2pz|2pz>
     }
     else{
		double px, py, pz, p2x, p2y, p2z ;
		px = y*z; // these three are one set of perpendicular dimensions
		py = -2*x*z;
		pz = x*y;
		p2x = py * z - pz * y; // these are another
		p2y = pz * x - px * z;
		p2z = px * y - py * x;
		double n_p, n_p2; // variables to help normalise p1, p2
		n_p = sqrt ( px*px + py*py + pz*pz);
		n_p2 = sqrt ( p2x*p2x + p2y*p2y + p2z*p2z);
		if (n_p == 0.0 || n_p2 == 0.0) {cout << "Error with making the geometrical factors " <<endl;return 1;}
		px = px / n_p;
		py = py / n_p;
		pz = pz / n_p;
		p2x = p2x / n_p2;
       		p2y = p2y / n_p2;
	        p2z = p2z / n_p2;


		///////////////////////////////////////////////////////////////////////


//		cout << " the pi pi overlap is: " << IIIpiIIIpi << " and the sigma: " << IIIsigIIIsig << " The 2s sigma is: " << IIIsIIIsig << " and the sigma 2s is: " << IIIsigIIIs << endl;

      		F[c1+1][c2+1]= beta * ( f_sig*x*x * IIIsigIIIsig +  f_pi * IIIpiIIIpi * (px * px + p2x * p2x ) )  ; //<2px| 2px
		F[c1+2][c2+1]= beta * ( f_sig*x*y * IIIsigIIIsig +  f_pi * IIIpiIIIpi * (px * py + p2x * p2y ) ) ; //<2px | 2py>
		F[c1+3][c2+1]= beta * ( f_sig*x*z * IIIsigIIIsig +  f_pi * IIIpiIIIpi * (px * pz + p2x * p2z ) );//<2px | 2pz>

        	F[c1+1][c2+2] = F[c1+2][c2+1] ; // <2py| 2px>
        	F[c1+2][c2+2] = beta * (f_sig*y*y * IIIsigIIIsig +  f_pi * IIIpiIIIpi * (py * py + p2y * p2y ) ) ;//<2py | 2py>
        	F[c1+3][c2+2] = beta * (f_sig*y*z * IIIsigIIIsig +  f_pi * IIIpiIIIpi * (py * pz + p2y * p2z ) ) ;// <2py| 2pz>

	        F[c1+1][c2+3] = F[c1+3][c2+1] ; // <2pz|2px>
       		F[c1+2][c2+3] = F[c1+3][c2+2] ;// <2pz|2py>
	        F[c1+3][c2+3] = beta * (f_sig*z*z * IIIsigIIIsig +  f_pi * IIIpiIIIpi * (pz * pz + p2z * p2z) ) ; // <2pz|2pz>
      }
}

else if(lbl1 > 1 && lbl1 < 9 && lbl2 > 9){ //row 2 row 3
    //careful with Ts here!!!
    pt = p*t; 
    p_pt[0]=1.0/pt; 
    for(i=1;i<7;i++) p_pt[i] = p_pt[i-1] / pt; 
    cos_h = cosh(pt); 
    sin_h = sinh(pt); 
    EXP = exp(-p);	
    p_p[0] =1.0/p; 
    for (i =1;i<7;i++) p_p[i] = p_p[i-1]/p;
    IIsIIIs     = p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(48.0*sqrt(30.0))*
       		  (A5(p_p, EXP) * B0(cos_h,sin_h,p_pt) - A4(p_p, EXP) *  B1(cos_h,sin_h,p_pt) - 2.0 * A3(p_p, EXP) * B2(cos_h,sin_h,p_pt) 
		   +2.0* A2(p_p, EXP) * B3(cos_h,sin_h,p_pt) + A1(p_p, EXP) * B4(cos_h,sin_h,p_pt) -A0(p_p, EXP) * B5(cos_h,sin_h,p_pt) );
    IIpiIIIpi   = p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(32.0 * sqrt(30.0)) *
		  (A5(p_p, EXP) * (B0(cos_h,sin_h,p_pt) -B2(cos_h,sin_h,p_pt) ) + A4(p_p, EXP) * (B3(cos_h,sin_h,p_pt) - B1(cos_h,sin_h,p_pt)) + 
		   A3(p_p, EXP) * (B4(cos_h,sin_h,p_pt) -B0(cos_h,sin_h,p_pt))  + A2(p_p, EXP) * (B1(cos_h,sin_h,p_pt) - B5(cos_h,sin_h,p_pt)) + 
		   A1(p_p, EXP) * (B2(cos_h,sin_h,p_pt) -B4(cos_h,sin_h,p_pt) ) + A0(p_p, EXP) * (B5(cos_h,sin_h,p_pt) - B3(cos_h,sin_h,p_pt))  ) ;
    IIsIIIsig   = p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(48.0*sqrt(10.0))* 
		  (-A5(p_p, EXP) * B1(cos_h,sin_h,p_pt) + A4(p_p, EXP) * B0(cos_h,sin_h,p_pt) + 2.0 * A3(p_p, EXP) * B3(cos_h,sin_h,p_pt) - 
		   2.0 * A2(p_p, EXP) * B2(cos_h,sin_h,p_pt) - A1(p_p, EXP) * B5(cos_h,sin_h,p_pt) + A0(p_p, EXP) * B4(cos_h,sin_h,p_pt) ) ;
    IIsigIIIs  =  p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(48.0*sqrt(10.0))* 
	          (A4(p_p, EXP) * (B0(cos_h,sin_h,p_pt) - 2.0 * B2(cos_h,sin_h,p_pt) ) + A1(p_p, EXP) * (2*B3(cos_h,sin_h,p_pt) - B5(cos_h,sin_h,p_pt) )  
		   + B1(cos_h,sin_h,p_pt) * (A5(p_p, EXP) - 2.0 * A3(p_p, EXP) ) +B4(cos_h,sin_h,p_pt) * (2.0*A2(p_p, EXP) - A0(p_p, EXP) ) );
    IIsigIIIsig = p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(16.0*sqrt(30.0))*
       		  (A2(p_p, EXP) * (B1(cos_h,sin_h,p_pt) + B5(cos_h,sin_h,p_pt) ) - A3(p_p, EXP) * (B0(cos_h,sin_h,p_pt) + B4(cos_h,sin_h,p_pt) ) 
		   - B3(cos_h,sin_h,p_pt) * (A0(p_p, EXP) + A4(p_p, EXP) ) + B2(cos_h,sin_h,p_pt) * ( A1(p_p, EXP) +A5(p_p, EXP) )  ) ;
////////////////easy geometric factors
    F[c1][c2]=beta * IIsIIIs; // <2s|3s>
    F[c1+1][c2]=beta * IIsigIIIs * (x); // <2px|3s>
    F[c1+2][c2]=beta * IIsigIIIs * (y); // <2py|3s>
    F[c1+3][c2]=beta * IIsigIIIs * (z); // <2pz|3s>

    F[c1][c2+1]=beta * IIsIIIsig * (-x); // <2s| 3px>
    F[c1][c2+2]=beta * IIsIIIsig * (-y);  //  <2s |3py> etc.
    F[c1][c2+3]=beta * IIsIIIsig * (-z);
///////////in this section we calculate the geometric factors///////////
    if ( Abs(x) <= TOLL && Abs(y) <= TOLL){
		F[c1+1][c2+1]= beta * f_pi * IIpiIIIpi  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_pi * IIpiIIIpi  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_sig * IIsigIIIsig; // <2pz|2pz>

     }
     else if (Abs(y) <= TOLL && Abs(z)<=TOLL){
		F[c1+1][c2+1]= beta * f_sig * IIsigIIIsig  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_pi * IIpiIIIpi  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_pi * IIpiIIIpi; // <2pz|2pz>
     }
     else if (Abs(x)<=TOLL && Abs(z) <= TOLL){
		F[c1+1][c2+1]= beta * f_pi * IIpiIIIpi  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_sig * IIsigIIIsig  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_pi * IIpiIIIpi; // <2pz|2pz>
     }
     else{
		double px, py, pz, p2x, p2y, p2z ;
		px = y*z; // these three are one set of perpendicular dimensions
		py = -2*x*z;
		pz = x*y;
		p2x = py * z - pz * y; // these are another
		p2y = pz * x - px * z;
		p2z = px * y - py * x;
		double n_p, n_p2; // variables to help normalise p1, p2
		n_p = sqrt ( px*px + py*py + pz*pz);
		n_p2 = sqrt ( p2x*p2x + p2y*p2y + p2z*p2z);
		if (n_p == 0.0 || n_p2 == 0.0) {cout << "Error with making the geometrical factors " <<endl;return 1;}
		px = px / n_p;
		py = py / n_p;
		pz = pz / n_p;
		p2x = p2x / n_p2;
       		p2y = p2y / n_p2;
	        p2z = p2z / n_p2;


		///////////////////////////////////////////////////////////////////////


//		cout << " the pi pi overlap is: " << IIpiIIIpi << " and the sigma: " << IIIsigIIIsig << " The 2s sigma is: " << IIIsIIIsig << " and the sigma 2s is: " << IIIsigIIIs << endl;

      		F[c1+1][c2+1]= beta * ( f_sig*x*x * IIsigIIIsig +  f_pi * IIpiIIIpi * (px * px + p2x * p2x ) )  ; //<2px| 2px
		F[c1+2][c2+1]= beta * ( f_sig*x*y * IIsigIIIsig +  f_pi * IIpiIIIpi * (px * py + p2x * p2y ) ) ; //<2px | 2py>
		F[c1+3][c2+1]= beta * ( f_sig*x*z * IIsigIIIsig +  f_pi * IIpiIIIpi * (px * pz + p2x * p2z ) );//<2px | 2pz>

        	F[c1+1][c2+2] = F[c1+2][c2+1] ; // <2py| 2px>
        	F[c1+2][c2+2] = beta * (f_sig*y*y * IIsigIIIsig +  f_pi * IIpiIIIpi * (py * py + p2y * p2y ) ) ;//<2py | 2py>
        	F[c1+3][c2+2] = beta * (f_sig*y*z * IIsigIIIsig +  f_pi * IIpiIIIpi * (py * pz + p2y * p2z ) ) ;// <2py| 2pz>

	        F[c1+1][c2+3] = F[c1+3][c2+1] ; // <2pz|2px>
       		F[c1+2][c2+3] = F[c1+3][c2+2] ;// <2pz|2py>
	        F[c1+3][c2+3] = beta * (f_sig*z*z * IIsigIIIsig +  f_pi * IIpiIIIpi * (pz * pz + p2z * p2z) ) ; // <2pz|2pz>
      }
}
else if( lbl1 > 9 && lbl2 > 1 && lbl2 < 9 ){ //row 3 row 2
//careful with Ts here!!!
//     cout << "Warning! Using untested Sulphur code! Take care!" <<endl;   
    t = -t;//check this out!
    pt = p*t; 
    p_pt[0]=1.0/pt; 
    for(i=1;i<7;i++) p_pt[i] = p_pt[i-1] / pt; 
    cos_h = cosh(pt); 
    sin_h = sinh(pt); 
    EXP = exp(-p);	
    p_p[0] =1.0/p; 
    for (i =1;i<7;i++) p_p[i] = p_p[i-1]/p;
    IIsIIIs     = p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(48.0*sqrt(30.0))*
       		  (A5(p_p, EXP) * B0(cos_h,sin_h,p_pt) - A4(p_p, EXP) *  B1(cos_h,sin_h,p_pt) - 2.0 * A3(p_p, EXP) * B2(cos_h,sin_h,p_pt) 
		   +2.0*A2(p_p, EXP) * B3(cos_h,sin_h,p_pt) + A1(p_p, EXP) * B4(cos_h,sin_h,p_pt) -A0(p_p, EXP) * B5(cos_h,sin_h,p_pt) );
    IIpiIIIpi   = p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(32.0 * sqrt(30.0)) *
		  (A5(p_p, EXP) * (B0(cos_h,sin_h,p_pt) -B2(cos_h,sin_h,p_pt) ) + A4(p_p, EXP) * (B3(cos_h,sin_h,p_pt) - B1(cos_h,sin_h,p_pt)) + 
		   A3(p_p, EXP) * (B4(cos_h,sin_h,p_pt) -B0(cos_h,sin_h,p_pt))  + A2(p_p, EXP) * (B1(cos_h,sin_h,p_pt) - B5(cos_h,sin_h,p_pt)) + 
		   A1(p_p, EXP) * (B2(cos_h,sin_h,p_pt) -B4(cos_h,sin_h,p_pt) ) + A0(p_p, EXP) * (B5(cos_h,sin_h,p_pt) - B3(cos_h,sin_h,p_pt))  ) ;
    IIIsigIIs   = p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(48.0*sqrt(10.0))* 
		  (-A5(p_p, EXP) * B1(cos_h,sin_h,p_pt) + A4(p_p, EXP) * B0(cos_h,sin_h,p_pt) + 2.0 * A3(p_p, EXP) * B3(cos_h,sin_h,p_pt) - 
		   2.0 * A2(p_p, EXP) * B2(cos_h,sin_h,p_pt) - A1(p_p, EXP) * B5(cos_h,sin_h,p_pt) + A0(p_p, EXP) * B4(cos_h,sin_h,p_pt) ) ;
    IIIsIIsig  =  p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(48.0*sqrt(10.0))* 
	          (A4(p_p, EXP) * (B0(cos_h,sin_h,p_pt) - 2.0 * B2(cos_h,sin_h,p_pt) ) + A1(p_p, EXP) * (2*B3(cos_h,sin_h,p_pt) - B5(cos_h,sin_h,p_pt) )  
		   + B1(cos_h,sin_h,p_pt) * (A5(p_p, EXP) - 2.0 * A3(p_p, EXP) ) +B4(cos_h,sin_h,p_pt) * (2.0*A2(p_p, EXP) - A0(p_p, EXP) ) );
    IIsigIIIsig = p*p*p*p*p*p*pow(1+t,2.5)*pow(1-t,3.5)/(16.0*sqrt(30.0))*
       		  (A2(p_p, EXP) * (B1(cos_h,sin_h,p_pt) + B5(cos_h,sin_h,p_pt) ) - A3(p_p, EXP) * (B0(cos_h,sin_h,p_pt) + B4(cos_h,sin_h,p_pt) ) 
		   - B3(cos_h,sin_h,p_pt) * (A0(p_p, EXP) + A4(p_p, EXP) ) + B2(cos_h,sin_h,p_pt) * ( A1(p_p, EXP) +A5(p_p, EXP) )  ) ;
////////////////easy geometric factors (might be the other wat around)
    F[c1][c2]=beta * IIsIIIs; // <2s|3s>
    F[c1+1][c2]=beta * IIIsigIIs * (x); // <3px|2s>
    F[c1+2][c2]=beta * IIIsigIIs * (y); // <3py|2s>
    F[c1+3][c2]=beta * IIIsigIIs * (z); // <3pz|2s>

    F[c1][c2+1]=beta * IIIsIIsig * (-x); // <3s| 2px>
    F[c1][c2+2]=beta * IIIsIIsig * (-y);  //  <3s |2py> etc.
    F[c1][c2+3]=beta * IIIsIIsig * (-z);
    
///////////in this section we calculate the geometric factors///////////
    if ( Abs(x) <= TOLL && Abs(y) <= TOLL){
		F[c1+1][c2+1]= beta * f_pi * IIpiIIIpi  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_pi * IIpiIIIpi  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_sig * IIsigIIIsig; // <2pz|2pz>

     }
     else if (Abs(y) <= TOLL && Abs(z)<=TOLL){
		F[c1+1][c2+1]= beta * f_sig * IIsigIIIsig  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_pi * IIpiIIIpi  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_pi * IIpiIIIpi; // <2pz|2pz>
     }
     else if (Abs(x)<=TOLL && Abs(z) <= TOLL){
		F[c1+1][c2+1]= beta * f_pi * IIpiIIIpi  ; //<2px| 2px
                F[c1+2][c2+1]= 0.0 ; //<2px | 2py>
                F[c1+3][c2+1]= 0.0 ;//<2px | 2pz>

                F[c1+1][c2+2] = 0.0 ; // <2py| 2px>
                F[c1+2][c2+2] = beta * f_sig * IIsigIIIsig  ;//<2py | 2py>
                F[c1+3][c2+2] = 0.0   ;// <2py| 2pz>

                F[c1+1][c2+3] = 0.0 ; // <2pz|2px>
                F[c1+2][c2+3] = 0.0 ;// <2pz|2py>
                F[c1+3][c2+3] = beta *  f_pi * IIpiIIIpi; // <2pz|2pz>
     }
     else{
		double px, py, pz, p2x, p2y, p2z ;
		px = y*z; // these three are one set of perpendicular dimensions
		py = -2*x*z;
		pz = x*y;
		p2x = py * z - pz * y; // these are another
		p2y = pz * x - px * z;
		p2z = px * y - py * x;
		double n_p, n_p2; // variables to help normalise p1, p2
		n_p = sqrt ( px*px + py*py + pz*pz);
		n_p2 = sqrt ( p2x*p2x + p2y*p2y + p2z*p2z);
		if (n_p == 0.0 || n_p2 == 0.0) {cout << "Error with making the geometrical factors " <<endl;return 1;}
		px = px / n_p;
		py = py / n_p;
		pz = pz / n_p;
		p2x = p2x / n_p2;
       		p2y = p2y / n_p2;
	        p2z = p2z / n_p2;


		///////////////////////////////////////////////////////////////////////


//		cout << " the pi pi overlap is: " << IIpiIIIpi << " and the sigma: " << IIIsigIIIsig << " The 2s sigma is: " << IIIsIIIsig << " and the sigma 2s is: " << IIIsigIIIs << endl;

      		F[c1+1][c2+1]= beta * ( f_sig*x*x * IIsigIIIsig +  f_pi * IIpiIIIpi * (px * px + p2x * p2x ) )  ; //<2px| 2px
		F[c1+2][c2+1]= beta * ( f_sig*x*y * IIsigIIIsig +  f_pi * IIpiIIIpi * (px * py + p2x * p2y ) ) ; //<2px | 2py>
		F[c1+3][c2+1]= beta * ( f_sig*x*z * IIsigIIIsig +  f_pi * IIpiIIIpi * (px * pz + p2x * p2z ) );//<2px | 2pz>

        	F[c1+1][c2+2] = F[c1+2][c2+1] ; // <2py| 2px>
        	F[c1+2][c2+2] = beta * (f_sig*y*y * IIsigIIIsig +  f_pi * IIpiIIIpi * (py * py + p2y * p2y ) ) ;//<2py | 2py>
        	F[c1+3][c2+2] = beta * (f_sig*y*z * IIsigIIIsig +  f_pi * IIpiIIIpi * (py * pz + p2y * p2z ) ) ;// <2py| 2pz>

	        F[c1+1][c2+3] = F[c1+3][c2+1] ; // <2pz|2px>
       		F[c1+2][c2+3] = F[c1+3][c2+2] ;// <2pz|2py>
	        F[c1+3][c2+3] = beta * (f_sig*z*z * IIsigIIIsig +  f_pi * IIpiIIIpi * (pz * pz + p2z * p2z) ) ; // <2pz|2pz>
      }

}
else if( lbl1 <2 && lbl2 >9){ //row 1 row 3
    pt = p*t; 
    p_pt[0]=1.0/pt; 
    for(i=1;i<7;i++) p_pt[i] = p_pt[i-1] / pt; 
    cos_h = cosh(pt); 
    sin_h = sinh(pt); 
    EXP = exp(-p);	
    p_p[0] =1.0/p; 
    for (i =1;i<7;i++) p_p[i] = p_p[i-1]/p;

    IsIIIs   = p*p*p*p*p*pow(1+t,1.5 )*pow(1-t,3.5 )/(24.0 * sqrt(10.0) )*
       		( A4(p_p, EXP) * B0(cos_h,sin_h,p_pt) -2.0 * A3(p_p, EXP) * B1(cos_h,sin_h,p_pt) 
		  + 2.0 * A1(p_p, EXP) * B3(cos_h,sin_h,p_pt) -A0(p_p, EXP) * B4(cos_h,sin_h,p_pt) )	;
    IsIIIsig = p*p*p*p*p*pow(1+t,1.5 )*pow(1-t,3.5 )/(8.0 * sqrt(30.0) )*
		( A3(p_p, EXP) * (B0(cos_h,sin_h,p_pt) + B2(cos_h,sin_h,p_pt) ) - A1(p_p, EXP) * ( B2(cos_h,sin_h,p_pt) + B4(cos_h,sin_h,p_pt) )
		  - B1(cos_h,sin_h,p_pt) * ( A2(p_p, EXP) + A4(p_p, EXP)) + B3(cos_h,sin_h,p_pt) * ( A0(p_p, EXP) + A2(p_p, EXP) ) );
 
    F[c1][c2]=beta*IsIIs; //<1s|3s>
    F[c1][c2+1]=beta*IsIIsig*(-x);//<1s|3px>
    F[c1][c2+2]=beta*IsIIsig*(-y);//<1s|3py>
    F[c1][c2+3]=beta*IsIIsig*(-z);
}
else if(lbl1 > 9 && lbl2 < 1){ //row 3 row 1
    t=-t;
    pt = p*t; 
    p_pt[0]=1.0/pt; 
    for(i=1;i<7;i++) p_pt[i] = p_pt[i-1] / pt; 
    cos_h = cosh(pt); 
    sin_h = sinh(pt); 
    EXP = exp(-p);	
    p_p[0] =1.0/p; 
    for (i =1;i<7;i++) p_p[i] = p_p[i-1]/p;
 
    IsIIIs   = p*p*p*p*p*pow(1+t,1.5 )*pow(1-t,3.5 )/(24.0 * sqrt(10.0) )*
       		( A4(p_p, EXP) * B0(cos_h,sin_h,p_pt) -2.0 * A3(p_p, EXP) * B1(cos_h,sin_h,p_pt) 
		  + 2.0 * A1(p_p, EXP) * B3(cos_h,sin_h,p_pt) -A0(p_p, EXP) * B4(cos_h,sin_h,p_pt) )	;
    IsIIIsig = p*p*p*p*p*pow(1+t,1.5 )*pow(1-t,3.5 )/(8.0 * sqrt(30.0) )*
		( A3(p_p, EXP) * (B0(cos_h,sin_h,p_pt) + B2(cos_h,sin_h,p_pt) ) - A1(p_p, EXP) * ( B2(cos_h,sin_h,p_pt) + B4(cos_h,sin_h,p_pt) )
		  - B1(cos_h,sin_h,p_pt) * ( A2(p_p, EXP) + A4(p_p, EXP)) + B3(cos_h,sin_h,p_pt) * ( A0(p_p, EXP) + A2(p_p, EXP) ) );
 
    F[c1][c2]=beta*IsIIIs; // <3s|1s>
    F[c1+1][c2]=beta*(IsIIIsig)*(x); // <3px|1s>
    F[c1+2][c2]=beta*(IsIIIsig)*(y); // <3py|1s>
    F[c1+3][c2]=beta*(IsIIIsig)*(z); // <3pz|1s>
}
else{
	cout << "error in the input of find F function, expecting only row 1 , row 2 or row 3 elements" << endl;
	return 1;
}

return 0;
}

int mol_and_orb::calc_F_lean( mol_and_orb B, int const& debug){


int i,j,k,l,m,n;
int n_mol1=N;
int n_mol2=B.N;

///////////////////////////////////////////////
////////generate Fi////////////////////////////


// fill up F, one element - i.e. more than 1 basis set at a time
int nb1_tmp; // counts how many basis sets we need to do at a time
int nb2_tmp;

double beta_av; // the value of the average beta
double p; // these two variables have the same meaning as in the mulliken paper
double t;

int b_c1; // counter for the basis set on mol1 that we are doing
int b_c2; // counter for the basis set on mol2 that we are doing

int a,b; // temporary counters to make writing easier

double dx,dy,dz; // doubles describing the relative position of 2 and 1
double length;

b_c1=0;
for(i=0;i<n_mol1;i++){

	a= lbl[i];
	if(a<=2) nb1_tmp=1;
	else nb1_tmp=4; 
	b_c2=0; // set the counter for mol2 to zero
	for(j=0;j<n_mol2;j++) {

 		b=B.lbl[j];

///////////set common position///////////////////////
		dx = B.pos[j].x - pos[i].x ;
		dy = B.pos[j].y - pos[i].y ;
		dz = B.pos[j].z - pos[i].z ;
		length = sqrt( dx*dx + dy*dy + dz*dz);
		dx = dx / length;
		dy = dy / length;
		dz = dz / length;
//		length = length / Ra; // express the length in Bohr
//////////////////////////////////////////////////////

///////////set t and p///////////////////////////////

		p = ( Mu[a] + Mu[b] ) * length / 2;
		t = ( Mu[a] - Mu[b] ) / ( Mu[a] + Mu[b] ) ; // in the forMulae on the paper t => 0
/////////////////////////////////////////////////////

///////////calculate av_beta//////////////////////////////
		beta_av = (Beta[a] + Beta[b])/2.0;
		if (debug ==2 || debug == -1 ) cout << " beta av " << beta_av << endl;
//////////////////////////////////////////////////////////

		if (b<2) nb2_tmp=1;
		else nb2_tmp=4;

		if ( calc_F_el_with_HASH(p,t,beta_av,dx,dy,dz,b_c1,b_c2,a,b) != 0 ) {
			cout << "Error making F elements" << endl;  return 1; } ;
		b_c2+=nb2_tmp;
	}
	b_c1+=nb1_tmp;
}
return 0;
}


int mol_and_orb::init_nocharge(char * nameinput, char * nameorbs){
	int i=0,j=0, k=0;
	int check;
	// read molecule//////////////////////////////////////////////////////////////////////////////////////

	ifstream in;
	in.open(nameinput);
	if(!in){cout << "Error, file " << nameinput << "does not exist" << endl; return 1;}
	string  xch,ych,zch;
	double  Xtmp, Ytmp, Ztmp;
	n_el=0;
	coord centre( 0.0, 0.0 ,0.0);
	string word;
	while ( in >> word){
		if(i%4==0) type[j] = word[0];
		else if (i%4==1) xch = word;
		else if (i%4==2) ych = word;
		else if (i%4==3) {
			zch = word;
			if(type[j] != 'C' && type[j] != 'O' && type[j] != 'H' && type[j]!= 'N' && type[j]!='S')
			{
				cout << "Bad atom type" << endl;
				return 1;
			}
			//set up labls, basis set ///////////////////////////////
			else if (type[j] == 'C' ){  // definition of notation used:
				bs[k] = 'S'; // 		S == 2s OR 3s
				bs[k+1] = 'x'; // 		x == 2px OR 3px
				bs[k+2] = 'y'; // 		y == 2py OR 3py
				bs[k+3] = 'z'; // 		z == 2pz OR 3pz : bs is being used only for rotorb
				k+=4;
				lbl[j] = 5;
				n_el += 4;
			}
			else if (type[j] == 'N') {
				bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 6;
				n_el += 5;
			}
			else if (type[j] == 'O' ){
				bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 7;
				n_el += 6;
			}
			else if (type[j] == 'S' ){
			        bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 15;
				n_el += 6;
			}
			else if (type[j] == 'H' ){
				bs[k] = 's';
				k+=1;
				lbl[j] = 0;
				n_el +=1;
			}
			

			////////////////////////////////////////////////////////
			sscanf(xch.c_str() , "%lf", &Xtmp );
			sscanf(ych.c_str() , "%lf", &Ytmp );
			sscanf(zch.c_str() , "%lf", &Ztmp );
			pos[j].set( Xtmp, Ytmp, Ztmp);
			pos[j]=pos[j]/RA;
			centre = centre + pos[j];
			j++;
		}
		i++;
	}
	N = j; 
	if (N>NAT_MAX){
	    cerr << N <<endl;
	    cerr << "increase NAT_MAX and recompile"<<endl;
	    return 2;
	}
	NBasis = k;
	if (NBasis > NBASIS_MAX ) {
	    cerr << NBasis <<endl;
	    cerr << "Increase NBASIS_MAX and recompile"<<endl;
	    return 2;
	}
	Xtmp = centre.x/double(j);
	Ytmp = centre.y/double(j);
	Ztmp = centre.z/double(j);	

	centre.set( Xtmp, Ytmp, Ztmp );

	for (i=0;i<N;i++){
		pos[i] = pos[i] - centre;
	}
	centre = centre - centre;
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// read orbs /////////////////////////////////////////////////////////////////////////////////////////
	check = read_orb(0, nameorbs); // chose 1 if you want to debug this procedure
	if (check != 0) { cout << " Error in reading orbitals " << endl; return 2; }


	static bool initialised_hash=false;
	if (initialised_hash == false ){
		init_HASH();
		initialised_hash == true;
	}

	return 0;
}

int mol_and_orb::init(char * nameinput, char * nameorbs, char * nameneutr, char * namecat, char * namean){
	int i=0,j=0, k=0;
	int check;
	// read molecule//////////////////////////////////////////////////////////////////////////////////////

	ifstream in;
	in.open(nameinput);
	if(!in){cout << "Error, file " << nameinput << "does not exist" << endl; return 1;}
	string  xch,ych,zch;
	double  Xtmp, Ytmp, Ztmp;
	n_el=0;
	coord centre( 0.0, 0.0 ,0.0);
	string word;
	while ( in >> word){
		if(i%4==0) type[j] = word[0];
		else if (i%4==1) xch = word;
		else if (i%4==2) ych = word;
		else if (i%4==3) {
			zch = word;
			if(type[j] != 'C' && type[j] != 'O' && type[j] != 'H' && type[j]!= 'N' && type[j] != 'S')
			{
				cout << "Bad atom type" << endl;
				return 1;
			}
			//set up labls, basis set ///////////////////////////////
			else if (type[j] == 'C' ){  // definition of notation used:
				bs[k] = 'S'; // 		S == 2s
				bs[k+1] = 'x'; // 		x == 2px
				bs[k+2] = 'y'; // 		y == 2py
				bs[k+3] = 'z'; // 		z == 2pz
				k+=4;
				lbl[j] = 5;
				n_el += 4;
			}
			else if (type[j] == 'N') {
				bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 6;
				n_el += 5;
			}
			else if (type[j] == 'O' ){
				bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 7;
				n_el += 6;
			}	
			else if (type[j] == 'S' ){
			        bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 15;
				n_el += 6;
			}
			else if (type[j] == 'H' ){
				bs[k] = 's';
				k+=1;
				lbl[j] = 0;
				n_el +=1;
			}
			

			////////////////////////////////////////////////////////
			sscanf(xch.c_str() , "%lf", &Xtmp );
			sscanf(ych.c_str() , "%lf", &Ytmp );
			sscanf(zch.c_str() , "%lf", &Ztmp );
			pos[j].set( Xtmp, Ytmp, Ztmp);
			pos[j]=pos[j]/RA;
			centre = centre + pos[j];
			j++;
		}
		i++;
	}
	N = j;
	if (N>NAT_MAX){
	    cerr << N <<endl;
	    cerr << "increase NAT_MAX and recompile"<<endl;
	    return 2;
	}
	NBasis = k;
	if (NBasis > NBASIS_MAX){
	    cerr << NBasis <<endl;
	    cerr << "Increase NBASIS_MAX and recompile" <<endl;
	    return 2;
	}
	Xtmp = centre.x/double(j);
	Ytmp = centre.y/double(j);
	Ztmp = centre.z/double(j);	

	centre.set( Xtmp, Ytmp, Ztmp );

	for (i=0;i<N;i++){
		pos[i] = pos[i] - centre;
	}
	centre = centre - centre;
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// read orbs /////////////////////////////////////////////////////////////////////////////////////////
	check = read_orb(0, nameorbs); // chose 1 if you want to debug this procedure
	if (check != 0) { cout << " Error in reading orbitals " << endl; return 2; }

	//read charges////////////////////////////////////////////////////////////////////////////////////////
	check = neutr.read_crg_eps(nameneutr, type, N);
	if (check != 0 ) {cout << "Error in reading charges" << endl; return 3;}
	check = cat.read_crg_eps(namecat, type, N);
	if (check != 0 ) {cout << "Error in reading charges" << endl; return 3;}
	check = an.read_crg_eps(namean, type, N );
	if (check != 0 ) {cout << "Error in reading charges" << endl; return 3;}


	static bool initialised_hash=false;
	if (initialised_hash == false ){
		init_HASH();
		initialised_hash == true;
	}

	return 0;
}

int mol_and_orb::init_nocharge_noorb(char * nameinput){
	int i=0,j=0, k=0;
	int check;
	// read molecule//////////////////////////////////////////////////////////////////////////////////////

	ifstream in;
	in.open(nameinput);
	if(!in){cout << "Error, file " << nameinput << "does not exist" << endl; return 1;}
	string  xch,ych,zch;
	double  Xtmp, Ytmp, Ztmp;
	n_el=0;
	coord centre( 0.0, 0.0 ,0.0);
	string word;
	while ( in >> word){
		if(i%4==0) type[j] = word[0];
		else if (i%4==1) xch = word;
		else if (i%4==2) ych = word;
		else if (i%4==3) {
			zch = word;
			if(type[j] != 'C' && type[j] != 'O' && type[j] != 'H' && type[j]!= 'N' && type[j]!='S')
			{
				cout << "Bad atom type" << endl;
				return 1;
			}
			//set up labls, basis set ///////////////////////////////
			else if (type[j] == 'C' ){  // definition of notation used:
				bs[k] = 'S'; // 		S == 2s
				bs[k+1] = 'x'; // 		x == 2px
				bs[k+2] = 'y'; // 		y == 2py
				bs[k+3] = 'z'; // 		z == 2pz
				k+=4;
				lbl[j] = 5;
				n_el += 4;
			}
			else if (type[j] == 'S' ){
			        bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 15;
				n_el += 6;
			}
			else if (type[j] == 'N') {
				bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 6;
				n_el += 5;
			}
			else if (type[j] == 'O' ){
				bs[k] = 'S';
				bs[k+1] = 'x';
				bs[k+2] = 'y';
				bs[k+3] = 'z';
				k+=4;
				lbl[j] = 7;
				n_el += 6;
			}
			else if (type[j] == 'H' ){
				bs[k] = 's';
				k+=1;
				lbl[j] = 0;
				n_el +=1;
			}
			

			////////////////////////////////////////////////////////
			sscanf(xch.c_str() , "%lf", &Xtmp );
			sscanf(ych.c_str() , "%lf", &Ytmp );
			sscanf(zch.c_str() , "%lf", &Ztmp );
			pos[j].set( Xtmp, Ytmp, Ztmp);
			pos[j]=pos[j]/RA;
			centre = centre + pos[j];
			j++;
		}
		i++;
	}
	N = j; 
	if (N>NAT_MAX){
	    cerr << N <<endl;
	    cerr << "increase NAT_MAX and recompile"<<endl;
	    return 2;
	}
	NBasis = k;
	if (NBasis > NBASIS_MAX){
	    cerr << NBasis <<endl;
	     cerr << "increase NBasis_MAX and recompile"<<endl;
	     return 2;
	}
	
	Xtmp = centre.x/double(j);
	Ytmp = centre.y/double(j);
	Ztmp = centre.z/double(j);	

	centre.set( Xtmp, Ytmp, Ztmp );

	for (i=0;i<N;i++){
		pos[i] = pos[i] - centre;
	}
	centre = centre - centre;
	//////////////////////////////////////////////////////////////////////////////////////////////////////
}



void mol_and_orb::calcallJ(mol_and_orb &B , double  displ [3], double  rotation [3][3]){
	int check=0;

	// get geometric factors;///////////////////////////////////////
	coord shove(displ[0], displ[1], displ[2]);
	rot R(rotation);

	// rotate molecule, translate it////////////////////////////////
	rotate(R);
	rotate_crg(R);
	rotorbs(psi,bs, NBasis, R);
	shift(shove);
	// calc F///////////////////////////////////////////////////////
	check = calc_F_lean( B, 0);
	vector <double> results;
	// calc J //////////////////////////////////////////////////////
	for (int i=0; i < NBasis/2; i++){
		for (int j =0; j<NBasis/2;j++){
			double J=0.0;

			for (int k=0;k<NBasis;k++){
				for ( int l=0;l<NBasis;l++){
					J += F[k][l] * psi[i][k] * B.psi[j][l];
				}
			}
			cout << J << '\t'  ;
		}
		cout << endl;
	}

	//put molecules  back to the coordinates of B 
	cp_pos(B);
	cp_orb(B);
}

vector <double> mol_and_orb::calcJ(mol_and_orb &A, mol_and_orb &B, mol_and_orb & templ, vector<int*> J_labels){
        
  	int const debug=0; //this debugs the stupid unitary rotations facility

	//now I first have to obtian the rotation
	int check=0;


	// get geometric factors;///////////////////////////////////////
	// At the moment it is not working for some reason I think that F for the dymer aint workin
	rot R1;
	rot R2;
	coord shove1;
	coord shove2;

	//first find the displacements
	for (int i=0;i< A.N; i++){
		shove1 = shove1 + pos[i];
		shove2 = shove2 + pos[i+A.N];
	}
	shove1.x = shove1.x/double(A.N);
	shove1.y = shove1.y/double(A.N);
	shove1.z = shove1.z/double(A.N);

	shove2.x = shove2.x/double(A.N);
	shove2.y = shove2.y/double(A.N);
	shove2.z = shove2.z/double(A.N);

	if (debug==1) {
	    print();
	    cout << shove1 << shove2;
	    cout << "that was the original dymer";
	}
		
	//now get the rotations
	const int lbl1 = 12 ; 
	const int lbl2 = 22 ;
	
	coord tmp1, tmp2;
	tmp1 = pos[lbl1]-shove1;
	tmp2 = pos[lbl2]-shove1;
	R1 = get_eulerrot(A.pos[lbl1] , A.pos[lbl2], tmp1, tmp2 );
	tmp1 = pos[lbl1+A.N]-shove2;
	tmp2 = pos[lbl2+A.N]-shove2;
	R2 = get_eulerrot(B.pos[lbl1], B.pos[lbl2], tmp1, tmp2); 

	// rotate molecule, translate it////////////////////////////////
	A.rotate(R1);
	if (debug == 1){
	    A.print();
	}
	A.shift(shove1);
	if (debug == 1){
	    A.print();
	}
	B.rotate(R2);
	if (debug == 1){
	    B.print();
	}
	B.shift(shove2);
	if (debug == 1){
	    B.print();
	}

	// calc F///////////////////////////////////////////////////////
	check = A.calc_F_lean( B, 0);

	static double orb1 [NBASIS_MAX];
	static double orb2 [NBASIS_MAX];
		
	// calc J //////////////////////////////////////////////////////
	vector <double> results;
	for (int i=0; i < J_labels.size(); i++){
		double J=0.0;
		int n_int1 = (J_labels[i])[0];
		int n_int2 = (J_labels[i])[1];

		for (int j=0;j<NBasis;j++){
			orb1[j] = A.psi[n_int1][j];
			orb2[j] = B.psi[n_int2][j];
		}

		// rotate the orbnitals
		check = rot_orb(orb2, B.bs, B.NBasis, R2);
		check = rot_orb(orb1, A.bs, A.NBasis, R1);
		if (check != 0 ) {cout << " Error in rotating orbtial" << endl; return results;}

		for (int k=0;k<A.NBasis;k++){
			for ( int l=0;l<A.NBasis;l++){
				J += A.F[k][l] * orb1[k] * orb2[l];
			}
		}
		results.push_back(J);
	}



	//put molecules back to the coordinates of B 
	if (debug ==1 ){
	    A.print();
	    B.print();
	}
	A.cp_pos_crg(templ);    
	B.cp_pos_crg(templ);
	return results;
}

vector<double> mol_and_orb::calcJ(mol_and_orb &B , double  displ [3], double  rotation [3][3], vector<int*> J_labels){
	int check=0;
	
    cerr << "Entering mol_and_orb::calcJ(mol_and_orb &B , double  displ [3], double  rotation [3][3], vector<int*> J_labels, char * nameout)" << endl << std::flush;


    // get geometric factors;///////////////////////////////////////
	coord shove(displ[0], displ[1], displ[2]);
	rot R(rotation);

	// rotate molecule, translate it////////////////////////////////
	rotate(R);
	shift(shove);
	// calc F///////////////////////////////////////////////////////
	check = calc_F_lean( B, 0);
	vector <double> results;
	// calc J //////////////////////////////////////////////////////
	for (int i=0; i < J_labels.size(); i++){
		int n_int1 = (J_labels[i])[0];
		int n_int2 = (J_labels[i])[1];

		double orb1 [NBASIS_MAX];
		double orb2rot [NBASIS_MAX];
		for (int j=0;j<NBasis;j++){
			orb1[j] = psi[n_int1][j];
			orb2rot[j] = psi[n_int2][j];
		}

		// rotate the second orbnital
		check = rot_orb(orb2rot, bs, NBasis, R);
		if (check != 0 ) {cout << " Error in rotating orbtial" << endl; return results;}
		double J=0.0;

		for (int k=0;k<NBasis;k++){
			for ( int l=0;l<NBasis;l++){
				J += F[k][l] * orb1[k] * orb2rot[l];
			}
		}
		results.push_back ( J);
	}

	//put molecules  back to the coordinates of B 
	cp_pos(B);

	return results;
}

vector<double> mol_and_orb::calcJ(mol_and_orb & B , double  displ [3], double  rotation [3][3], vector<int*> J_labels, char * nameout){
	
    int check=0;

	// get geometric factors;///////////////////////////////////////
	coord shove(displ[0], displ[1], displ[2]);
	rot R(rotation);

	// rotate molecule, translate it////////////////////////////////
	rotate(R);
	shift(shove);
	// calc F///////////////////////////////////////////////////////
	check = calc_F_lean( B, 0);
	vector <double> results;
	// calc J //////////////////////////////////////////////////////

	for (int i=0; i < J_labels.size(); i++){
		int n_int1 = (J_labels[i])[0];
		int n_int2 = (J_labels[i])[1];

		double orb1 [NBASIS_MAX];
		double orb2rot [NBASIS_MAX];
		for (int j=0;j<NBasis;j++){
			orb1[j] = psi[n_int1][j];
			orb2rot[j] = psi[n_int2][j];
		}

		// rotate the second orbnital
		check = rot_orb(orb2rot, bs, NBasis, R);
		if (check != 0 ) {cout << " Error in rotating orbtial" << endl; return results;}
		double J=0.0;

		for (int k=0;k<NBasis;k++){
			for ( int l=0;l<NBasis;l++){
				J += F[k][l] * orb1[k] * orb2rot[l];
			}
		}
		results.push_back ( J);
	}

	ofstream out (nameout);
	out << 2*N<< '\n' << endl;
//        out << "# J: " << J << "  J*J: " << J*J << endl;
        out.setf(ios::scientific);

	for (int i =0 ; i<N; i++){
		out << type[i] << " " << pos[i]*RA;
	}
	for (int i =0 ;i<N;i++)
	{
		out << B.type[i] << " " <<B.pos[i]*RA;
	}

	//put molecules  back to the coordinates of B 
	cp_pos(B);

	return results;
}

vector<double> mol_and_orb::calcJ_DG(mol_and_orb &A, mol_and_orb &B, mol_and_orb & templ, vector<int*> J_labels){
        
  	int const debug=0; //this debugs the stupid unitary rotations facility

	//now I first have to obtian the rotation
	int check=0;

	vector <double> results;

	// get geometric factors;///////////////////////////////////////
	// At the moment it is not working for some reason I think that F for the dymer aint workin
	rot R1;
	rot R2;
	coord shove1;
	coord shove2;

	//first find the displacements
	for (int i=0;i< A.N; i++){
		shove1 = shove1 + pos[i];
		shove2 = shove2 + pos[i+A.N];
	}
	shove1.x = shove1.x/double(A.N);
	shove1.y = shove1.y/double(A.N);
	shove1.z = shove1.z/double(A.N);

	shove2.x = shove2.x/double(A.N);
	shove2.y = shove2.y/double(A.N);
	shove2.z = shove2.z/double(A.N);

	if (debug==1) {
	    print();
	    cout << shove1 << shove2;
	    cout << "that was the original dymer";
	}
		
	//now get the rotations
	const int lbl1 = 7; 
	const int lbl2 = 8;
	coord tmp1, tmp2;
	tmp1 = pos[lbl1]-shove1;
	tmp2 = pos[lbl2]-shove1;
	R1 = get_eulerrot(A.pos[lbl1] , A.pos[lbl2], tmp1, tmp2 );
	tmp1 = pos[lbl1+A.N]-shove2;
	tmp2 = pos[lbl2+A.N]-shove2;
	R2 = get_eulerrot(B.pos[lbl1], B.pos[lbl2], tmp1, tmp2); 

	// rotate molecule, translate it////////////////////////////////
	A.rotate(R1);
	if (debug == 1){
	    A.print();
	}
	A.shift(shove1);
	if (debug == 1){
	    A.print();
	}
	B.rotate(R2);
	if (debug == 1){
	    B.print();
	}
	B.shift(shove2);
	if (debug == 1){
	    B.print();
	}
	//First calculate the energy///////////////////////////////////
	double vdWnrg = A.exp_6( B );
	if ( vdWnrg > 1e+200 ){
	    for (int i=0;i< J_labels.size();i++){
		results.push_back(0.0);
	    }
	    results.push_back(vdWnrg);
	    results.push_back(0.0);
	    results.push_back(0.0);
	    results.push_back(0.0);
	    results.push_back(0.0);
	    results.push_back(0.0);
		
	    A.print();
	    B.print();
	    A.cp_pos( templ );
	    B.cp_pos( templ );	
	    
	    return results;
	} 
	A.rotate_crg(R1);
	B.rotate_crg(R2);
	// calc F///////////////////////////////////////////////////////
	check = A.calc_F_lean( B, 0);

	static double orb1 [NBASIS_MAX];
	static double orb2 [NBASIS_MAX];
		
	// calc J //////////////////////////////////////////////////////
	for (int i=0; i < J_labels.size(); i++){
		int n_int1 = (J_labels[i])[0];
		int n_int2 = (J_labels[i])[1];

		for (int j=0;j<NBasis;j++){
			orb1[j] = A.psi[n_int1][j];
			orb2[j] = B.psi[n_int2][j];
		}

		// rotate the orbnitals
		check = rot_orb(orb2, B.bs, B.NBasis, R2);
		check = rot_orb(orb1, A.bs, A.NBasis, R1);
		if (check != 0 ) {cout << " Error in rotating orbtial" << endl; return results;}
		double J=0.0;

		for (int k=0;k<A.NBasis;k++){
			for ( int l=0;l<A.NBasis;l++){
				J += A.F[k][l] * orb1[k] * orb2[l];
			}
		}
		results.push_back ( J);
	}

	double dg [5];

	A.nrg_EStone_part_mpls(dg, B);
	results.push_back( vdWnrg );
	results.push_back( dg[0]);
	results.push_back( dg[1]);
	results.push_back( dg[2]);
	results.push_back( dg[3]);
	results.push_back( dg[4]);

	//put molecules back to the coordinates of B 
	if (debug ==1 ){
	    A.print();
	    B.print();
	}
	A.cp_pos_crg(templ);    
	B.cp_pos_crg(templ);
	return results;
}



double mol_and_orb::calcS( int n1, int n2){
	double S=0.0;
	for (int k=0;k<NBasis;k++){
                                S += psi[n1][k] * psi[n2][k];
                }
	return S;
}


inline double KrDel(int a, int b){
    if (a == b ) return 1.0;
    else return 0.0;
}
/* use this to debug only 
void mol_and_orb::nrg_EStone_part( double *v1, mol_and_orb & B ){
	double T0;
	double T1[3];
	double T2[3][3];
	double T3[3][3][3];
	double T4[3][3][3][3];

	double r,r2,r3,r5,r7,r9 ;
	double R[3];

	v1[0]=0.0; //cat n
	v1[1]=0.0; // an n
	v1[2]=0.0; // n n 
	v1[3]=0.0; // n cat
	v1[4]=0.0; // n an

	for (int K=0;K<N;K++){
    		for (int L=0;L<B.N;L++){
			R[0]=B.pos[L].x - pos[K].x;
    			R[1]=B.pos[L].y - pos[K].y;
			R[2]=B.pos[L].z - pos[K].z;
			
			r  = 1 / (sqrt (R[0]*R[0] + R[1]*R[1] + R[2]*R[2] ) );
			r2 = 1 / (R[0]*R[0] + R[1]*R[1] + R[2]*R[2] );
			r3 = r2*r;
			r5 = r3*r2;
			r7 = r5*r2;
			r9 = r7*r2;

			//mon mon
			T0 = r;
			v1[0] += neutr.mpls[K]*B.cat.mpls[L]*T0;    
			v1[1] += neutr.mpls[K]*B.an.mpls[L]*T0;
			v1[2] += neutr.mpls[K]*B.neutr.mpls[L]*T0;
			v1[3] += cat.mpls[K]  *B.neutr.mpls[L]*T0;
			v1[4] += an.mpls[K]   *B.neutr.mpls[L]*T0;
		

			//mon dip
			for (int i=0;i<3;i++){
		    		T1[i]=-r3*R[i];

				v1[0] += T1[i] * ( neutr.mpls[K] * B.cat.dpls[L][i] - B.cat.mpls[L] * neutr.dpls[K][i] );
				v1[1] += T1[i] * ( neutr.mpls[K] * B.an.dpls[L][i] - B.an.mpls[L] * neutr.dpls[K][i] );
				v1[2] += T1[i] * ( neutr.mpls[K] * B.neutr.dpls[L][i] - B.neutr.mpls[L] *  neutr.dpls[K][i] );
				v1[3] += T1[i] * ( cat.mpls[K] * B.neutr.dpls[L][i] - B.neutr.mpls[L] * cat.dpls[K][i] );
				v1[4] += T1[i] * ( an.mpls[K] * B.neutr.dpls[L][i] - B.neutr.mpls[L] * an.dpls[K][i] );
			}
		
			//mon quad
			for (int i=0;i<3;i++)
			{
			    for (int j=0;j<3;j++)
			    {
				T2[i][j]=-KrDel(i,j) * r3 + 3.0 * r5 * R[i]*R[j];

				v1[0] += T2[i][j] * ( neutr.mpls[K] * B.cat.qdrpls[L][i][j] + B.cat.mpls[L] * neutr.qdrpls[K][i][j] );
				v1[1] += T2[i][j] * ( neutr.mpls[K] * B.an.qdrpls[L][i][j] + B.an.mpls[L] * neutr.qdrpls[K][i][j] );
				v1[2] += T2[i][j] * ( neutr.mpls[K] * B.neutr.qdrpls[L][i][j] + B.neutr.mpls[L] * neutr.qdrpls[K][i][j] );
				v1[3] += T2[i][j] * ( cat.mpls[K] * B.neutr.qdrpls[L][i][j] + B.neutr.mpls[L] * cat.qdrpls[K][i][j] );
				v1[4] += T2[i][j] * ( an.mpls[K] * B.neutr.qdrpls[L][i][j] + B.neutr.mpls[L] * an.qdrpls[K][i][j] );
			    }
			}
		
			//dip dip 
			for(int i=0;i<3;i++)
		        {
		            for (int j=0;j<3;j++)
			    {
				v1[0] -= T2[i][j] * neutr.dpls[K][i] * B.cat.dpls[L][j];
				v1[1] -= T2[i][j] * neutr.dpls[K][i] * B.an.dpls[L][j];
				v1[2] -= T2[i][j] * neutr.dpls[K][i] * B.neutr.dpls[L][j];
				v1[3] -= T2[i][j] * cat.dpls[K][i] * B.neutr.dpls[L][j];
				v1[4] -= T2[i][j] * an.dpls[K][i] * B.neutr.dpls[L][j];
			    }
			}
		
			//dip quad
			for (int i=0;i<3;i++){
			    for(int j=0; j<3; j++){
				for (int k=0;k<3;k++){
					T3[i][j][k] = 3.0 * ( r5 * (R[i] * KrDel(j,k) + R[j] * KrDel(i,k) + R[k] * KrDel(i,j) ) 
						     -5.0*r7*R[i]*R[j]*R[k] ); 	
					v1[0] += 1/3.0 * T3[i][j][k] *  ( - neutr.dpls[K][i] * B.cat.qdrpls[L][j][k] + B.cat.dpls[L][i] * neutr.qdrpls[K][j][k] );
					v1[1] += 1/3.0 * T3[i][j][k] *  ( - neutr.dpls[K][i] * B.an.qdrpls[L][j][k] + B.an.dpls[L][i] * neutr.qdrpls[K][j][k] );
					v1[2] += 1/3.0 * T3[i][j][k] *  ( - neutr.dpls[K][i] * B.neutr.qdrpls[L][j][k] + B.neutr.dpls[L][i] * neutr.qdrpls[K][j][k] );
					v1[3] += 1/3.0 * T3[i][j][k] *  ( - cat.dpls[K][i] * B.neutr.qdrpls[L][j][k] + B.neutr.dpls[L][i] * cat.qdrpls[K][j][k] );
					v1[4] += 1/3.0 * T3[i][j][k] *  ( - an.dpls[K][i] * B.neutr.qdrpls[L][j][k] + B.neutr.dpls[L][i] * an.qdrpls[K][j][k] );
				}
			    }
			}
		
			//quad quad
			for (int i=0;i<3;i++){
			    for(int j=0; j<3; j++){
				for (int k=0;k<3;k++){
					for (int l=0;l<3;l++){
					    T4[i][j][k][l] = 3.0 * ( r5 * ( KrDel(i,j)*KrDel(k,l) + KrDel(i,k)*KrDel(j,l) + KrDel(i,l) * KrDel(j,k) ) -
						           5.0*r7* ( R[i]*R[j]*KrDel(k,l) + R[i]*R[k]*KrDel(j,l) + R[i]*R[l]*KrDel(j,k) +
							             R[j]*R[k]*KrDel(i,l) + R[j]*R[l]*KrDel(i,k) + R[l]*R[k]*KrDel(i,j) ) +
							   35*r9*  (R[i]*R[j]*R[k]*R[l]) );
					    v1[0] += neutr.qdrpls[K][i][j] *B.cat.qdrpls[L][k][l]*T4[i][j][k][l] /9.0;
					    v1[1] += neutr.qdrpls[K][i][j] *B.an.qdrpls[L][k][l]*T4[i][j][k][l] /9.0;
					    v1[2] += neutr.qdrpls[K][i][j] *B.neutr.qdrpls[L][k][l]*T4[i][j][k][l] /9.0;
					    v1[3] += cat.qdrpls[K][i][j] *B.neutr.qdrpls[L][k][l]*T4[i][j][k][l] /9.0;
					    v1[4] += an.qdrpls[K][i][j] *B.neutr.qdrpls[L][k][l]*T4[i][j][k][l] /9.0;
					}
				}
			    }
			}
		}
	}
		

}

*/

void mol_and_orb::nrg_EStone_part_mpls( double * v1, mol_and_orb & B){
	double T0;

	double r ;
	double R[3];

	v1[0]=0.0; //neutr cat
	v1[1]=0.0; //neutr an
	v1[2]=0.0; //neutr neutr
	v1[3]=0.0; //cat  neutr
	v1[4]=0.0; //an   neutr
	

	for (int K=0;K<N;K++){
    		for (int L=0;L<B.N;L++){
			R[0]=B.pos[L].x - pos[K].x;
    			R[1]=B.pos[L].y - pos[K].y;
			R[2]=B.pos[L].z - pos[K].z;
			
			r  = 1 / (sqrt (R[0]*R[0] + R[1]*R[1] + R[2]*R[2] ) );
			//mon mon
			T0 = r;
			v1[0] += neutr.mpls[K]*B.cat.mpls[L]*T0;    
			v1[1] += neutr.mpls[K]*B.an.mpls[L]*T0;
			v1[2] += neutr.mpls[K]*B.neutr.mpls[L]*T0;
			v1[3] += cat.mpls[K]  *B.neutr.mpls[L]*T0;
			v1[4] += an.mpls[K]   *B.neutr.mpls[L]*T0;
		}
	}
	
}

void mol_and_orb::nrg_EStone_part( double * v1, mol_and_orb & B ){
	double T0;
	double T1[3];
	double T2[3][3];

	double r,r2,r3,r5,r7,r9 ;
	double R[3];

	v1[0]=0.0; //neutr cat
	v1[1]=0.0; //neutr an
	v1[2]=0.0; //neutr neutr
	v1[3]=0.0; //cat  neutr
	v1[4]=0.0; //an   neutr
	
	int i,j;

	for (int K=0;K<N;K++){
    		for (int L=0;L<B.N;L++){
			R[0]=B.pos[L].x - pos[K].x;
    			R[1]=B.pos[L].y - pos[K].y;
			R[2]=B.pos[L].z - pos[K].z;
			
			r  = 1 / (sqrt (R[0]*R[0] + R[1]*R[1] + R[2]*R[2] ) );
			r2 = 1 / (R[0]*R[0] + R[1]*R[1] + R[2]*R[2] );
			r3 = r2*r;
			r5 = r3*r2;
			r7 = r5*r2;
			r9 = r7*r2;

			//mon mon
			T0 = r;
			v1[0] += neutr.mpls[K]*B.cat.mpls[L]*T0;    
			v1[1] += neutr.mpls[K]*B.an.mpls[L]*T0;
			v1[2] += neutr.mpls[K]*B.neutr.mpls[L]*T0;
			v1[3] += cat.mpls[K]  *B.neutr.mpls[L]*T0;
			v1[4] += an.mpls[K]   *B.neutr.mpls[L]*T0;
		
			//mon dip 
			for ( i=0;i<3;i++){
		    		T1[i]=-r3*R[i];
				v1[0] += T1[i] * ( neutr.mpls[K] * B.cat.dpls[L][i] - B.cat.mpls[L] * neutr.dpls[K][i] );
				v1[1] += T1[i] * ( neutr.mpls[K] *  B.an.dpls[L][i] -  B.an.mpls[L] * neutr.dpls[K][i] );
				v1[2] += T1[i] * ( neutr.mpls[K] * B.neutr.dpls[L][i] - B.neutr.mpls[L] * neutr.dpls[K][i] );
				v1[3] += T1[i] * ( cat.mpls[K] * B.neutr.dpls[L][i] - B.neutr.mpls[L] * cat.dpls[K][i] );
				v1[4] += T1[i] * (  an.mpls[K] * B.neutr.dpls[L][i] -  B.neutr.mpls[L] * an.dpls[K][i] );
			}
			
			//mon quad
			for (int i=0;i<3;i++)
			{
			    j=i;
			    T2[i][j] = - r3 + 3.0 * r5 * R[i] * R[j];
			    v1[0] +=  T2[i][j]/3.0 * ( neutr.mpls[K] * B.cat.qdrpls[L][i][j] + B.cat.mpls[L] * neutr.qdrpls[K][i][j] );
			    v1[1] +=  T2[i][j]/3.0 * ( neutr.mpls[K] *  B.an.qdrpls[L][i][j] +  B.an.mpls[L] * neutr.qdrpls[K][i][j] );
			    v1[2] +=  T2[i][j]/3.0 * ( neutr.mpls[K] * B.neutr.qdrpls[L][i][j] + B.neutr.mpls[L] * neutr.qdrpls[K][i][j] );
			    v1[3] +=  T2[i][j]/3.0 * ( cat.mpls[K] * B.neutr.qdrpls[L][i][j] + B.neutr.mpls[L] * cat.qdrpls[K][i][j] );
			    v1[4] +=  T2[i][j]/3.0 * (  an.mpls[K] * B.neutr.qdrpls[L][i][j] +  B.neutr.mpls[L] * an.qdrpls[K][i][j] );
			    for (int j=i+1;j<3;j++)
			    {
			//	T2[i][j]=-KrDel(i,j) * r3 + 3.0 * r5 * R[i]*R[j];
			//	v1[2] += T2[i][j] * ( mpls[K] * B.qdrpls[L][i][j] + B.mpls[L] * qdrpls[K][i][j] );
				T2[i][j] = 3.0 *r5 * R[i] *R[j];
				T2[j][i] = T2[i][j];
			        v1[0] +=  2.0*T2[i][j]/3.0 * ( neutr.mpls[K] * B.cat.qdrpls[L][i][j] + B.cat.mpls[L] * neutr.qdrpls[K][i][j] );
			        v1[1] +=  2.0*T2[i][j]/3.0 * ( neutr.mpls[K] *  B.an.qdrpls[L][i][j] +  B.an.mpls[L] * neutr.qdrpls[K][i][j] );
			        v1[2] +=  2.0*T2[i][j]/3.0 * ( neutr.mpls[K] * B.neutr.qdrpls[L][i][j] + B.neutr.mpls[L] * neutr.qdrpls[K][i][j] );
			        v1[3] +=  2.0*T2[i][j]/3.0 * ( cat.mpls[K] * B.neutr.qdrpls[L][i][j] + neutr.mpls[L] * cat.qdrpls[K][i][j] );
			        v1[4] +=  2.0*T2[i][j]/3.0 * (  an.mpls[K] * B.neutr.qdrpls[L][i][j] +  B.neutr.mpls[L] * an.qdrpls[K][i][j] );
			    }
			}
		
			//dip dip 
			for(int i=0;i<3;i++)
		        {
		            for (int j=0;j<3;j++)
			    {
				v1[0] -= T2[i][j] * neutr.dpls[K][i] * B.cat.dpls[L][j];
				v1[1] -= T2[i][j] * neutr.dpls[K][i] *  B.an.dpls[L][j];
				v1[2] -= T2[i][j] * neutr.dpls[K][i] * B.neutr.dpls[L][j];
				v1[3] -= T2[i][j] * cat.dpls[K][i] * B.neutr.dpls[L][j];
				v1[4] -= T2[i][j] *  an.dpls[K][i] * B.neutr.dpls[L][j];
			    }
			}
		
			//dip quad
			v1[0] += ( r5 * R[0]*3 + r5 * R[0]*3 + r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[0]*R[0] ) * (-neutr.dpls[K][0] * B.cat.qdrpls[L][0][0] + B.cat.dpls[L][0] *   neutr.qdrpls[K][0][0])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[0]*R[0]*R[1] ) * (-neutr.dpls[K][0] * B.cat.qdrpls[L][0][1] + B.cat.dpls[L][0] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[0]*R[0]*R[2] ) * (-neutr.dpls[K][0] * B.cat.qdrpls[L][0][2] + B.cat.dpls[L][0] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[1]*R[1] ) * (-neutr.dpls[K][0] * B.cat.qdrpls[L][1][1] + B.cat.dpls[L][0] *   neutr.qdrpls[K][1][1])/3.0 + 
(  (-15.0) * r7 *R[0]*R[1]*R[2] ) * (-neutr.dpls[K][0] * B.cat.qdrpls[L][1][2] + B.cat.dpls[L][0] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[2]*R[2] ) * (-neutr.dpls[K][0] * B.cat.qdrpls[L][2][2] + B.cat.dpls[L][0] *   neutr.qdrpls[K][2][2])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[0]*R[0] ) * (-neutr.dpls[K][1] * B.cat.qdrpls[L][0][0] + B.cat.dpls[L][1] *   neutr.qdrpls[K][0][0])/3.0 + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[1]*R[0]*R[1] ) * (-neutr.dpls[K][1] * B.cat.qdrpls[L][0][1] + B.cat.dpls[L][1] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
(  (-15.0) * r7 *R[1]*R[0]*R[2] ) * (-neutr.dpls[K][1] * B.cat.qdrpls[L][0][2] + B.cat.dpls[L][1] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[1]*3 + r5 * R[1]*3 + r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[1]*R[1] ) * (-neutr.dpls[K][1] * B.cat.qdrpls[L][1][1] + B.cat.dpls[L][1] *   neutr.qdrpls[K][1][1])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[1]*R[1]*R[2] ) * (-neutr.dpls[K][1] * B.cat.qdrpls[L][1][2] + B.cat.dpls[L][1] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[2]*R[2] ) * (-neutr.dpls[K][1] * B.cat.qdrpls[L][2][2] + B.cat.dpls[L][1] *   neutr.qdrpls[K][2][2])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[0]*R[0] ) * (-neutr.dpls[K][2] * B.cat.qdrpls[L][0][0] + B.cat.dpls[L][2] *   neutr.qdrpls[K][0][0])/3.0 + 
(  (-15.0) * r7 *R[2]*R[0]*R[1] ) * (-neutr.dpls[K][2] * B.cat.qdrpls[L][0][1] + B.cat.dpls[L][2] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[2]*R[0]*R[2] ) * (-neutr.dpls[K][2] * B.cat.qdrpls[L][0][2] + B.cat.dpls[L][2] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[1]*R[1] ) * (-neutr.dpls[K][2] * B.cat.qdrpls[L][1][1] + B.cat.dpls[L][2] *   neutr.qdrpls[K][1][1])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[2]*R[1]*R[2] ) * (-neutr.dpls[K][2] * B.cat.qdrpls[L][1][2] + B.cat.dpls[L][2] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[2]*3 + r5 * R[2]*3 + r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[2]*R[2] ) * (-neutr.dpls[K][2] * B.cat.qdrpls[L][2][2] + B.cat.dpls[L][2] *   neutr.qdrpls[K][2][2])/3.0	;
			
			v1[1] += ( r5 * R[0]*3 + r5 * R[0]*3 + r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[0]*R[0] ) * (-neutr.dpls[K][0] * B.an.qdrpls[L][0][0] + B.an.dpls[L][0] *   neutr.qdrpls[K][0][0])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[0]*R[0]*R[1] ) * (-neutr.dpls[K][0] * B.an.qdrpls[L][0][1] + B.an.dpls[L][0] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[0]*R[0]*R[2] ) * (-neutr.dpls[K][0] * B.an.qdrpls[L][0][2] + B.an.dpls[L][0] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[1]*R[1] ) * (-neutr.dpls[K][0] * B.an.qdrpls[L][1][1] + B.an.dpls[L][0] *   neutr.qdrpls[K][1][1])/3.0 + 
(  (-15.0) * r7 *R[0]*R[1]*R[2] ) * (-neutr.dpls[K][0] * B.an.qdrpls[L][1][2] + B.an.dpls[L][0] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[2]*R[2] ) * (-neutr.dpls[K][0] * B.an.qdrpls[L][2][2] + B.an.dpls[L][0] *   neutr.qdrpls[K][2][2])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[0]*R[0] ) * (-neutr.dpls[K][1] * B.an.qdrpls[L][0][0] + B.an.dpls[L][1] *   neutr.qdrpls[K][0][0])/3.0 + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[1]*R[0]*R[1] ) * (-neutr.dpls[K][1] * B.an.qdrpls[L][0][1] + B.an.dpls[L][1] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
(  (-15.0) * r7 *R[1]*R[0]*R[2] ) * (-neutr.dpls[K][1] * B.an.qdrpls[L][0][2] + B.an.dpls[L][1] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[1]*3 + r5 * R[1]*3 + r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[1]*R[1] ) * (-neutr.dpls[K][1] * B.an.qdrpls[L][1][1] + B.an.dpls[L][1] *   neutr.qdrpls[K][1][1])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[1]*R[1]*R[2] ) * (-neutr.dpls[K][1] * B.an.qdrpls[L][1][2] + B.an.dpls[L][1] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[2]*R[2] ) * (-neutr.dpls[K][1] * B.an.qdrpls[L][2][2] + B.an.dpls[L][1] *   neutr.qdrpls[K][2][2])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[0]*R[0] ) * (-neutr.dpls[K][2] * B.an.qdrpls[L][0][0] + B.an.dpls[L][2] *   neutr.qdrpls[K][0][0])/3.0 + 
(  (-15.0) * r7 *R[2]*R[0]*R[1] ) * (-neutr.dpls[K][2] * B.an.qdrpls[L][0][1] + B.an.dpls[L][2] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[2]*R[0]*R[2] ) * (-neutr.dpls[K][2] * B.an.qdrpls[L][0][2] + B.an.dpls[L][2] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[1]*R[1] ) * (-neutr.dpls[K][2] * B.an.qdrpls[L][1][1] + B.an.dpls[L][2] *   neutr.qdrpls[K][1][1])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[2]*R[1]*R[2] ) * (-neutr.dpls[K][2] * B.an.qdrpls[L][1][2] + B.an.dpls[L][2] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[2]*3 + r5 * R[2]*3 + r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[2]*R[2] ) * (-neutr.dpls[K][2] * B.an.qdrpls[L][2][2] + B.an.dpls[L][2] *   neutr.qdrpls[K][2][2])/3.0	;
			
			v1[2] += ( r5 * R[0]*3 + r5 * R[0]*3 + r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[0]*R[0] ) * (-neutr.dpls[K][0] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][0] *   neutr.qdrpls[K][0][0])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[0]*R[0]*R[1] ) * (-neutr.dpls[K][0] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][0] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[0]*R[0]*R[2] ) * (-neutr.dpls[K][0] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][0] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[1]*R[1] ) * (-neutr.dpls[K][0] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][0] *   neutr.qdrpls[K][1][1])/3.0 + 
(  (-15.0) * r7 *R[0]*R[1]*R[2] ) * (-neutr.dpls[K][0] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][0] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[2]*R[2] ) * (-neutr.dpls[K][0] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][0] *   neutr.qdrpls[K][2][2])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[0]*R[0] ) * (-neutr.dpls[K][1] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][1] *   neutr.qdrpls[K][0][0])/3.0 + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[1]*R[0]*R[1] ) * (-neutr.dpls[K][1] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][1] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
(  (-15.0) * r7 *R[1]*R[0]*R[2] ) * (-neutr.dpls[K][1] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][1] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[1]*3 + r5 * R[1]*3 + r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[1]*R[1] ) * (-neutr.dpls[K][1] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][1] *   neutr.qdrpls[K][1][1])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[1]*R[1]*R[2] ) * (-neutr.dpls[K][1] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][1] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[2]*R[2] ) * (-neutr.dpls[K][1] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][1] *   neutr.qdrpls[K][2][2])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[0]*R[0] ) * (-neutr.dpls[K][2] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][2] *   neutr.qdrpls[K][0][0])/3.0 + 
(  (-15.0) * r7 *R[2]*R[0]*R[1] ) * (-neutr.dpls[K][2] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][2] *   neutr.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[2]*R[0]*R[2] ) * (-neutr.dpls[K][2] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][2] *   neutr.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[1]*R[1] ) * (-neutr.dpls[K][2] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][2] *   neutr.qdrpls[K][1][1])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[2]*R[1]*R[2] ) * (-neutr.dpls[K][2] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][2] *   neutr.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[2]*3 + r5 * R[2]*3 + r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[2]*R[2] ) * (-neutr.dpls[K][2] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][2] *   neutr.qdrpls[K][2][2])/3.0	;
			
			v1[3] += ( r5 * R[0]*3 + r5 * R[0]*3 + r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[0]*R[0] ) * (-cat.dpls[K][0] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][0] *   cat.qdrpls[K][0][0])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[0]*R[0]*R[1] ) * (-cat.dpls[K][0] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][0] *   cat.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[0]*R[0]*R[2] ) * (-cat.dpls[K][0] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][0] *   cat.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[1]*R[1] ) * (-cat.dpls[K][0] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][0] *   cat.qdrpls[K][1][1])/3.0 + 
(  (-15.0) * r7 *R[0]*R[1]*R[2] ) * (-cat.dpls[K][0] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][0] *   cat.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[2]*R[2] ) * (-cat.dpls[K][0] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][0] *   cat.qdrpls[K][2][2])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[0]*R[0] ) * (-cat.dpls[K][1] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][1] *   cat.qdrpls[K][0][0])/3.0 + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[1]*R[0]*R[1] ) * (-cat.dpls[K][1] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][1] *   cat.qdrpls[K][0][1])/3.0*2.0  + 
(  (-15.0) * r7 *R[1]*R[0]*R[2] ) * (-cat.dpls[K][1] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][1] *   cat.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[1]*3 + r5 * R[1]*3 + r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[1]*R[1] ) * (-cat.dpls[K][1] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][1] *   cat.qdrpls[K][1][1])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[1]*R[1]*R[2] ) * (-cat.dpls[K][1] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][1] *   cat.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[2]*R[2] ) * (-cat.dpls[K][1] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][1] *   cat.qdrpls[K][2][2])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[0]*R[0] ) * (-cat.dpls[K][2] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][2] *   cat.qdrpls[K][0][0])/3.0 + 
(  (-15.0) * r7 *R[2]*R[0]*R[1] ) * (-cat.dpls[K][2] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][2] *   cat.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[2]*R[0]*R[2] ) * (-cat.dpls[K][2] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][2] *   cat.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[1]*R[1] ) * (-cat.dpls[K][2] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][2] *   cat.qdrpls[K][1][1])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[2]*R[1]*R[2] ) * (-cat.dpls[K][2] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][2] *   cat.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[2]*3 + r5 * R[2]*3 + r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[2]*R[2] ) * (-cat.dpls[K][2] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][2] *   cat.qdrpls[K][2][2])/3.0	;
			
			v1[4] += ( r5 * R[0]*3 + r5 * R[0]*3 + r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[0]*R[0] ) * (-an.dpls[K][0] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][0] *   an.qdrpls[K][0][0])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[0]*R[0]*R[1] ) * (-an.dpls[K][0] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][0] *   an.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[0]*R[0]*R[2] ) * (-an.dpls[K][0] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][0] *   an.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[1]*R[1] ) * (-an.dpls[K][0] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][0] *   an.qdrpls[K][1][1])/3.0 + 
(  (-15.0) * r7 *R[0]*R[1]*R[2] ) * (-an.dpls[K][0] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][0] *   an.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[0]*R[2]*R[2] ) * (-an.dpls[K][0] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][0] *   an.qdrpls[K][2][2])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[0]*R[0] ) * (-an.dpls[K][1] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][1] *   an.qdrpls[K][0][0])/3.0 + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[1]*R[0]*R[1] ) * (-an.dpls[K][1] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][1] *   an.qdrpls[K][0][1])/3.0*2.0  + 
(  (-15.0) * r7 *R[1]*R[0]*R[2] ) * (-an.dpls[K][1] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][1] *   an.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[1]*3 + r5 * R[1]*3 + r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[1]*R[1] ) * (-an.dpls[K][1] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][1] *   an.qdrpls[K][1][1])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[1]*R[1]*R[2] ) * (-an.dpls[K][1] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][1] *   an.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[1]*R[2]*R[2] ) * (-an.dpls[K][1] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][1] *   an.qdrpls[K][2][2])/3.0 + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[0]*R[0] ) * (-an.dpls[K][2] * B.neutr.qdrpls[L][0][0] + B.neutr.dpls[L][2] *   an.qdrpls[K][0][0])/3.0 + 
(  (-15.0) * r7 *R[2]*R[0]*R[1] ) * (-an.dpls[K][2] * B.neutr.qdrpls[L][0][1] + B.neutr.dpls[L][2] *   an.qdrpls[K][0][1])/3.0*2.0  + 
( r5 * R[0]*3 +  (-15.0) * r7 *R[2]*R[0]*R[2] ) * (-an.dpls[K][2] * B.neutr.qdrpls[L][0][2] + B.neutr.dpls[L][2] *   an.qdrpls[K][0][2])/3.0*2.0  + 
( r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[1]*R[1] ) * (-an.dpls[K][2] * B.neutr.qdrpls[L][1][1] + B.neutr.dpls[L][2] *   an.qdrpls[K][1][1])/3.0 + 
( r5 * R[1]*3 +  (-15.0) * r7 *R[2]*R[1]*R[2] ) * (-an.dpls[K][2] * B.neutr.qdrpls[L][1][2] + B.neutr.dpls[L][2] *   an.qdrpls[K][1][2])/3.0*2.0  + 
( r5 * R[2]*3 + r5 * R[2]*3 + r5 * R[2]*3 +  (-15.0) * r7 *R[2]*R[2]*R[2] ) * (-an.dpls[K][2] * B.neutr.qdrpls[L][2][2] + B.neutr.dpls[L][2] *   an.qdrpls[K][2][2])/3.0	;

			//quad quad
			v1[0] +=  ( r5 * 9 + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[0]) * neutr.qdrpls[K][0][0]*B.cat.qdrpls[L][0][0]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[1]) * neutr.qdrpls[K][0][0]*B.cat.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[2]) * neutr.qdrpls[K][0][0]*B.cat.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[1]) * neutr.qdrpls[K][0][0]*B.cat.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[2]) * neutr.qdrpls[K][0][0]*B.cat.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[0]*R[2] * R[2]) * neutr.qdrpls[K][0][0]*B.cat.qdrpls[L][2][2]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[0]) * neutr.qdrpls[K][0][1]*B.cat.qdrpls[L][0][0]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[1]) * neutr.qdrpls[K][0][1]*B.cat.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[2]) * neutr.qdrpls[K][0][1]*B.cat.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[1]) * neutr.qdrpls[K][0][1]*B.cat.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[2]) * neutr.qdrpls[K][0][1]*B.cat.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[2] * R[2]) * neutr.qdrpls[K][0][1]*B.cat.qdrpls[L][2][2]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][0][2]*B.cat.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][0][2]*B.cat.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][0][2]*B.cat.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][0][2]*B.cat.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][0][2]*B.cat.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][0][2]*B.cat.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[0]) * neutr.qdrpls[K][1][1]*B.cat.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[1]) * neutr.qdrpls[K][1][1]*B.cat.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[2]) * neutr.qdrpls[K][1][1]*B.cat.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[1]) * neutr.qdrpls[K][1][1]*B.cat.qdrpls[L][1][1]/9.0  + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[2]) * neutr.qdrpls[K][1][1]*B.cat.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[1]*R[2] * R[2]) * neutr.qdrpls[K][1][1]*B.cat.qdrpls[L][2][2]/9.0  + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][1][2]*B.cat.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][1][2]*B.cat.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][1][2]*B.cat.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][1][2]*B.cat.qdrpls[L][1][1]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][1][2]*B.cat.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][1][2]*B.cat.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][2][2]*B.cat.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][2][2]*B.cat.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][2][2]*B.cat.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][2][2]*B.cat.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][2][2]*B.cat.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[2]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][2][2]*B.cat.qdrpls[L][2][2]/9.0;

			v1[1] +=  ( r5 * 9 + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[0]) * neutr.qdrpls[K][0][0]*B.an.qdrpls[L][0][0]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[1]) * neutr.qdrpls[K][0][0]*B.an.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[2]) * neutr.qdrpls[K][0][0]*B.an.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[1]) * neutr.qdrpls[K][0][0]*B.an.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[2]) * neutr.qdrpls[K][0][0]*B.an.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[0]*R[2] * R[2]) * neutr.qdrpls[K][0][0]*B.an.qdrpls[L][2][2]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[0]) * neutr.qdrpls[K][0][1]*B.an.qdrpls[L][0][0]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[1]) * neutr.qdrpls[K][0][1]*B.an.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[2]) * neutr.qdrpls[K][0][1]*B.an.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[1]) * neutr.qdrpls[K][0][1]*B.an.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[2]) * neutr.qdrpls[K][0][1]*B.an.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[2] * R[2]) * neutr.qdrpls[K][0][1]*B.an.qdrpls[L][2][2]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][0][2]*B.an.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][0][2]*B.an.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][0][2]*B.an.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][0][2]*B.an.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][0][2]*B.an.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][0][2]*B.an.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[0]) * neutr.qdrpls[K][1][1]*B.an.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[1]) * neutr.qdrpls[K][1][1]*B.an.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[2]) * neutr.qdrpls[K][1][1]*B.an.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[1]) * neutr.qdrpls[K][1][1]*B.an.qdrpls[L][1][1]/9.0  + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[2]) * neutr.qdrpls[K][1][1]*B.an.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[1]*R[2] * R[2]) * neutr.qdrpls[K][1][1]*B.an.qdrpls[L][2][2]/9.0  + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][1][2]*B.an.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][1][2]*B.an.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][1][2]*B.an.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][1][2]*B.an.qdrpls[L][1][1]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][1][2]*B.an.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][1][2]*B.an.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][2][2]*B.an.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][2][2]*B.an.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][2][2]*B.an.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][2][2]*B.an.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][2][2]*B.an.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[2]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][2][2]*B.an.qdrpls[L][2][2]/9.0;

			v1[2] +=  ( r5 * 9 + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[0]) * neutr.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[1]) * neutr.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[2]) * neutr.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[1]) * neutr.qdrpls[K][0][0]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[2]) * neutr.qdrpls[K][0][0]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[0]*R[2] * R[2]) * neutr.qdrpls[K][0][0]*B.neutr.qdrpls[L][2][2]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[0]) * neutr.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[1]) * neutr.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[2]) * neutr.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[1]) * neutr.qdrpls[K][0][1]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[2]) * neutr.qdrpls[K][0][1]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[2] * R[2]) * neutr.qdrpls[K][0][1]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][0][2]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][0][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][0][2]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[0]) * neutr.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[1]) * neutr.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[2]) * neutr.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[1]) * neutr.qdrpls[K][1][1]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[2]) * neutr.qdrpls[K][1][1]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[1]*R[2] * R[2]) * neutr.qdrpls[K][1][1]*B.neutr.qdrpls[L][2][2]/9.0  + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][1][2]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][1][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][1][2]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[0]) * neutr.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[1]) * neutr.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[2]) * neutr.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[1]) * neutr.qdrpls[K][2][2]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[2]) * neutr.qdrpls[K][2][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[2]*R[2]*R[2] * R[2]) * neutr.qdrpls[K][2][2]*B.neutr.qdrpls[L][2][2]/9.0;

			v1[3] +=  ( r5 * 9 + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[0]) * cat.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[1]) * cat.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[2]) * cat.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[1]) * cat.qdrpls[K][0][0]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[2]) * cat.qdrpls[K][0][0]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[0]*R[2] * R[2]) * cat.qdrpls[K][0][0]*B.neutr.qdrpls[L][2][2]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[0]) * cat.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[1]) * cat.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[2]) * cat.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[1]) * cat.qdrpls[K][0][1]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[2]) * cat.qdrpls[K][0][1]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[2] * R[2]) * cat.qdrpls[K][0][1]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[0]) * cat.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[1]) * cat.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[2]) * cat.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[1]) * cat.qdrpls[K][0][2]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[2]) * cat.qdrpls[K][0][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[2] * R[2]) * cat.qdrpls[K][0][2]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[0]) * cat.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[1]) * cat.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[2]) * cat.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[1]) * cat.qdrpls[K][1][1]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[2]) * cat.qdrpls[K][1][1]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[1]*R[2] * R[2]) * cat.qdrpls[K][1][1]*B.neutr.qdrpls[L][2][2]/9.0  + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[0]) * cat.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[1]) * cat.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[2]) * cat.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[1]) * cat.qdrpls[K][1][2]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[2]) * cat.qdrpls[K][1][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[2] * R[2]) * cat.qdrpls[K][1][2]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[0]) * cat.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[1]) * cat.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[2]) * cat.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[1]) * cat.qdrpls[K][2][2]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[2]) * cat.qdrpls[K][2][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[2]*R[2]*R[2] * R[2]) * cat.qdrpls[K][2][2]*B.neutr.qdrpls[L][2][2]/9.0;
			
			v1[4] +=  ( r5 * 9 + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[0]) * an.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[1]) * an.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[0]*R[0] * R[2]) * an.qdrpls[K][0][0]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[1]) * an.qdrpls[K][0][0]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[0]*R[1] * R[2]) * an.qdrpls[K][0][0]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[0]*R[2] * R[2]) * an.qdrpls[K][0][0]*B.neutr.qdrpls[L][2][2]/9.0  + 
( r7 * R[0]*R[1] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[0]) * an.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[1]) * an.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[0] * R[2]) * an.qdrpls[K][0][1]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) + r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[1]) * an.qdrpls[K][0][1]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[1]*R[1] * R[2]) * an.qdrpls[K][0][1]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[1]*R[2] * R[2]) * an.qdrpls[K][0][1]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[0]) * an.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[1]) * an.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r5 * 3 + r7 * R[0]*R[0] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[0] * R[2]) * an.qdrpls[K][0][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[1]) * an.qdrpls[K][0][2]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r7 * R[0]*R[1] * (-15) +  105 * r9 * R[0]*R[2]*R[1] * R[2]) * an.qdrpls[K][0][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) + r7 * R[0]*R[2] * (-15) +  105 * r9 * R[0]*R[2]*R[2] * R[2]) * an.qdrpls[K][0][2]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[0]) * an.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) + r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[1]) * an.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[1]*R[0] * R[2]) * an.qdrpls[K][1][1]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[1]) * an.qdrpls[K][1][1]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[1]*R[1] * R[2]) * an.qdrpls[K][1][1]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[1]*R[2] * R[2]) * an.qdrpls[K][1][1]*B.neutr.qdrpls[L][2][2]/9.0  + 
( r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[0]) * an.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][0]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[1]) * an.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[1]*R[2]*R[0] * R[2]) * an.qdrpls[K][1][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[1]) * an.qdrpls[K][1][2]*B.neutr.qdrpls[L][1][1]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[1]*R[1] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[1] * R[2]) * an.qdrpls[K][1][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0  * 2.0   + 
( r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) + r7 * R[1]*R[2] * (-15) +  105 * r9 * R[1]*R[2]*R[2] * R[2]) * an.qdrpls[K][1][2]*B.neutr.qdrpls[L][2][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[0]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[0]) * an.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][0]/9.0  + 
( r7 * R[1]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[1]) * an.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][1]/9.0 * 2.0   + 
( r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) + r7 * R[2]*R[0] * (-15) +  105 * r9 * R[2]*R[2]*R[0] * R[2]) * an.qdrpls[K][2][2]*B.neutr.qdrpls[L][0][2]/9.0 * 2.0   + 
( r5 * 3 + r7 * R[2]*R[2] * (-15) + r7 * R[1]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[1]) * an.qdrpls[K][2][2]*B.neutr.qdrpls[L][1][1]/9.0  + 
( r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) + r7 * R[2]*R[1] * (-15) +  105 * r9 * R[2]*R[2]*R[1] * R[2]) * an.qdrpls[K][2][2]*B.neutr.qdrpls[L][1][2]/9.0 * 2.0   + 
( r5 * 9 + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) + r7 * R[2]*R[2] * (-15) +  105 * r9 * R[2]*R[2]*R[2] * R[2]) * an.qdrpls[K][2][2]*B.neutr.qdrpls[L][2][2]/9.0;
		
		}
	}
		

} 

double mol_and_orb::exp_6(  mol_and_orb const & two ){
	static const double A = 0.214 * 0.04336 * 0.03675119441381844909; // the second number is the conversion bw eV and kcal/mol
	static const double C = 47000.0 *  0.04336 * 0.03675119441381844909; // to which i will diligently add the conversion factor forH to eV
	static const double Alpha = 12.35;
	
	static const double A_1 = A/5.0;
	static const double C_1 = C/2.7;
	static const double Alpha_1 = 13.8; // these parameters are taken from CAillet '75
	static const double R_VDW[10]={1.06/RA, 0.0/RA, 0.0/RA, 0.0/RA, 0.0/RA, 1.53/RA, 1.46/RA, 1.42/RA, 0.0/RA, 0.0}; // Bondi 65
	static const double K_VDW[10]={ 1.0, 0.0, 0.0, 0.0, 0.0,  1.0, 1.18, 1.36, 0.0, 0.0}; // Caillet 75
	static const double VAL[10]  ={ 1.0, 0.0, 0.0, 0.0, 0.0,  6.0,  7.0 , 8.0, 0.0, 0.0};

	static const double nrg_hard_ball=1E300;
	static const double rad_balls=1.0/RA;
	
	double a, c, alpha;
	
	double nrg=0.0;
	double dist;
	double corr_i;
	double R_0, R;
	coord dm;
	int i,j;
	for (i=0;i<N;i++){
	    int I=lbl[i];
	    corr_i = (1.0 - neutr.mpls[i] / VAL[I]);
	    for (j=0;j<two.N;j++){
		int J = two.lbl[j];
	       	dm = pos[i]-two.pos[j];
		dist =dm.mod();
		if ( dist < rad_balls) return nrg_hard_ball;
		if ( I == 0 || J == 0){ //set parameters for Hidrogen bonds
		    if ( dist < 1.8/RA ){
			a = A;
			c = C;
			alpha = Alpha;	
		    }
		    else if (dist > 2.6/RA ) {
			a = A_1;
			c = C_1;
			alpha = Alpha_1;
		    } 
		    else {
		       double x  = (dist-2.2/RA)/0.4;
		       double f = ( 0.375 *x*x*x*x*x -1.25 *x*x*x + 1.875 * x );
		       alpha = (Alpha + Alpha_1) / 2.0 + (Alpha - Alpha_1) /2.0 * f ;
		       c = ( C + C_1 ) /2.0 + ( C - C_1 ) /2.0 *f;
		       a = ( A + A_1 ) /2.0 + ( A - A_1 ) /2.0 *f;
			
		    }
		    
		}
		else { 
		    alpha=Alpha;
		    c=C;
		    a=A;
		}
		R_0= sqrt(4.0 * R_VDW[ I ] * R_VDW[ J ] );
		R = dist/R_0;
	    
		c = c* corr_i * (1 - two.neutr.mpls[j] / VAL[ J] );
		double exp_rep =  K_VDW[I]* K_VDW[J] * c * exp ( -alpha * R ) ;
		double disp =  - K_VDW[I]* K_VDW[J] * a /(R*R*R*R*R*R);
		double de = exp_rep + disp;
		nrg += de;  
	    }
	}
	return nrg;
}


double mol_and_orb::find_rmin_binary( mol_and_orb & A , double th, double  phi, double d_min, double d_min_mx, double d_max){
    //takes in a molecule in a give orientation and displace is along a certain theta and phi untill dmin<> particular values`
    int count=0; // count the number of cycles it is taking us
    const int countmax=1000;
    static mol_and_orb blue_print;
    static bool first_time=true;
    if (first_time == true ){ 
	blue_print = A;
	first_time = false;
    }
    coord displ;
    coord tmp;
    double d;
    double d_cl =0.0;
    double dmx, dmn,d_tmp;
    dmx = d_max;
    dmn = d_min;
    blue_print.cp_pos(*this); // copy the orientation of the second mol
    while ( d_cl < d_min || d_cl > d_min_mx ){
	d = (dmx - dmn )/2.0 + dmn ;
	displ.set('P', d, th, phi);
	shift(displ);
	d_cl=1E+06;
	for (int i=0;i< A.N;i++) {
	    for (int j=0;j<N;j++) {
		tmp = pos[j] - A.pos[i];
		d_tmp = tmp.mod();
		if ( d_tmp < d_cl) d_cl = d_tmp;
	    }
	}
	if (d_cl > d_min_mx ) dmx = d;
	else if ( d_cl < d_min) dmn = d;
	
	count ++;
	if (count > countmax)  {  cp_pos(blue_print); return 0.0;}
	cp_pos(blue_print);
    }
    return d;
}

rot get_eulerrot(coord & a, coord & a_, coord & b, coord  & b_){
    // want to obtain the Euler rotation which brings a to b and a_ to b_
	
    const bool debug = false;
    coord u1=a;
    coord u1_=a_;
    coord u1_norm;
    coord u2=b;
    coord u2_=b_;	 	
    coord u2_norm;
	
    //first make sure that all coordinates are unit matrices (should sort out problem with possibly different legths)//
    double l;
    l = u1.mod();
    u1 = u1 / l;

    l = u2.mod();
    u2 = u2 / l;

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /////in order to get the position of the z and x axis before/after the rotation I want to have three
    //   orthogonal, unit vectors!
    u1_norm = u1.cross( u1_);
    l = u1_norm.mod();
    u1_norm = u1_norm / l;
    u1_ = u1_norm.cross(u1);
    l =  u1_.mod();
    u1_ = u1_ / l;
	
    
    u2_norm = u2.cross( u2_);
    l = u2_norm.mod();
    u2_norm = u2_norm/l;
    u2_ = u2_norm.cross(u2);
    l =  u2_.mod();
    u2_ = u2_ / l;
	
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///then I will need to decompose the "z" axis and "x" axis in in terms of these three vectors.//////////////////////
   double ax, bx,cx, az,bz,cz; //these refer to the three components (u1, u1_m u1_norm) of the x and z vector resp 
   /// because u1 and u1_ are not necesseraly normal, so then : 
	
   ax = (u1.x ) ; 
   bx = (u1_.x) ;
   cx = u1_norm.x;

   az = (u1.z  ) ; 
   bz = (u1_.z ) ;
   cz = u1_norm.z;

   coord x_2, z_2, tmp;

   x_2 = u2 * ax ;
   tmp = ( u2_ * bx );
   x_2 = x_2 + tmp; 
   tmp = ( u2_norm * cx );
   x_2 = x_2 + tmp;

   z_2 = u2 * az ;
   tmp = u2_ *bz ;
   z_2 = z_2 + tmp;
   tmp = u2_norm * cz;
   z_2 = z_2 +tmp;

    //////////////////////////Obtaining the elements of the matrix//////////////////////////////////////////////////////
    //// we will use Euler rotations in the x convention///////////////////////////////////////////////////////////////*
    /* the form of the unitary transfromation is: 
cg1[1][1]	=	cospsi * cosphi-costheta * sinphi * sinpsi	
cg1[1][2]	=	cospsi * sinphi+costheta * cosphi * sinpsi	
cg1[1][3]	=	sinpsi * sintheta	
cg1[2][1]	=	-sinpsi* cosphi-costheta* sinphi * cospsi	
cg1[2][2]	=	-sinpsi * sinphi+costheta * cosphicospsi	
cg1[2][3]	=	cospsi * sintheta	
cg1[3][1]	=	sintheta * sinphi	
cg1[3][2]	=	-sintheta * cosphi	
cg1[3][3]	=	costheta	
	first we will obtain the first two angles, phi and theta, from the 1st vector u1 ///////////////////
     */	
	
    	double cosphi,  costheta, cospsi; // the three euler angles
	double sinphi, sintheta, sinpsi;

	//obtain costheta
	costheta = z_2.z;
 	sintheta = sqrt ( 1- costheta *costheta);   
	//obtain cosphi
	const double toll = .999999999999999 ;
//	const double toll = 1.0;
	if ( costheta <=  toll && costheta >= -toll )
	{    
		cosphi = z_2.y / sintheta; //sintheta is always positive (theta can only be netween 0 and pi)
		sinphi = z_2.x / sintheta;

		sinpsi = x_2.z / sintheta;
                if (cosphi <= toll &&  cosphi  >= -toll ) {
                        sinpsi = x_2.z / sintheta;
                        cospsi = -(x_2.y + cosphi * costheta *sinpsi)/sinphi;
                }
                else if (cosphi >= toll ) {
                    sinpsi = 0.0;
                    cospsi =1.0;
                    cosphi =1.0;
                    sinphi=0.0;
                }
                else {
                    sinpsi = 0.0;
                    cospsi = 1.0;
                    cosphi = -1.0;
                    sinphi = 0.0;
                }

	}
	
	else if( costheta >= toll) {
	    // we are in the case here where theta && phi have been set to 0, so the position of the X axis will define the
	    // rotation matrix according to the formula:
	    // cos(psi) sin(psi) 0
	    // -sin(psi) cos(psi) 0   
	    // 0           0      1     //mash up the gimbal lock
	    cosphi = 1.0;
	    sinphi = 0.0;
	    sintheta = 0.0;
	    costheta =1.0;
	    cospsi = x_2.x;
	    sinpsi = x_2.y;	
	}

	else if ( costheta <= -toll) {
	    cosphi=1.0;
	    sinphi=0.0;
	    sintheta =0.0;
	    costheta =-1.0;
	    cospsi = x_2.x;
	    sinpsi = x_2.y;
	}
		
	
	double cg1 [3][3];
	cg1[0][0]= cosphi * cospsi - sinphi * costheta * sinpsi;
	cg1[0][1]= cosphi * sinpsi + sinphi * costheta * cospsi;
	cg1[0][2]= sinphi * sintheta 	;
	cg1[1][0]= -sinphi * cospsi - cosphi * costheta *sinpsi ;
	cg1[1][1]= -sinphi * sinpsi + cosphi * costheta * cospsi ;
	cg1[1][2]= cosphi * sintheta ;
	cg1[2][0]= sintheta * sinpsi ;
	cg1[2][1]= -sintheta * cospsi ;
	cg1[2][2]=  costheta     ;
	
	rot res(cg1);

	if (debug==true){
	    coord tmp1 ( 0.0,0.0,1.0);
	    tmp1.rotate(res);
	    coord tmp2  (1.0,0.0,0.0);
	    tmp2.rotate(res);
	    tmp1 = tmp1-z_2;
	    tmp2 = tmp2-x_2;
	    cerr << "Difference in modulus: " << tmp1.mod() << " and " << tmp2.mod()<< endl ;
	}
	return res;
}

