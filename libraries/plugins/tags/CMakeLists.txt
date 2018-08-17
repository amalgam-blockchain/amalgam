file(GLOB HEADERS "include/amalgam/tags/*.hpp")

add_library( amalgam_tags
             tags_plugin.cpp )

target_link_libraries( amalgam_tags amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_tags
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_tags

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
