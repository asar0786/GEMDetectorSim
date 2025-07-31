cmake_minimum_required(VERSION 3.10)
project(GEMDetectorSim)

# Find packages
find_package(Geant4 REQUIRED ui_all vis_all)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)

# Include Geant4 settings
include(${Geant4_USE_FILE})

# Headers
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIRS})

# Define executable
add_executable(GEMDetector
    GEMDetector.cc
    src/GEMDetectorConstruction.cc
    src/GEMSensitiveDetector.cc
    src/PrimaryGeneratorAction.cc
    src/RootOutputManager.cc
    src/HoneycombBuilder.cc
    src/Materials.cc
    src/DipoleMagnet.cc
    src/GEMDetector.cc
)

# Link libraries
target_link_libraries(GEMDetector ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

