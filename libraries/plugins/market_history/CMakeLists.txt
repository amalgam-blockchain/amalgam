file(GLOB HEADERS "include/amalgam/market_history/*.hpp")

add_library( amalgam_market_history
             market_history_plugin.cpp
             market_history_api.cpp
           )

target_link_libraries( amalgam_market_history amalgam_chain amalgam_protocol amalgam_app )
target_include_directories( amalgam_market_history
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   amalgam_market_history

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)