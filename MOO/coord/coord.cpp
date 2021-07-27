
	#include "./coord.h"

	
	coord::coord(const char &t, const double &X, const double &Y, const double &Z)
	{
		if (t != 'C' && t != 'P')
			cout << "error what type of co-rd would you like?" << endl;
		if(t == 'C')
		{
			x = X; 
			y = Y;
			z= Z;
		}
		if(t=='P')
		{
			x = X * cos(Y) * cos(Z);
			y = X * cos(Y) * sin(Z);
			z = X * sin(Y); 
		}
    }
    
	
	void coord::set(const char &t,const double & X, const double & Y,const double & Z)
	{
	
		if (t != 'C' && t != 'P')
			cout << "Error what type of co-rd would you like?" << endl;
		
		if(t == 'C')
		{
			x = X; 
			y = Y;
			z= Z;
		}
		if(t=='P')
		{
			if(X<0)
		    	cout << "!!!"<<endl;
			x = X * cos(Y) * cos(Z);
			y = X * cos(Y) * sin(Z);
			z = X * sin(Y); 
		}
    }
	
    double coord::getx()
	{
		return x;
	}

	double coord::gety()
	{
		return y;		
	}

	double coord::getz()
	{
		return z;
	}
	
	double coord::getph()
	{
		double ans;
		if(x!=0)
			ans = atan(y/x);
		else
		{
			if(y==0)
				ans = 0;
			else if(y>0)
				ans = PI/2;
			else 
				ans = -PI/2;
		}

		if(x<0)
			return PI+ans;
		else
			return ans;
	}

	double coord::getth()
	{
		double b = x*x+y*y;
		double ans;
		if(b!=0)
			ans = atan(z/sqrt(x*x+y*y));
		else if(z==0)
			ans = 0;
		else
		{
			if(z>0)
				ans = PI/2;
			else
				ans = -PI/2;
		}
		if(ans>PI/2 || ans <-PI/2)
			cout << "Error with gettheta!!!"<<endl;
		return ans;
	}

	void coord::print() 
	{
		cout << "(" << x << ", " 
					<< y << ", " 
					<< z << ")" << endl;
	}
	
	void coord::print(ofstream &out) 
	{
		out <<  x << '\t'
					<< y << '\t'
					<< z << '\t' << endl;
	}

	void coord::Rot(const double &phi,const double &theta,const double &psi) // euler rotations by theta and phy about the x axis and the z axis.
	{
		double x_t, y_t, z_t;
		double a1, a2,a3,a4,a5,a6,a7,a8,a9;
		a1 = cos(phi)*cos(psi)- cos(theta)*sin(phi)*sin(psi);
		a2 = cos(psi)*sin(phi)+cos(theta)*cos(phi)*sin(psi);
		a3 = sin(psi)*sin(theta);
		a4 = -sin(psi)*cos(phi)-cos(theta)*sin(phi)*cos(psi);
		a5 = -sin(psi)*sin(phi)+cos(theta)*cos(phi)*cos(psi);
		a6 = cos(psi)*sin(theta);
		a7 = sin(theta)*sin(phi);
		a8 = -sin(theta)*cos(phi);
		a9 = cos(theta);

		x_t = a1*x+a2*y+a3*z;
		y_t = a4*x+a5*y+a6*z;
		z_t =  a7*x+a8*y+a9*z;
		x = x_t;
		y = y_t;
		z = z_t; 
	}
	
	void coord::rotlean(double &cosphi, double &sinphi, double &costheta, double &sintheta, double &cospsi,double &sinpsi)
	{
        double x_t, y_t, z_t;
	double a1, a2,a3,a4,a5,a6,a7,a8,a9;
		
        a1 = cosphi*cospsi- costheta*sinphi*sinpsi;
	a2 = cospsi*sinphi+costheta*cosphi*sinpsi;
	a3 = sinpsi*sintheta;
	a4 = -sinpsi*cosphi-costheta*sinphi*cospsi;
	a5 = -sinpsi*sinphi+costheta*cosphi*cospsi;
	a6 = cospsi*sintheta;
	a7 = sintheta*sinphi;
	a8 = -sintheta*cosphi;
	a9 = costheta;

	x_t = a1*x+a2*y+a3*z;
	y_t = a4*x+a5*y+a6*z;
	z_t =  a7*x+a8*y+a9*z;
	x = x_t;
	y = y_t;
	z = z_t; 
	}
	
	void coord::xrot(const double &xtheta)
	{
		double x_t,y_t,z_t;
		double Cos = cos(xtheta);
		double Sin = sin(xtheta);
		x_t = x;
		y_t = Cos * y + Sin *z;
		z_t = Cos * z - Sin * y;
		x = x_t;
		y = y_t;
		z = z_t;
	}

	void coord::yrot(const double &ytheta)
	{
		double x_t,y_t,z_t;
		y_t = y;
		double Cos = cos(ytheta);
		double Sin = sin(ytheta);
		x_t = Cos * x - Sin*z;
		z_t = Cos * z + Sin*x;
		x = x_t;
		y = y_t;
		z = z_t;
	}
	
	void coord::zrot(const double &ztheta)
	{
		double x_t,y_t,z_t;
		z_t = z;
		x_t = cos(ztheta) * x + sin(ztheta) *y;
		y_t = cos(ztheta) * y - sin(ztheta) *x;
		x = x_t;
		y = y_t;
		z = z_t;
	}

	void coord::xrot_l(const double &Cos, const double &Sin)
	{
		double y_t,z_t;
		y_t=Cos*y+Sin*z;
		z_t=Cos*z-Sin*y;
		y=y_t;
		z=z_t;
	}
	
	void coord::yrot_l(const double &Cos, const double &Sin)
        {
                double z_t,x_t;
                z_t=Cos*z+Sin*x;
                x_t=Cos*x-Sin*z;
                z=z_t;
                x=x_t;
        }
	
	void coord::zrot_l(const double &Cos, const double &Sin)
        {
                double x_t,y_t;
                x_t=Cos*x+Sin*y;
                y_t=Cos*y-Sin*x;
                x=x_t;
                y=y_t;
        }

	void coord::add( coord &a){
		x  = x  + a.x;
		y  = y  + a.y;
		z  = z  + a.z;
	}
 	
	ostream & operator<<(ostream & out , coord a)
	{
         return out << a.getx() << '\t' << a.gety() << '\t' << a.getz() <<endl;       	  
	}
	



void coord::rot_about_axis(double const &alpha, double const &phi, double const &theta)
{
    double a,b,c,d; // these are the quaternion for the roation (as from wikipedia)
    a=cos(alpha/2);
    double s= sin(alpha/2);
    double c_p=cos(phi);
    double s_p=sin(phi);
    double c_t=cos(theta);
    double s_t=sin(theta);
    b = s*c_p*c_t;
    c = s*s_p*c_t;
    d = s*s_t;
    double x_t,y_t,z_t;
    x_t = x * ( a*a + b*b - c*c - d*d ) + y * 2.0 * ( b*c - a*d ) + z * 2.0 * ( a*c + b*d ) ;
    y_t = x * 2.0 * ( a*d + b*c ) + y * ( a*a - b*b + c*c - d*d ) + z * 2.0 * ( c*d - a*b ) ;
    z_t = x * 2.0 * ( b*d - a*c ) + y * 2.0 * ( a*b + c*d ) + z * ( a*a - b*b - c*c + d*d ) ;
    x = x_t;
    y = y_t;
    z = z_t;
}
