#Jarvist Frost wrote this 8-1-07 -->
# tuonela: Program to generate Rotations & Transpositions of molecules
#          for exploration of Transfer Integral phase space
#A great whirl was caused at the north pole by the rotation of column of sky. 
#Through this whirl souls could go to the underground land of dead, Tuonela.

from numpy import *
from math import sin,cos

import pyMOO

import os

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


def frange(start, stop, step=1.0):
    sign = cmp(0, step)
    while cmp(start, stop) == sign:
        yield start
        start += step

I=matrix([(1.0,0.0,0.0),(0.0,1.0,0.0),(0.0,0.0,1.0)])        
 
#print I
    
pyMOO.readmols() #read molecule / init

r=9

tbest=0.0

a=b=c=0
b=pi/6

#for a in frange (-pi, pi, pi/3):
#    for b in frange (-pi, pi, pi/3):
#        for c in frange (-pi, pi, pi/3):
for t in frange (-pi,pi,pi/400):
    for phi in [0.0]: #frange (0, pi, pi/200):
        vec=matrix([[(r),(0.0),(0.0)]])
        vec=vec*(yroll(t)*zroll(phi))

        rotmat=xroll(a)*yroll(b)*zroll(c)
        tint=pyMOO.transferint(vec[0,0],vec[0,1],vec[0,2],rotmat[0,0],rotmat[0,1],rotmat[0,2],rotmat[1,0],rotmat[1,1],rotmat[1,2],rotmat[2,0],rotmat[2,1],rotmat[2,2])

        if (tint>tbest):
            os.system("mv tmp.out best.out")
            tbest=tint
            print "New Best!",tbest
        print log(tint),a,b,c,t,phi,vec[0,0],vec[0,1],vec[0,2], tint,log(tint)
                    
