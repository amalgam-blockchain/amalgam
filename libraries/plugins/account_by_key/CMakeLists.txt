file(GLOB HEADERS "include/amalgam/account_by_key/*.hpp")

add_library( amalgam_account_by_key
             account_by_key_plugin.cpp
             account_by_key_api.cpp
           )

target_link_libraries( amalgam_account_by_key amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_account_by_key
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_account_by_key

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
