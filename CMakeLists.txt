PROJECT(qucs-project CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
cmake_policy(VERSION 2.8)


file (STRINGS "${qucs-project_SOURCE_DIR}/VERSION" QUCS_VERSION)
message(STATUS "Configuring Qucs Project: VERSION ${QUCS_VERSION}")

ADD_SUBDIRECTORY( qucs )
ADD_SUBDIRECTORY( qucs-core )

