#ifndef FILE_MOLECULES_EL
#define FILE_MOLECULES_EL

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "../rotation/rotation.h"
#include "../charges/charges.h"
#include "../coord/coord.h"

using namespace std;

class mol_and_orb
{
private:
    coord pos [NAT_MAX]; // positions of atoms
    char type [NAT_MAX]; // char for atom types
    int lbl   [NAT_MAX]; // int for atom types
    char bs     [NBASIS_MAX]; //basis sets
    int N; // the number of atoms in the molecule
    int NBasis; // the number of basis sets

    coord centre;
    double psi [NBASIS_MAX][NBASIS_MAX]; //orbitals
    double F [NBASIS_MAX][NBASIS_MAX]; //  Fock Matrix
    multipoles neutr, an, cat; // include all multipoles in the mol_and_orb class

public:

    int n_el;

    mol_and_orb (){
    }

    ~mol_and_orb(){
    }

    coord getpos(int & i){   //accessors
    	return pos[i];
	}

    char gettype(int & i){
    	return type[i];
	}

    int getN(){
    	return N;
	}
    coord getcentre(){
    	return centre;
    }

    void cp_pos_crg(mol_and_orb const & A)
    {
	int i=0;
	while (i<N)
	{
	    pos[i].x=A.pos[i].x;
	    pos[i].y=A.pos[i].y;
	    pos[i].z=A.pos[i].z;
	    i++;
	}
	neutr.cp_crg(A.neutr, N);
	cat.cp_crg(A.cat, N);
	an.cp_crg(A.an, N);
    }

    void cp_pos(mol_and_orb const & A)
    {
	int i=0;
	while (i<N)
	{
	    pos[i].x=A.pos[i].x;
	    pos[i].y=A.pos[i].y;
	    pos[i].z=A.pos[i].z;
	    i++;
	}
	centre=A.centre;
    }

	
    void cp_orb( mol_and_orb const  &A){
	for (int i=0; i < NBasis;i++) {
		for (int j =0; j < NBasis ; j++){
			psi[i][j] = A.psi[i][j];
		}
	}	
    }

    void rotate(const rot &a)
    {
	int i=0;
	while ( i < N )
	{
	    pos[i].rotate(a);
	    i++;
	}
	centre.rotate(a);
    }
    void rotate_crg(rot a){
        neutr.rotate(a, N);
        cat.rotate(a, N);
        an.rotate(a, N);
    }


    void shift( coord & a)
    {
	int i=0;
	while ( i < N )
	{
	    pos[i]=pos[i]+a;
	    i++;
	}
       centre = centre +a;
    }

    void print(){
	cout << N <<endl<<endl;
	cout.setf(ios::scientific);
     	for (int i=0;i<N;i++){
	    coord tmp = pos[i] * RA;
	    cout << type[i] << '\t' << tmp;
	}
    }
		
    void operator =(mol_and_orb A)
    {
	    N=A.N;
	    centre=A.centre;
    }

   

    double total_charge_neutr(){
	double crg=0.0; 
	for (int i=0; i < N; i++){
	    	crg += neutr.mpls[i];
	}
	return crg;
    }
    double total_charge_cat(){
	double crg=0.0; 
	for (int i=0; i < N; i++){
	    	crg += cat.mpls[i];
	}
	return crg;
    }
    double total_charge_an(){
	double crg=0.0; 
	for (int i=0; i < N; i++){
	    	crg += an.mpls[i];
	}
	return crg;
    }
    int read_orb (const int &, char nameorbs[]="fort.7");
    int calc_F_el_with_HASH( double & ,  double & ,  double & , double & , double & , double & , int & , int & , int & , int &  );
    int calc_F_lean( mol_and_orb, int const & );

    int init(char * nameinput , char * nameorbs, char * nameneutr, char * namecat, char * namean );
    int init_nocharge(char * nameinput , char * nameorbs );
    int init_nocharge_noorb(char * nameinput);	
	
    vector <double> calcJ_DG( mol_and_orb & , mol_and_orb &, mol_and_orb &,  vector<int*>);
    vector <double> calcJ( mol_and_orb & , mol_and_orb &, mol_and_orb &,  vector<int*>);
    vector <double> calcJ( mol_and_orb & , double [3], double [3][3], vector<int*>);
    vector <double> calcJ( mol_and_orb & , double [3], double [3][3], vector<int*>, char *);
	

    void calcallJ( mol_and_orb & , double [3], double [3][3]); 
    double calcS( int, int);
    void nrg_EStone_part( double [5], mol_and_orb &); //this bastard is received by copying out Stones equations and making them fast!.
    void nrg_EStone_part_mpls( double [5], mol_and_orb &); //this does it just for mpl mpl interaction
    double exp_6( mol_and_orb const & );
    double find_rmin_binary( mol_and_orb &, double, double, double, double, double);
};
#endif //FILE_MOLECULES_EL
