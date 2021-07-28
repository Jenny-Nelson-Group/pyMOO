# pyMOO

pyMOO is a Python interface to, and repository for, James Kirkpatrick's 2007 'Molecular orbital overlap' (MOO) C++ code. 
This calculates the electronic transfer integral between two identical molecules, by calculating the overlap of the molecular orbitals at the level of the ZINDO Hamiltonian. 
A key benefit compared to other methods is that it is extremely fast. You start the use of this code by calculating the molecular orbitals for an individual molecule, and then the overlap between these orbitals and a projected set (the second molecule) takes < 0.1 ms. 
This allows you to interactively explore the phase space of transfer integrals between two organic semiconductors. 

A limitation is that the ZINDO Hamiltonian struggles with more complex electronic structure, such as that present in Fullerene based molecules, and push-pull (donor-acceptor) polymers. 
Transfer integrals at close separations (where the transfer integrals are large) are also likely to be increasingly erroneous, as the electronic structure doesn't change. 

The following journal article describes the method, and would be a useful starting point to read if you want to use this technique. Please also cite this article if you make use of the method or codes.

> Kirkpatrick, J., 2008. 
>
> An approximate method for calculating transfer integrals based on the ZINDO Hamiltonian. Int. J. Quantum Chem. 108, 51–56. 
>
> https://doi.org/10.1002/qua.21378
>
> In this article, we discuss a method for calculating transfer integrals for pairs of molecules based on Zerner's Independent Neglect of Differential Overlap Hamiltonian which requires only a single self-consistent field calculation on an isolated molecule to be performed to determine the transfer integral for a pair of molecules. This method is compared with results obtained by projection of the pair of molecules' molecular orbitals onto the vector space defined by the molecular orbitals of each isolated molecule. The two methods are found to be in good agreement using three compounds as model systems: pentacene, ethylene, and hexabenzocoronene. © 2007 Wiley Periodicals, Inc. Int J Quantum Chem, 2008
