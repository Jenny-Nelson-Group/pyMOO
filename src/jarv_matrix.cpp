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


int main()
{
	// definition of two molecules 
	mol_and_orb A;
	mol_and_orb B;
        
        // initialization (reading the data from the fort.7 file)
	char * namegeom="INPUT_COORDS";
	char * nameorbs="fort.7";
	int checkA = A.init_nocharge(namegeom, nameorbs);
	int checkB = B.init_nocharge(namegeom, nameorbs);

	vector<double> overlap_integral;

	// homo level of the 
	int homo = A.n_el / 2 -1;

	cout << "Number of electrons: " << A.n_el << endl;

	vector< int* > input;
	int comb1[2] = {homo, homo};
//	int comb2[2] = {homo, homo-1};
//	int comb3[2] = {homo-1, homo};
//	int comb4[2] = {homo-1, homo-1};

	input.push_back(comb1);
//	input.push_back(comb2);
//	input.push_back(comb3);
//	input.push_back(comb4);

	// initialize the rotation matrix and translation vector
	double displ[3] = {0.0, 0.0, 0.0};
	double rotation[3][3]= {{1.0 , 0.0, 0.0  }, { 0.0, 1.0, 0.0 }, {0.0, 0.0, 1.0} }; 

        // reading a text file

        using namespace std;

        //int main () {
        string line;
	string delims = " ";
	vector<string> result;

        ifstream myfile ("matrix.txt");


        if (myfile.is_open())
        {
                // read the first line with molecular indeces
//		getline(myfile,line);
//		int number_of_pairs = atoi(line.c_str());
//                cout << "number of pairs: " << number_of_pairs << endl;

                // read in each pair
	  	while(getline(myfile,line)) {
    			//cout << line << endl;  
                        result.clear();
			parce_string (line, delims, &result);
//                        int index1 = atoi(result[0].c_str());
//                        int index2 = atoi(result[1].c_str());
                        for (int i=0; i<3; i++) {displ[i] = atof(result[i].c_str());};
			rotation[0][0] = atof(result[3].c_str());
			rotation[0][1] = atof(result[4].c_str());
			rotation[0][2] = atof(result[5].c_str());
			rotation[1][0] = atof(result[6].c_str());
			rotation[1][1] = atof(result[7].c_str());
			rotation[1][2] = atof(result[8].c_str());
			rotation[2][0] = atof(result[9].c_str());
			rotation[2][1] = atof(result[10].c_str());
			rotation[2][2] = atof(result[11].c_str());
		   
		        char * nameout = "tmp.out";
			overlap_integral = A.calcJ( B, displ, rotation, input, nameout);

		        for (int i=0;i<3;i++)
			    cout <<  displ[i] << " ";
		        cout << "  ";
		        for (int i=0;i<3;i++)
		     {
		            for(int ii=0;ii<3;ii++)
		               cout << rotation[i][ii] << " ";
		            cout << "  ";
		     }
		   
		   
			for (int k =0; k< input.size() ;k++){
				cout << overlap_integral[k] << " ";
			}
			cout << endl;
   		}

        	myfile.close();
        }
        else cout << "Unable to open file"; 


	return 0;
}
