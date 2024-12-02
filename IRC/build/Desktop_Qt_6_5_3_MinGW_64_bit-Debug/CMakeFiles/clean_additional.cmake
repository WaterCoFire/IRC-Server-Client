# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\IRC_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\IRC_autogen.dir\\ParseCache.txt"
  "IRC_autogen"
  )
endif()
