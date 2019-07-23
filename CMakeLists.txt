cmake_minimum_required(VERSION 3.5)

IF(DEFINED $ENV{TRAVIS_BUILD_NUMBER})
	project(signature VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
ELSE()
	project(signature VERSION 0.0.0)
ENDIF()

set(CMAKE_CXX_STANDARD 14)

add_executable(signature main.cpp)
add_library(signature_option_lib option.cpp)
add_library(signature_operation_lib operation.cpp)

find_package(Threads)
find_package(Boost COMPONENTS program_options filesystem REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

set_target_properties(signature PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)

target_link_libraries(signature_option_lib
	${Boost_LIBRARIES}
)

target_link_libraries(signature_operation_lib
	${Boost_LIBRARIES}
)

target_link_libraries(signature
	${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	signature_option_lib
	signature_operation_lib
)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT sava_test@mail.ru)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include (CPack)
