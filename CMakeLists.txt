cmake_minimum_required(VERSION 3.5)

project(signature VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

add_executable(signature main.cpp)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

set_target_properties(signature PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)

target_link_libraries(signature
	${Boost_LIBRARIES}
)

install(TARGETS signature RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT sava_test@mail.ru)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include (CPack)
