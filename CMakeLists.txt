##---------------------------------------------------------------------------
## Author:  Petri Mäkijärvi   [Canne]     License: GNU GPVv3
## ***************************************************************************
## *   Copyright (C) 2010 by Petri Mäkijärvi                             *
## *   License:     GPLv3
## *   This program is free software; you can redistribute it and/or modify  *
## *   it under the terms of the GNU General Public License as published by  *
## *   the Free Software Foundation; either version 2 of the License, or     *
## *   (at your option) any later version.                                   *
## *                                                                         *
## *   This program is distributed in the hope that it will be useful,       *
## *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
## *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
## *   GNU General Public License for more details.                          *
## *                                                                         *
## *   You should have received a copy of the GNU General Public License     *
## *   along with this program; if not, write to the                         *
## *   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx,                                       *
## *   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                       *
#-----------------------------------------------------------------------------
# Frontend2 Author: Jon Gough 
# Testplugn(TP) v1.0.254.0 09/09/2023 DD/MM/YYYY format
# buildCI:yes useODAPI:no validateJSON:no
# ---------------------------------------------------------------------------
## -- When changing this file do NOT change the order of the sections.    -- ##
## -- Make changes only between the section blocks identified             -- ##
## -- The whole configuration process relies on this sequence for success.-- ##
##
## -- Modify section below to include all the details for your plugin.    -- ##

set(CMLOC "CMakeLists: ")

# define minimum cmake version
cmake_minimum_required(VERSION 3.5.1)
if(COMMAND cmake_policy)
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif(POLICY CMP0043)
    cmake_policy(SET CMP0048 NEW)
# Need cmake 3.13 to implement this, but raspbian builds are at 3.7.2 and bionic at 3.10 and xenial builds at 3.5.1
#    cmake_policy(SET CMP0076 NEW)
    if(POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
    endif(POLICY CMP0077)
endif(COMMAND cmake_policy)

# ON = performance instruments, 
# OFF = exclude all tactics_pi originated code from compilation _and_ build
SET(TACTICS_INSTRUMENTS ON)

# define plugin name, owner and versions
IF (TACTICS_INSTRUMENTS)
   set(PACKAGE "dashboard_tactics_pi")
   set(VERBOSE_NAME "Dashboard_Tactics")
   set(COMMON_NAME "DashboardTactics")  # Search "getcommonname"in <plugin_pi>.cpp
   set(TITLE_NAME "DashboardTactics")
   set(PACKAGE_CONTACT "Rick Gleason")
   set(VERSION_MAJOR 1)
   set(VERSION_MINOR 7)
   set(VERSION_PATCH 5)
   set(VERSION_TWEAK 0)
   set(SHORT_DESCRIPTION "Dashboard and Tactics")
   set(LONG_DESCRIPTION "Dashboard PlugIn with Tactics for OpenCPN  Provides navigation instruments enhanced with performance functions and alternative input/output functions.") 
ELSE (TACTICS_INSTRUMENTS)
   set(PACKAGE "dashboard_pi")
   set(VERBOSE_NAME "Dashboard")
   set(COMMON_NAME "Dashboard") # Search "getcommonname"in <plugin_pi>.cpp
   set(TITLE_NAME "Dashboard")
   set(PACKAGE_CONTACT "Rick Gleason")
   set(VERSION_MAJOR 1)
   set(VERSION_MINOR 5)
   set(VERSION_PATCH 12)
   set(VERSION_TWEAK 1)   
   set(SHORT_DESCRIPTION "Dashboard")
   set(LONG_DESCRIPTION "Dashboard PlugIn with Tactics for OpenCPN  Provides navigation instruments enhanced with performance functions and alternative input/output functions.")
ENDIF (TACTICS_INSTRUMENTS)
set(CPACK_PACKAGE_CONTACT "Rick Gleason")
set(VERSION_DATE "09/08/2023") # DD/MM/YYYY format
set(OCPN_MIN_VERSION "ov58")
set(OCPN_API_VERSION_MAJOR "1")
set(OCPN_API_VERSION_MINOR "16")
set(TP_COMMENT "  * Release using CI")

set(PARENT "opencpn")


IF (TACTICS_INSTRUMENTS)
  ADD_DEFINITIONS(-D_INCLUDE_TACTICS_PI_)
ENDIF (TACTICS_INSTRUMENTS)

#Use to set up a local webserver with git installed.
#Default is github.com. Now set in pluginconfigure.cmake
#set(GIT_REPOSITORY_SERVER "github.com")

## Specifies Cloudsmith upload repository suffix for each catalog
set(PROD "prod")   #Standard Repos
set(BETA "beta")   #Standard Repos
set(ALPHA "alpha") #Standard Repos

# Set if  Cloudsmith Base Repository name does not match your Git Repository name.
#set(CLOUDSMITH_BASE_REPOSITORY "${GIT_REPOSITORY_NAME}")
#set(CLOUDSMITH_BASE_REPOSITORY "opencpn-plugins")  #Ricks's customization
set (CLOUDSMITH_BASE_REPOSITORY "dashboardtactics")  #without the pi 

# Set the following to use OpenCPN cloudsmith repositories instead of own repositories
set(CLOUDSMITH_USER "opencpn")
#set(CLOUDSMITH_USER "rick-gleason")

set(XML_INFO_URL "https://opencpn.org/OpenCPN/plugins/dashboardtactics.html")
set(XML_SUMMARY ${SHORT_DESCRIPTION})
set(XML_DESCRIPTION ${LONG_DESCRIPTION})

## ----- Modify section above to include all the details for your plugin ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##

option(PLUGIN_USE_SVG "Use SVG graphics" ON)

set(CMAKE_CXX_STANDARD 11)

# Use local version of GLU library
# requires libs/glu directory
set(USE_LOCAL_GLU FALSE) # Don't use local version of GLU library
# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")   #not operational

option(USE_GL "Enable OpenGL support" ON)
message(STATUS "${CMLOC}USE_GL: ${USE_GL}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")

## ----- Statements below used to setup standard variables required by the CMAKE process -- Do not remove - ##

project(${PACKAGE})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#========================================================
include("PluginSetup")
#========================================================
## ----- Above used to setup standard variables required by CMAKE process - Do not remove ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##

set(CMAKE_VERBOSE_MAKEFILE "Activate verbose mode for make files" ON)

option(Plugin_CXX11 "Use c++11" OFF)

## ----- Modify section above if there are special requirements for the plugin ----- ##
## ----- do not change next section - needed to configure build process ----- ##
#===========================================================
include("PluginConfigure")
#===========================================================
## ----- do not change section above - needed to configure build process ----- ##
## ----- Change below to match project requirements for source, headers, etc. ----- ##

add_definitions(-DUSE_S57)

SET(SRC_DASHBOARD
    src/dashboard_pi.h
    src/dashboard_pi.cpp
    src/instrument.cpp
    src/instrument.h
    src/iirfilter.h
    src/iirfilter.cpp
    src/compass.cpp
    src/compass.h
    src/dial.cpp
    src/dial.h
    src/gps.cpp
    src/gps.h
    src/rudder_angle.cpp
    src/rudder_angle.h
    src/speedometer.cpp
    src/speedometer.h
    src/wind.cpp
    src/wind.h
    src/depth.cpp
    src/depth.h
    src/clock.cpp
    src/clock.h
    src/icons.cpp
    src/icons.h
    src/wind_history.cpp
    src/wind_history.h
    src/baro_history.cpp
    src/baro_history.h
    src/from_ownship.cpp
    src/from_ownship.h
)

set(LIBSSRC 
    libs/wxJSON/src/jsonreader.cpp
    libs/wxJSON/src/jsonval.cpp
    libs/wxJSON/src/jsonwriter.cpp)

set(LIBHDRS
    libs/GL/gl.h
    libs/GL/gl_private.h
    libs/GL/glext.h
    libs/GL/glu.h
    libs/wxJSON/include/json_defs.h
    libs/wxJSON/include/jsonreader.h
    libs/wxJSON/include/jsonval.h
    libs/wxJSON/include/jsonwriter.h)


IF (TACTICS_INSTRUMENTS)
  SET(SRC_TACTICS
      src/tactics_pi.h
      src/tactics_pi.cpp
      src/performance.h
      src/performance.cpp
      src/streamin-sk.h
      src/streamin-sk.cpp
      src/streamout.h
      src/streamout.cpp
      src/plugin_ids.h
      src/bearingcompass.h
      src/bearingcompass.cpp
      src/polarcompass.h
      src/polarcompass.cpp
      src/avg_wind.h
      src/avg_wind.cpp
  )

ENDIF (TACTICS_INSTRUMENTS)

SET(SRC_NMEA0183
    libs/nmea0183/LatLong.hpp
    libs/nmea0183/latlong.cpp
    libs/nmea0183/long.cpp
    libs/nmea0183/nmea0183.cpp
    libs/nmea0183/nmea0183.hpp
    libs/nmea0183/Response.hpp
    libs/nmea0183/response.cpp
    libs/nmea0183/RMB.hpp
    libs/nmea0183/rmb.cpp
    libs/nmea0183/Sentence.hpp
    libs/nmea0183/sentence.cpp
    libs/nmea0183/talkerid.cpp
    libs/nmea0183/RMC.HPP
    libs/nmea0183/rmc.cpp
    libs/nmea0183/hexvalue.cpp
    libs/nmea0183/lat.cpp
    libs/nmea0183/expid.cpp
    libs/nmea0183/wpl.hpp
    libs/nmea0183/wpl.cpp
    libs/nmea0183/rte.hpp
    libs/nmea0183/rte.cpp
    libs/nmea0183/hdt.hpp
    libs/nmea0183/hdt.cpp
    libs/nmea0183/hdg.hpp
    libs/nmea0183/hdg.cpp
    libs/nmea0183/hdm.hpp
    libs/nmea0183/hdm.cpp
    libs/nmea0183/gll.hpp
    libs/nmea0183/gll.cpp
    libs/nmea0183/vtg.hpp
    libs/nmea0183/vtg.cpp
    libs/nmea0183/gga.hpp
    libs/nmea0183/gga.cpp
    libs/nmea0183/gsv.hpp
    libs/nmea0183/gsv.cpp
    libs/nmea0183/dbt.cpp
    libs/nmea0183/dbt.hpp
    libs/nmea0183/dpt.cpp
    libs/nmea0183/dpt.hpp
    libs/nmea0183/mtw.cpp
    libs/nmea0183/mtw.hpp
    libs/nmea0183/mwv.cpp
    libs/nmea0183/mwv.hpp
    libs/nmea0183/vhw.hpp
    libs/nmea0183/vhw.cpp
    libs/nmea0183/vwr.cpp
    libs/nmea0183/vwr.hpp
    libs/nmea0183/xte.cpp
    libs/nmea0183/xte.hpp
    libs/nmea0183/zda.cpp
    libs/nmea0183/zda.hpp
    libs/nmea0183/rsa.hpp
    libs/nmea0183/rsa.cpp
    libs/nmea0183/SatInfo.h
    libs/nmea0183/mwd.cpp
    libs/nmea0183/mwd.hpp
    libs/nmea0183/vwt.cpp
    libs/nmea0183/vwt.hpp
    libs/nmea0183/mta.cpp
    libs/nmea0183/mta.hpp
    libs/nmea0183/vlw.cpp
    libs/nmea0183/vlw.hpp
    libs/nmea0183/mda.cpp
    libs/nmea0183/mda.hpp
    libs/nmea0183/xdr.cpp
    libs/nmea0183/xdr.hpp
)


add_definitions(-DPLUGIN_USE_SVG)

## ----- Section above - Add your project SET(SRCS and SET(HDRS  etc..
## ----- If using JSON validation in plugin section below is needed ----- ##

# Do not need Json Schema Validation

## ----- If using JSON validation in plugin section above is needed ----- ##


##================================================================================
## Change as required to collect all the set( headers and SRCS
## This add_library  can be used in conditionals below.
## ===================================================================================
IF (TACTICS_INSTRUMENTS)
  SET(SRC ${SRC_DASHBOARD} ${SRC_TACTICS} ${LIBSSRC} ${SRC_NMEA0183})
ELSE (TACTICS_INSTRUMENTS)
  SET(SRC ${SRC_DASHBOARD} ${LIBSSRC} ${SRC_NMEA0183})
ENDIF (TACTICS_INSTRUMENTS)

##=============================================================
## Modify/configure 'Include' Directories below for the Plugin
##=============================================================

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/GL)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/glu/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/ocpn-api)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/wxJSON/include)

#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/jsoncpp)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/nmea0183/src)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/nmea0183)

#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/pluginDC/include)


## Change target_sources to match your project needs.
##=================================================
target_sources(${PACKAGE_NAME} PUBLIC ${SRC} )
##=================================================

if(NOT OCPN_FLATPAK_CONFIG)
    # Build environment not available when flatpak is being configured so following statements will not work
    message(STATUS "${CMLOC}Adding target link libraries to ${PACKAGE_NAME}")

    add_subdirectory(libs/tinyxml)
    target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)

#    add_subdirectory(libs/nmea0183)
#    target_link_libraries(${PACKAGE_NAME} ocpn::nmea0183)	
    
endif(NOT OCPN_FLATPAK_CONFIG)

add_definitions(-DTIXML_USE_STL)

##
## ----- If using JSON validation section below is needed ----- ##
##

# Do not need Json Schema Validation

##
## ----- Do not change next section - needed to configure build process ----- ##
##
##------- Change below to match project requirements --------------

# Needed for android builds
if(QT_ANDROID)
    include_directories(BEFORE ${qt_android_include})
endif(QT_ANDROID)

## Needed for all builds Do not change - needed to build app --- ##
#========================================================
include("PluginInstall")
include("PluginLocalization")
include("PluginPackage")
#========================================================
## ----- Do not change section above - needed to configure build process ----- ##

message(STATUS "${CMLOC}include directories: ")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "${CMLOC}dir='${dir}'")
endforeach()
