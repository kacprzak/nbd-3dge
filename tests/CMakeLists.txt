macro( add_test_exec test_name srcs )
    string( TOLOWER ${test_name} test_lower_name srcs )
    set( exec_name ${test_lower_name}_test)

    set( abs_srcs "" )
    foreach( item ${srcs} )
      list( APPEND abs_srcs ${CMAKE_SOURCE_DIR}/src/${item} )
    endforeach()

    add_executable( ${exec_name} ${test_name}_test.cpp ${abs_srcs} )
    target_include_directories( ${exec_name} PRIVATE ${CMAKE_SOURCE_DIR}/src )
    target_link_libraries( ${exec_name} Boost::unit_test_framework )
    add_test( ${test_name} ${exec_name} )
endmacro( add_test_exec )

add_test_exec( MtlLoader "MtlLoader.cpp;Loader.cpp;Util.cpp" )
