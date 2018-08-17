file(GLOB HEADERS "include/amalgam/blockchain_statistics/*.hpp")

add_library( amalgam_blockchain_statistics
             blockchain_statistics_plugin.cpp
             blockchain_statistics_api.cpp
           )

target_link_libraries( amalgam_blockchain_statistics amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_blockchain_statistics
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_blockchain_statistics

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)