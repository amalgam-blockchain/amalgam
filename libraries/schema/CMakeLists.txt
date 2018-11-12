file(GLOB HEADERS "include/amalgam/schema/*.hpp")
add_library( amalgam_schema schema.cpp ${HEADERS} )
target_link_libraries( amalgam_schema fc )
target_include_directories( amalgam_schema PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_schema

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
install( FILES ${HEADERS} DESTINATION "include/amalgam/schema" )
