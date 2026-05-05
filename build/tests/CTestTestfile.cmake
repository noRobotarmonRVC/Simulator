# CMake generated Testfile for 
# Source directory: /home/wan/OOAD/Simulator/Simulator/tests
# Build directory: /home/wan/OOAD/Simulator/Simulator/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_grid "/home/wan/OOAD/Simulator/Simulator/build/tests/test_grid")
set_tests_properties(test_grid PROPERTIES  _BACKTRACE_TRIPLES "/home/wan/OOAD/Simulator/Simulator/tests/CMakeLists.txt;16;add_test;/home/wan/OOAD/Simulator/Simulator/tests/CMakeLists.txt;0;")
add_test(test_rvcstate "/home/wan/OOAD/Simulator/Simulator/build/tests/test_rvcstate")
set_tests_properties(test_rvcstate PROPERTIES  _BACKTRACE_TRIPLES "/home/wan/OOAD/Simulator/Simulator/tests/CMakeLists.txt;16;add_test;/home/wan/OOAD/Simulator/Simulator/tests/CMakeLists.txt;0;")
add_test(test_dispatcher "/home/wan/OOAD/Simulator/Simulator/build/tests/test_dispatcher")
set_tests_properties(test_dispatcher PROPERTIES  _BACKTRACE_TRIPLES "/home/wan/OOAD/Simulator/Simulator/tests/CMakeLists.txt;16;add_test;/home/wan/OOAD/Simulator/Simulator/tests/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
