file(REMOVE_RECURSE
  "libmfs_client.so.pdb"
  "libmfs_client.so.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/mfs_client.so.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
