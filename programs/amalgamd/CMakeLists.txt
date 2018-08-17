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
      amalgam_plugins
      amalgam_mf_plugins
      amalgam_app
      amalgam_witness
      amalgam_account_history
      amalgam_chain
      amalgam_protocol
      fc
      ${CMAKE_DL_LIBS}
      ${PLATFORM_SPECIFIC_LIBS}
   )
else( AMALGAM_STATIC_BUILD )
   target_link_libraries( amalgamd PRIVATE
      amalgam_plugins
      amalgam_mf_plugins
      amalgam_app
      amalgam_witness
      amalgam_account_history
      amalgam_chain
      amalgam_protocol
      fc
      ${CMAKE_DL_LIBS}
      ${PLATFORM_SPECIFIC_LIBS}
   )
endif( AMALGAM_STATIC_BUILD )

install( TARGETS
   amalgamd

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
