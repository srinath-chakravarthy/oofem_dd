nonstat_nonlin_02.out.tm
Quadrilateral elements subjected to heat flux (Newton b.c)
NonStationaryProblem nsteps 3 deltat 36000 alpha 0.5 lumpedcapa exportfields 1 5 nmodules 1
errorcheck
#vtkxml tstep_all domain_all primvars 1 6
domain heattransfer
OutputManager tstep_all dofman_all element_all
ndofman 9 nelem 4 ncrosssect 1 nmat 1 nbc 2 nic 1 nltf 1
node 1 coords 3 0.000000e+00 0.000000e+00 0.000000e+00 ic 1 1 bc 1 2
node 2 coords 3 0.100000e+00 0.000000e+00 0.000000e+00 ic 1 1 bc 1 2
node 3 coords 3 0.200000e+00 0.000000e+00 0.000000e+00 ic 1 1 bc 1 2
node 4 coords 3 0.000000e+00 1.000000e+00 0.000000e+00 ic 1 1
node 5 coords 3 0.100000e+00 1.000000e+00 0.000000e+00 ic 1 1
node 6 coords 3 0.200000e+00 1.000000e+00 0.000000e+00 ic 1 1 
node 7 coords 3 0.000000e+00 2.000000e+00 0.000000e+00 ic 1 1 
node 8 coords 3 0.100000e+00 2.000000e+00 0.000000e+00 ic 1 1
node 9 coords 3 0.200000e+00 2.000000e+00 0.000000e+00 ic 1 1 
quad1ht 1 nodes 4 1 2 5 4 crosssect 1 mat 1
quad1ht 2 nodes 4 2 3 6 5 crosssect 1 mat 1
quad1ht 3 nodes 4 4 5 8 7 crosssect 1 mat 1 boundaryLoads 2 1 3
quad1ht 4 nodes 4 5 6 9 8 crosssect 1 mat 1 boundaryLoads 2 1 3
SimpleCS 1 thick 1.0
IsoHeat 1 d 2400. k 1.5 c 800.0
constantedgeload 1 loadTimeFunction 1 ndofs 1 components 1 -2000.0 loadtype 2
BoundaryCondition 2 loadTimeFunction 1 prescribedvalue 0.0
InitialCondition 1 Conditions 1 u 0.0
ConstantFunction 1 f(t) 1.0
#%BEGIN_CHECK%
#NODE tStep 1 number 5 dof 10 unknown d value 9.98146959e-01
#NODE tStep 1 number 9 dof 10 unknown d value 7.29756332e+01
#NODE tStep 2 number 5 dof 10 unknown d value 3.88482167e+00
#NODE tStep 2 number 9 dof 10 unknown d value 1.42064950e+02
#NODE tStep 3 number 5 dof 10 unknown d value 8.45463247e+00
#NODE tStep 3 number 9 dof 10 unknown d value 2.07578282e+02
#%END_CHECK%

