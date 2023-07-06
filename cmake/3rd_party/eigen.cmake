option(EIGEN_TEST_CXX11 "Enable testing with C++11 and C++11 features (e.g. Tensor module)." OFF)
option(EIGEN_SPLIT_LARGE_TESTS "Split large tests into smaller executables" ON)
option(EIGEN_BUILD_DOC "Enable creation of Eigen documentation" OFF)
option(BUILD_TESTING "Enable creation of Eigen tests." OFF)

add_subdirectory(eigen)
