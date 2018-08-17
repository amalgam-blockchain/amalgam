file(GLOB HEADERS "include/amalgam/private_message/*.hpp")

add_library( amalgam_private_message
             private_message_plugin.cpp
           )

target_link_libraries( amalgam_private_message amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_private_message
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_private_message

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
