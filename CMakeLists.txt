cmake_minimum_required(VERSION 3.12)

PROJECT(p2p_server)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)


set (CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++17 -D__STRICT_ANSI__")


find_package(Boost 1.73 REQUIRED COMPONENTS
  date_time
  system
  filesystem
  iostreams
  iostreams
  thread
  serialization
  program_options
  log
  )

#for boost.log
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)


find_package (Threads REQUIRED)
find_package (inja REQUIRED)

if(NOT Boost_FOUND)
  message(FATAL "Boost not found")
endif()
#  add_subdirectory(controllers)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(${PROJECT_NAME} "main.cpp" )
#  add_dependencies(${PROJECT_NAME} database)

  target_link_libraries(${PROJECT_NAME}
    Boost::date_time
    Boost::system
    Boost::filesystem
    Boost::iostreams
    Boost::thread
    Boost::log
    #    Boost::mpi
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_SYSTEM_LIBRARY}
    ${MPI_CXX_LIBRARIES}
    ${Boost_LIBRARIES}

    ${CMAKE_THREAD_LIBS_INIT}
    ${LIBBSONCXX_LIBRARIES}
    ${LIBMONGOCXX_LIBRARIES}

    h2o
    ssl
    crypto
    uv
    pthread
    z
    # stl filesystem
    stdc++fs


    yaml-cpp
    log4cpp
    )


set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME OFF)
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
