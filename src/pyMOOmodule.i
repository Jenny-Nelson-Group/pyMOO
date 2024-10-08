 /* pyMOOmodule.i */
 %module pyMOO
 %{
 /* Put header files here or function declarations like below */
 extern double transferint(double rx, double ry,double rz, double r0, double r1,double r2,double r3,double r4,double r5,double r6,double r7,double r8);
 extern int readmols();
 %}

 extern double transferint(double rx, double ry,double rz, double r0, double r1,double r2,double r3,double r4,double r5,double r6,double r7,double r8);
 extern int readmols();
