file(GLOB HEADERS "include/amalgam/witness/*.hpp")

add_library( amalgam_witness
             witness_plugin.cpp
             witness_evaluators.cpp
             witness_operations.cpp
           )

target_link_libraries( amalgam_witness amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_witness
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_witness

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
