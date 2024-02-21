file(REMOVE_RECURSE
  "libmfs_client.a.a"
  "libmfs_client.a.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/mfs_client.a.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
