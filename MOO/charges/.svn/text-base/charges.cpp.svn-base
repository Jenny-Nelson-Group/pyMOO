#include "charges.h"

int multipoles::read_crg_eps(char * namefile, char *type, int &N){
        ifstream in(namefile);
        int i=0;
        string word;

        while( i < N ) {
                in >> word;
                in >> word;
                if ( type[i] != word[0] ){
                        cout << "Error in reading charges atom:" <<  i  << "is not of type" << type[i] << endl;
                        return 1;
                }
                in >> word;
                sscanf(word.c_str(), "%lf", &mpls[i]);
                i++;
        }
        return 0;
}


int multipoles::read_crg(char * namefile, char * type, int &N)
{
	ifstream in (namefile);
	string line, word, word2, word3, word4 , word5;
	double q10, q11c, q11s, q20, q22c,q22s, q21c, q21s; // from the manual

	getline(in, line);
	getline(in,line);
	getline(in, line);
		
	int i=0;
	int rank;
	
	while(i < N ){
		in >> word;
		if ( type[i] != word[0] ){
			cout << "Error reading charges " <<endl;
			return 1;
		}
		in >> word; // x pos
		in >> word; // ypos
		in >> word; //zpos
		in >> word; //"Rank"
		in >> word; // this is the one
		getline(in, line); // return character
		sscanf ( word.c_str(), "%i", &rank);
		if (rank == 2)
		{
			in >> word;
			sscanf(word.c_str(), "%lf", &mpls[i]);
			
			in >> word;
			in >> word2;
			in >> word3;
			sscanf(word.c_str(), "%lf", &q10);
			sscanf(word2.c_str(), "%lf", &q11c);
        	        sscanf(word3.c_str(), "%lf", &q11s);
			dpls[i][0]=q11c;
			dpls[i][1]=q11s;
		       	dpls[i][2]=q10;
			getline(in, line);
	
			in >> word;
               	 	in >> word2;
             		in >> word3;
			in >> word4;
			in >> word5;
                	sscanf(word.c_str(), "%lf", &q20);
               		sscanf(word2.c_str(), "%lf", &q21c);
                	sscanf(word3.c_str(), "%lf", &q21s);
                	sscanf(word4.c_str(), "%lf", &q22c);
                	sscanf(word5.c_str(), "%lf", &q22s);
			qdrpls[i][0][0] =  - q20 * 0.5 + 0.5 * sqrt(3.0) * q22c;
			qdrpls[i][0][1] =	          0.5*sqrt(3.0) * q22s;
			qdrpls[i][0][2] =	          0.5*sqrt(3.0) * q21c;
			qdrpls[i][1][1] =	        - q20 * 0.5 - 0.5 * sqrt(3.0) * q22c;
			qdrpls[i][1][2] =	          0.5*sqrt(3.0) * q21s;
			qdrpls[i][2][2] = q20;
			qdrpls[i][2][1] = qdrpls[i][1][2];
			qdrpls[i][2][0] = qdrpls[i][0][2];
			qdrpls[i][1][0] = qdrpls[i][0][1];
			// temporary only // 
			/*
			if (qdrpls[i].q[0]+qdrpls[i].q[3] != -qdrpls[i].q[5] ){
			    cout << "Error in reading quadrupole " << qdrpls[i].q[0]+qdrpls[i].q[3] <<
			      "  " << -qdrpls[i].q[5] << " " << i <<endl;
			    qdrpls[i].q[5]=-qdrpls[i].q[0]-qdrpls[i].q[3];
			}*/

			getline(in, line);
		}	
		else if (rank==0){
			in >> word;
			sscanf(word.c_str(), "%lf", &mpls[i]);
		        dpls[i][0]=0.0;
                        dpls[i][1]=0.0;
                        dpls[i][2]=0.0;
		        qdrpls[i][0][0] = 0.0;
		      	qdrpls[i][0][1] = 0.0;
		    	qdrpls[i][0][2] = 0.0;
		    	qdrpls[i][1][1] = 0.0;
		    	qdrpls[i][1][2] = 0.0;
		    	qdrpls[i][2][2] = 0.0;
	     	    	qdrpls[i][2][1] = 0.0;
		    	qdrpls[i][2][0] = 0.0;
		    	qdrpls[i][1][0] = 0.0;		
			getline (in, line);
		}
		else if (rank ==1 ){
		    in >> word;
                    sscanf(word.c_str(), "%lf", &mpls[i]);

                    in >> word;
                    in >> word2;
                    in >> word3;
                    sscanf(word.c_str(), "%lf", &q10);
                    sscanf(word2.c_str(), "%lf", &q11c);
                    sscanf(word3.c_str(), "%lf", &q11s);
		    dpls[i][0]=q11c;
                    dpls[i][1]=q11s;
                    dpls[i][2]=q10;
                    getline(in, line);
		    qdrpls[i][0][0] = 0.0;
		    qdrpls[i][0][1] = 0.0;
		    qdrpls[i][0][2] = 0.0;
		    qdrpls[i][1][1] = 0.0;
		    qdrpls[i][1][2] = 0.0;
		    qdrpls[i][2][2] = 0.0;
	     	    qdrpls[i][2][1] = 0.0;
		    qdrpls[i][2][0] = 0.0;
		    qdrpls[i][1][0] = 0.0;	
		}
		else if (rank == 4) {
			in >> word;
			sscanf(word.c_str(), "%lf", &mpls[i]);
			
			in >> word;
			in >> word2;
			in >> word3;
			sscanf(word.c_str(), "%lf", &q10);
			sscanf(word2.c_str(), "%lf", &q11c);
        	        sscanf(word3.c_str(), "%lf", &q11s);
			dpls[i][0]=q11c;
			dpls[i][1]=q11s;
		       	dpls[i][2]=q10;
			getline(in, line);
	
			in >> word;
               	 	in >> word2;
             		in >> word3;
			in >> word4;
			in >> word5;
                	sscanf(word.c_str(), "%lf", &q20);
               		sscanf(word2.c_str(), "%lf", &q21c);
                	sscanf(word3.c_str(), "%lf", &q21s);
                	sscanf(word4.c_str(), "%lf", &q22c);
                	sscanf(word5.c_str(), "%lf", &q22s);
			qdrpls[i][0][0] =  - q20 * 0.5 + 0.5 * sqrt(3.0) * q22c;
			qdrpls[i][0][1] =	          0.5*sqrt(3.0) * q22s;
			qdrpls[i][0][2] =	          0.5*sqrt(3.0) * q21c;
			qdrpls[i][1][1] =	        - q20 * 0.5 - 0.5 * sqrt(3.0) * q22c;
			qdrpls[i][1][2] =	          0.5*sqrt(3.0) * q21s;
			qdrpls[i][2][2] = q20;
			qdrpls[i][2][1] = qdrpls[i][1][2];
			qdrpls[i][2][0] = qdrpls[i][0][2];
			qdrpls[i][1][0] = qdrpls[i][0][1];
			getline(in, line);
			getline(in, line);
			getline(in, line);
			getline(in, line);
			getline(in, line);
		} 

		i++;
	}
	return 0;
	
}

