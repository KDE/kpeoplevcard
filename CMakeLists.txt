# SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@kde.org>
# SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(KPeopleVCard VERSION 0.2)

set(QT_MIN_VERSION 5.15)
set(KF_MIN_VERSION 5.83)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(WriteBasicConfigVersionFile)
include(CheckFunctionExists)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

include(ECMAddTests)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n People Contacts)

add_subdirectory(src)
add_subdirectory(autotests)

ki18n_install(po)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF${QT_MAJOR_VERSION}PeopleVCard")
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KFPeopleVCardConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF${QT_MAJOR_VERSION}PeopleVCardConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF${QT_MAJOR_VERSION}PeopleVCardConfig.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

file(GLOB_RECURSE SOURCE_FILES *.cpp *.h)
kde_clang_format(${SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
