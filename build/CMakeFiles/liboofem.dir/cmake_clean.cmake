file(REMOVE_RECURSE
  "liboofem.pdb"
  "liboofem.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/liboofem.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
