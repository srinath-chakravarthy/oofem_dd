  set (dd
    complex.cpp
    point.cpp
    domain.cpp
    slipplane.cpp
    slipsystem.cpp
    point/dislocation.cpp
    point/source.cpp
    point/obstacle.cpp
    point/virtualdislocation.cpp
    timemanager.cpp
    ddcounter.cpp
    forcecache.cpp
    )
  
add_library(dd
    ${LIB_TYPE}
    ${dd}
    )
