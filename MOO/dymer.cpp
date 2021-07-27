#include <iostream>
#include <sstream>
#include <vector>
#include "mol_and_orb/molecules_and_orbitals.h"
#include "RandomB/RandomB.h"

using namespace std;



int main(int argc, char * argv[])
{
        cout << " Usage: n pairs of orbitals that you want to calculate J for (smaller than 100pls); pairs of orbitals, n geom fils" << endl;
	mol_and_orb A;
	mol_and_orb B;
	mol_and_orb AB; // this one will have two molecules, only coordinates
	mol_and_orb templ;
	
	char * name_geom="INPUT_COORDS";
	char * nameorbs="fort.7";

	int check = A.init_nocharge( name_geom, nameorbs);
	if (check != 0) {cout << "Error in read " << endl; return 1;}
	check = B.init_nocharge( name_geom, nameorbs);
	if (check != 0) {cout << "Error in read "<< endl; return 1;}
	check = templ.init_nocharge(name_geom, nameorbs);        
	if (check != 0) {cout << "Error in read " << endl; return 1;}

//	sscanf(argv[1], "%i", &homo);
	int npairs;
	sscanf(argv[1], "%i", &npairs);
	vector <int*> input;
	int comb1[100][2];
	for (int i =0;i <npairs;i++){
	    int pair_one, pair_two;
	    sscanf(argv[2*i+2], "%i", &pair_one);
	    sscanf(argv[2*i+3], "%i", &pair_two);
            comb1[i][0]=pair_one;
	    comb1[i][1]=pair_two;
	    input.push_back(comb1[i]);
	}
	cout << "J1 " <<endl;
	cout.setf(ios::scientific);
	
	vector <double> results;
	for (int i = npairs*2+2; i < argc; i++){
		check = AB.init_nocharge_noorb(argv[i]);

		results = AB.calcJ( A, B, templ, input);
		cout << argv[i] << '\t' ;  // J1
		for (int j =0 ; j< npairs;j++) cout << results[j] << '\t';
		cout <<endl;
		
	}
	return 0;
}
