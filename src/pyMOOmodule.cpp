#include <iostream>
#include "mol_and_orb/molecules_and_orbitals.h"
#include <fstream>
#include <string>

using namespace std;
//wrapper written with Denis to do MOO only on a file which give the unitary transformation + displacement one per line
void  parce_string (string line, string delims, vector<string>* result ) {
  string::size_type begIdx, endIdx;

  begIdx = line.find_first_not_of(delims);
  while (begIdx != string::npos) {
    endIdx = line.find_first_of (delims, begIdx);
    if (endIdx == string::npos) {
      endIdx = line.length();
    }
    result->push_back( line.substr(begIdx, endIdx-begIdx) );
    if (endIdx == line.length()) { break; cout << "I am still here";}
    begIdx = line.find_first_not_of (delims, endIdx);
  }
}



// definition of two molecules 
mol_and_orb A;
mol_and_orb B;

int readmols()
{        
        // initialization (reading the data from the fort.7 file)
	char namegeom[]="INPUT_COORDS";
	char nameorbs[]="fort.7";
	int checkA = A.init_nocharge(namegeom, nameorbs);
	int checkB = B.init_nocharge(namegeom, nameorbs);
 return 0;
}


double transferint(double rx, double ry,double rz, double r0, double r1,double r2,double r3,double r4,double r5,double r6,double r7,double r8)
{
	vector<double> overlap_integral;

	// homo level of the 
	int homo = A.n_el / 2 -1;

	vector< int* > input;
	int comb1[2] = {homo, homo};
//	int comb4[2] = {homo-1, homo-1};

	input.push_back(comb1);
//	input.push_back(comb4);

	// initialize the rotation matrix and translation vector
	double displ[3] = {10.0, 0.0, 0.0};
	double rotation[3][3]= {{1.0 , 0.0, 0.0  }, { 0.0, 1.0, 0.0 }, {0.0, 0.0, 1.0} }; 

        // reading a text file

        using namespace std;

  
    displ[0]=rx; displ[1]=ry; displ[2]=rz;
    rotation[0][0] = r0;
	rotation[0][1] = r1;
	rotation[0][2] = r2;
	rotation[1][0] = r3;
	rotation[1][1] = r4;
	rotation[1][2] = r5;
	rotation[2][0] = r6;
	rotation[2][1] = r7;
	rotation[2][2] = r8;

	char nameout[] = "tmp.out"; //put geoms in this file
	
    cout << "OK, ready to calculate J in transferint()" << endl;
    
    cout << "displ[0] = " << displ[0] << endl;
    cout << "displ[1] = " << displ[1] << endl;
    cout << "displ[2] = " << displ[2] << endl;
    cout << "rotation[0][0] = " << rotation[0][0] << endl;
    cout << "rotation[0][1] = " << rotation[0][1] << endl;
    cout << "rotation[0][2] = " << rotation[0][2] << endl;
    cout << "rotation[1][0] = " << rotation[1][0] << endl;
    cout << "rotation[1][1] = " << rotation[1][1] << endl;
    cout << "rotation[1][2] = " << rotation[1][2] << endl;
    cout << "rotation[2][0] = " << rotation[2][0] << endl;
    cout << "rotation[2][1] = " << rotation[2][1] << endl;
    cout << "rotation[2][2] = " << rotation[2][2] << endl;

    cout << "Calculating J..." << endl;

    overlap_integral = A.calcJ( B, displ, rotation, input);

	return(overlap_integral[0]); // JMF 2024-10-08 rewrote directly in the
                                 // calcJ function so that it returns J, rather
                                 // than taking the sqrt(J*J) 
    //THIS NOW RETURNS J. JMF 17-1-07
}

