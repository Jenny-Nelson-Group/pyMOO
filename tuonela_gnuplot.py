#Jarvist Frost wrote this 8-1-07 -->
# tuonela: Program to generate Rotations & Transpositions of molecules
#          for exploration of Transfer Integral phase space
#A great whirl was caused at the north pole by the rotation of column of sky. 
#Through this whirl souls could go to the underground land of dead, Tuonela.

import Gnuplot
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

a=b=c=0.0
a=pi/6

g = Gnuplot.Gnuplot()

for a in frange (0.0001, 2*pi, pi/96):
    for b in [0.0]: #frange (-pi, pi, pi/3):
        for c in [0.0]: #frange (-pi, pi, pi/3):
            out=open('f_'+str(a)+'_'+str(b)+'_'+str(c)+'.dat','w')
            print "Generating: ",a,b,c
            for t in frange (-pi,pi,pi/192):
                for phi in frange (-pi/2, pi/2, pi/192):
                    vec=matrix([[(r),(0.0),(0.0)]])
                    vec=vec*(yroll(t)*zroll(phi))

                    rotmat=xroll(a)*yroll(b)*zroll(c)
                    tint=pyMOO.transferint(vec[0,0],vec[0,1],vec[0,2],rotmat[0,0],rotmat[0,1],rotmat[0,2],rotmat[1,0],rotmat[1,1],rotmat[1,2],rotmat[2,0],rotmat[2,1],rotmat[2,2])

#        if (tint>tbest):
#            os.system("mv tmp.out best.out")
#            tbest=tint
#            print "New Best!",tbest
                    print >>out,log(tint),a,b,c,t,phi,vec[0,0],vec[0,1],vec[0,2], tint,log(tint)
            g.title('a='+str(a)+' b='+str(b)+' c='+str(c))
            g.xlabel('Theta')
            g.ylabel('Phi')
            g('set view map')
            g('unset key')
            g('set xrange [-3.1415:3.1415]')
            g('set yrange [-1.57075:1.57075]')
            g('set cbrange [0:0.6]')
#            g('set palette maxcolors 100')
            g('unset grid')
#            g('set pm3d')
#            g('set dgrid3d 96,48,2')
#            g.splot(Gnuplot.File('f_'+str(a)+'_'+str(b)+'_'+str(c)+'.dat', using="5:6:10 palette",with="d"))


            g('set pm3d map; splot \'<awk -f colorpts.awk '+'f_'+str(a)+'_'+str(b)+'_'+str(c)+'.dat'+' 0.013 0.013\'') #0.03272 ish (exact pi/96)
            
            g('set terminal postscript enhanced colour; set output \"'+'f_'+str(a)+'_'+str(b)+'_'+str(c)+'.ps'+'\"; replot')

            g('set terminal x11; set output \"false.out\"') #just in case...
            
#            g('sp \"'+str(a)+'_'+str(b)+'_'+str(c)+'.dat'+'\" u 5:6:10 palette')
#            g.hardcopy('f_'+str(a)+'_'+str(b)+'_'+str(c)+'.ps',enhanced=1,color=1)
#            g.hardcopy('f_'+str(a)+'_'+str(b)+'_'+str(c)+'.png',terminal='png')            
            print "Plot saved to: "+'f_'+str(a)+'_'+str(b)+'_'+str(c)+'.ps'
            g.reset()
