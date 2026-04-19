add_executable(test_util test_util.c)

target_link_libraries(test_util PRIVATE cucumber_internal)

target_include_directories(test_util PRIVATE
    ${CMAKE_SOURCE_DIR}/include/internal
    ${CMAKE_SOURCE_DIR}/tests
)

# Register with CTest
add_test(NAME test_util COMMAND test_util)

# Valgrind target, only available if Valgrind is installed
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_test(
        NAME test_util_valgrind
        COMMAND ${VALGRIND}
            --leak-check=full
            --track-origins=yes
            --error-exitcode=1
            $<TARGET_FILE:test_util>
    )
endif()