file(GLOB HEADERS "include/amalgam/plugins/raw_block/*.hpp")

add_library( amalgam_raw_block
             ${HEADERS}
             raw_block_plugin.cpp
             raw_block_api.cpp
           )

target_link_libraries( amalgam_raw_block amalgam_app amalgam_chain amalgam_protocol fc )
target_include_directories( amalgam_raw_block
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
