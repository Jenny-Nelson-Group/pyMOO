// ABJect: Calculate J between molecules A and B
// Inputs: structure A, orbitals A, structure B, orbitals B, structure AB
// Jack F. Coker 06/01/2023

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "mol_and_orb/molecules_and_orbitals.h"
#include "RandomB/RandomB.h"

using namespace std;

int main(int argc, char * argv[])
{
	//cout << "structure A, orbitals A, structure B, orbitals B, structure AB" << endl;
	mol_and_orb A;
	mol_and_orb B;
	mol_and_orb AB;
	mol_and_orb templ;

	char * name_geom_A=argv[1];
	char * name_orbs_A=argv[2];
	char * name_geom_B=argv[3];
	char * name_orbs_B=argv[4];
	char * name_geom_AB=argv[5];

	int checkA = A.init_nocharge( name_geom_A, name_orbs_A);
	if (checkA != 0) {cout << "Error in read " << name_geom_A << " OR " << name_orbs_A << endl; return 1;}
	int checkB = B.init_nocharge( name_geom_B, name_orbs_B);
	if (checkB != 0) {cout << "Error in read " << name_geom_B << " OR " << name_orbs_B << endl; return 1;}
	int checkAB = AB.init_nocharge_noorb( name_geom_AB ); // Only coordinates, not orbitals
	int checkTempl = templ.init_nocharge( name_geom_A, name_orbs_A); // Same as A

	// homo and lumo levels of mols A and B
	int homoA = A.n_el / 2 -1;
	int lumoA = A.n_el / 2;
	int homoB = B.n_el / 2 -1;
	int lumoB = B.n_el / 2;

	vector< int* > input;
	int hh[2] = {homoA, homoB};
	int ll[2] = {lumoA, lumoB};
	input.push_back(hh);
	input.push_back(ll);
	
	vector <double> results;
	results = AB.calcJ( A, B, templ, input);

	cout << results[0] << " " << results[1] << endl;
	return 0;
}
