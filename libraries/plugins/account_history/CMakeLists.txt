file(GLOB HEADERS "include/amalgam/account_history/*.hpp")

add_library( amalgam_account_history
             account_history_plugin.cpp
           )

target_link_libraries( amalgam_account_history amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_account_history
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_account_history

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
