#define PRG_VERSION "OOFEM version 2.4"
#define OOFEG_VERSION "OOFEG version 2.4"
#define OOFEM_COPYRIGHT "Copyright (C) 1994-2014 Borek Patzak"

#define PRG_HEADER_SM "\
____________________________________________________\n\
           OOFEM - Finite Element Solver\n\
        Copyright (C) 1994-2014 Borek Patzak\n\
____________________________________________________\n"

#define PRG_HEADER "\
############################################################## www.oofem.org ###\n\
       ####    ####   ######  ######  #    #\n\
     #    #  #    #  #       #       ##  ##\n\
    #    #  #    #  #####   #####   # ## #\n\
   #    #  #    #  #       #       #    #\n\
  #    #  #    #  #       #       #    #   OOFEM ver. 2.4\n\
  ####    ####   #       ######  #    #    Copyright (C) 1994-2014 Borek Patzak\n\
################################################################################\n\n"

#define HOST_TYPE "x86_64-Linux"
#define HOST_NAME "nazgul.bw01.neu.edu"
#define MODULE_LIST "fm;tm;sm;dd;dss;LAPACK;parmetis;metis;PETSc;MPI;IML++"

/* Headers */

/* Define if unistd.h defined */
#define HAVE_UNISTD_H 1

/* Define if execinfo.h defined */
#define HAVE_EXECINFO_H 1

/* Symbols */
/* Define if access is defined */
#define HAVE_ACCESS 1

/* Define if cbrt is defined */
#define HAVE_CBRT 1

/* Define if isnan is defined */
#define HAVE_ISNAN 1

#include "oofem_export.h"
