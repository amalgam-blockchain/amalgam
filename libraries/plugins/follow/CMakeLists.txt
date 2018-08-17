file(GLOB HEADERS "include/amalgam/follow/*.hpp")

add_library( amalgam_follow
             follow_plugin.cpp
             follow_api.cpp
             follow_operations.cpp
             follow_evaluators.cpp
           )

target_link_libraries( amalgam_follow amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_follow
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_follow

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
