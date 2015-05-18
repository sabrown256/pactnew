separate_arguments(HEADERS UNIX_COMMAND ${LIBHEADERS})


## Copy headers to the includes folder within the build directory
foreach(hdr ${HEADERS})
   message(STATUS "copy ${hdr} to ${HEADER_INCLUDES_DIRECTORY}")
   file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${TGTLIB}/${hdr} DESTINATION ${HEADER_INCLUDES_DIRECTORY})
endforeach()