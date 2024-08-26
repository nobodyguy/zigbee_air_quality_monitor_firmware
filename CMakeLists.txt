#
# Copyright (c) 2024 Jan Gnip
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

if ("${BOARD}" STREQUAL "zigbee")
  set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
endif()

if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/configuration/${BOARD}")
  message(FATAL_ERROR
          "Board ${BOARD} is not supported.\n"
          "Please make sure board specific configuration files are added to "
          "${CMAKE_CURRENT_SOURCE_DIR}/configuration/${BOARD}")
endif()

################################################################################

# The application uses the configuration/<board> scheme for configuration files.
set(APPLICATION_CONFIG_DIR "configuration/\${BOARD}")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ZigbeeAirQualityMonitor)

################################################################################

FILE(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
target_sources(app PRIVATE ${app_sources})

target_include_directories(app PRIVATE include)
