# --------------------------------------------------
# CTest
# --------------------------------------------------
enable_testing()

include(ProcessorCount)
ProcessorCount(N)
if(N EQUAL 0)
  set(N 1)
endif()

set(CTEST_OPTS -j${N} --output-on-failure --progress ${CTEST_OPTS} -C $<CONFIG>)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} ${CTEST_OPTS}
                        USES_TERMINAL)
add_dependencies(check llvm2c)

# --------------------------------------------------
# Clang and Opt
# --------------------------------------------------
# REQUIRED available only with CMake 3.18+
foreach(TOOL CLANG OPT)
  string(TOLOWER ${TOOL} TOOL_LOWER)

  # REQUIRED available only with CMake 3.18+
  find_program(${TOOL} ${TOOL_LOWER} PATHS ${LLVM_TOOLS_BINARY_DIR}
                                     NO_DEFAULT_PATH)
  if(NOT ${TOOL})
    message(FATAL_ERROR "${TOOL_LOWER}: version compatible with LLVM \
                         ${LLVM_PACKAGE_VERSION} not found")
  endif()
  message(STATUS "${TOOL_LOWER}: ${${TOOL}}")
endforeach()

# --------------------------------------------------
# Tests
# --------------------------------------------------
configure_file(run.in run @ONLY)

set(tests
  asm
  branching
  globals
  loops
  main
  math
  phi
  pointer
  standard_lib
  statements
  struct
  union
)

foreach(test ${tests})
  add_test(NAME ${test} COMMAND ./run ${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT LLVM2C=$<TARGET_FILE:llvm2c>)
endforeach()
