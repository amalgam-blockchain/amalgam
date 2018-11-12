add_executable( amalgamd main.cpp )
if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling amalgamd with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

if( AMALGAM_STATIC_BUILD )
   target_link_libraries( amalgamd PRIVATE
      "-static-libstdc++ -static-libgcc"
      appbase
      amalgam_utilities
      amalgam_plugins
      ${CMAKE_DL_LIBS}
      ${PLATFORM_SPECIFIC_LIBS}
   )
else( AMALGAM_STATIC_BUILD )
   target_link_libraries( amalgamd PRIVATE
      appbase
      amalgam_utilities
      amalgam_plugins
      ${CMAKE_DL_LIBS}
      ${PLATFORM_SPECIFIC_LIBS}
   )
endif( AMALGAM_STATIC_BUILD )

if( CLANG_TIDY_EXE )
   set_target_properties(
      amalgamd PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

install( TARGETS
   amalgamd

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
