cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(Tutorial)

find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cc
    ${PROJECT_SOURCE_DIR}/src/ActionInitialization.cc
    ${PROJECT_SOURCE_DIR}/src/DetectorConstruction.cc
    ${PROJECT_SOURCE_DIR}/src/DetectorSD.cc
    ${PROJECT_SOURCE_DIR}/src/PrimaryGeneratorAction.cc
    ${PROJECT_SOURCE_DIR}/src/RunAction.cc
    ${PROJECT_SOURCE_DIR}/src/SimulationConfig.cc # <-- Added here
)

file(GLOB MACRO_FILES ${PROJECT_SOURCE_DIR}/macros/*.mac)
file(COPY ${MACRO_FILES} DESTINATION ${CMAKE_BINARY_DIR})

add_executable(sim ${SOURCES})
target_link_libraries(sim ${Geant4_LIBRARIES})