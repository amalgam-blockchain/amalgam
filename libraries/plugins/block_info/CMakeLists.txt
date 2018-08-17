file(GLOB HEADERS "include/amalgam/plugins/block_info/*.hpp")

add_library( amalgam_block_info
             ${HEADERS}
             block_info_plugin.cpp
             block_info_api.cpp
           )

target_link_libraries( amalgam_block_info amalgam_app amalgam_chain amalgam_protocol fc )
target_include_directories( amalgam_block_info
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
