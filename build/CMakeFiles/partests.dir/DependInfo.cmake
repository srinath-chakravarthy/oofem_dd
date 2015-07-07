# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  )
# The set of files for implicit dependencies of each language:

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS
  "MPICH_IGNORE_CXX_SEEK"
  "__DD_MODULE"
  "__FM_MODULE"
  "__LAPACK_MODULE"
  "__METIS_MODULE"
  "__PARALLEL_MODE"
  "__PETSC_MODULE"
  "__SM_MODULE"
  "__TM_MODULE"
  )

# Targets to which this target links.
set(CMAKE_TARGET_LINKED_INFO_FILES
  )

# The include file search paths:
set(CMAKE_C_TARGET_INCLUDE_PATH
  "../src/oofemlib"
  "."
  "../src/fm"
  "../src/tm"
  "../src/sm"
  "../src/dd"
  "../src/dss"
  "/opt/petsc/linux_gcc_openmpi/include"
  "/usr/lib/openmpi/include"
  "/usr/lib/openmpi/include/openmpi"
  ".."
  )
set(CMAKE_CXX_TARGET_INCLUDE_PATH ${CMAKE_C_TARGET_INCLUDE_PATH})
set(CMAKE_Fortran_TARGET_INCLUDE_PATH ${CMAKE_C_TARGET_INCLUDE_PATH})
set(CMAKE_ASM_TARGET_INCLUDE_PATH ${CMAKE_C_TARGET_INCLUDE_PATH})
