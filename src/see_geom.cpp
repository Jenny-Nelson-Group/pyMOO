#include <iostream>
#include <sstream>
#include "molecules_and_orbitals.h"
#include "RandomB.h"

using namespace std;



int main(int argc, char * argv[])
{
	mol_and_orb still;
	mol_and_orb mov;
	if (argc != 7 ) {
	   cout << "arguments: x y z theta phi psi " << endl;
	  return 0;
	} 
	
	char * name_geom="INPUT_COORDS";
	char * nameorbs="fort.7";

	int check = still.init_nocharge( name_geom, nameorbs);
	if (check != 0) {cout << "Error in read charge " << endl; return 1;}
	check = mov.init_nocharge( name_geom, nameorbs);
	if (check != 0) {cout << "Error in read charge " << endl; return 1;}
	
	double displ[3] = {0.0, 0.0, 6.0/RA};

	double Psi, Theta, Phi; // these are the geometrical factors
	double E_vdw;
	sscanf(argv[1], "%lf", &displ[0]);
	sscanf(argv[2], "%lf", &displ[1]);
	sscanf(argv[3], "%lf", &displ[2]);
	sscanf(argv[4], "%lf", &Phi);
        sscanf(argv[5], "%lf", &Theta);
        sscanf(argv[6], "%lf", &Psi);
	
	coord vect(displ[0], displ[1], displ[2]);
	rot a(Phi, Theta, Psi);	
	
	mov.rotate(a);
	mov.shift(vect);

	E_vdw = mov.exp_6(still);
	cout << "#" << E_vdw;	

	still.print();
	mov.print();
	return 0;
}
