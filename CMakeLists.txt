cmake_minimum_required(VERSION 3.0)
project(KPeopleVCard)

find_package(ECM 1.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMSetupVersion)
include(ECMAddTests)

include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(WriteBasicConfigVersionFile)
include(CheckFunctionExists)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(Qt5 "5.2" REQUIRED COMPONENTS Widgets)
find_package(KF5 "5.8" REQUIRED COMPONENTS CoreAddons People)

find_package(KF5Contacts REQUIRED)

add_subdirectory(src)
add_subdirectory(autotests)

set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5PeopleVCard")
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5PeopleVCardConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleVCardConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleVCardConfig.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )
