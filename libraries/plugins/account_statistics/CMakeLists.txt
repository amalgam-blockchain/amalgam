file(GLOB HEADERS "include/amalgam/account_statistics/*.hpp")

add_library( amalgam_account_statistics
             account_statistics_plugin.cpp
             account_statistics_api.cpp
           )

target_link_libraries( amalgam_account_statistics amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_account_statistics
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_account_statistics

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)