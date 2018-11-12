file(GLOB HEADERS "include/amalgam/chain/*.hpp" "include/amalgam/chain/util/*.hpp")

## SORT .cpp by most likely to change / break compile
add_library( amalgam_chain

             # As database takes the longest to compile, start it first
             database.cpp

             amalgam_evaluator.cpp
             amalgam_objects.cpp

             witness_schedule.cpp
             fork_database.cpp

             shared_authority.cpp
             block_log.cpp

             util/impacted.cpp
             util/advanced_benchmark_dumper.cpp

             ${HEADERS}
           )

target_link_libraries( amalgam_chain amalgam_protocol fc chainbase amalgam_schema appbase
                       ${PATCH_MERGE_LIB} )
target_include_directories( amalgam_chain
                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include" )

if( CLANG_TIDY_EXE )
   set_target_properties(
      amalgam_chain PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

if(MSVC)
  set_source_files_properties( database.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

INSTALL( TARGETS
   amalgam_chain

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
INSTALL( FILES ${HEADERS} DESTINATION "include/amalgam/chain" )
