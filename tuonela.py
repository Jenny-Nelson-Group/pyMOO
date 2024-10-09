#Jarvist Frost wrote this 8-1-07 -->
# tuonela: Program to generate Rotations & Transpositions of molecules
#          for exploration of Transfer Integral phase space
#A great whirl was caused at the north pole by the rotation of column of sky. 
#Through this whirl souls could go to the underground land of dead, Tuonela.

# 2024-10-08: Jarvist: updating this for Python 3 + recent pyMOO

from numpy import *
import numpy as np

from math import sin,cos

import pyMOO

set_printoptions(precision=3, suppress=True,linewidth=1000) #numpy

#Form and definitons from
# http://en.wikipedia.org/wiki/Matrix_rotation  8-1-07
def roll(theta):
    return(exp(matrix([ (0.0,0.0,0.0), (0.0,0.0,-theta), (0.0,theta,0.0) ])))    
def pitch(theta):
    return(exp(matrix([ (0.0,0.0,theta), (0.0,0.0,0.0), (-theta,0.0,0.0) ])))    
def yaw(theta):
    return(exp(matrix([ (0.0,-theta,0.0), (theta,0.0,0.0), (0.0,0.0,0.0) ])))

def xroll(theta):
    return(matrix([(1.0,0.0,0.0),(0.0,cos(theta),sin(theta)),(0.0,-sin(theta),cos(theta))]))
def yroll(theta):
    return(matrix([(cos(theta),0.0,-sin(theta)),(0.0,1.0,0.0),(sin(theta),0.0,cos(theta))]))
def zroll(theta):
    return(matrix([(cos(theta),sin(theta),0.0),(-sin(theta),cos(theta),0.0),(0.0,0.0,1.0)]))

# God - I forgot how primitive Python 2 was!
#def frange(start, stop, step=1.0):
#    sign = cmp(0, step)
#    while cmp(start, stop) == sign:
#        yield start
#        start += step

I=matrix([(1.0,0.0,0.0),(0.0,1.0,0.0),(0.0,0.0,1.0)])        
 
#print I
    
pyMOO.readmols() #read molecule / init
print("Successfully read molecules...")

for r in np.linspace(6, 15, 3):
    for t in np.linspace(0,pi,96):
        for phi in np.linspace(0, pi, 96):
#            print
#            x=r*sin(rolltheta)
#            y=r*cos(rolltheta)
#            z=0.0
            vec=matrix([[(r),(0.0),(0.0)]])
#            print vec
            vec=vec*(yroll(t)*zroll(phi))
#            print vec
            
#            rotmat=yroll(2.0*pi/3.0)
            rotmat=yroll(t)*zroll(phi)

#            print(f"r = {r}, t = {t}, phi = {phi}")

#            print(vec[0,0],vec[0,1],vec[0,2],rotmat[0,0],rotmat[0,1],rotmat[0,2],rotmat[1,0],rotmat[1,1],rotmat[1,2],rotmat[2,0],rotmat[2,1],rotmat[2,2])

            tint=pyMOO.transferint(vec[0,0],vec[0,1],vec[0,2],rotmat[0,0],rotmat[0,1],rotmat[0,2],rotmat[1,0],rotmat[1,1],rotmat[1,2],rotmat[2,0],rotmat[2,1],rotmat[2,2])
#            print rotmat

            print(t,phi,r,vec[0,0],vec[0,1],vec[0,2], tint,log(tint))
