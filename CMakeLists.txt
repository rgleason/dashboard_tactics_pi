##---------------------------------------------------------------------------
## Author:  Petri Mäkijärvi   [Canne]     License: GNU GPVv3
## ***************************************************************************
## *   Copyright (C) 2010 by David S. Register                               *
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
## *   Free Software Foundation, Inc.,                                       *
## *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
## ***************************************************************************
# Frontend2 Author: Jon Gough  
# Frontend2: v1.0.180
# ---------------------------------------------------------------------------
## ----- When changing this file do NOT change the order in which sections occur        ----- ##
## ----- Changes should only be made between the section blocks that identify where     ----- ##
## ----- these changes should be. The whole configuration process relies on this        ----- ##
## ----- sequence to be successful                                                      ----- ##
##
##----- Modify section below to include all the details for your plugin ----- ##
##

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
   set(VERSION_MINOR 6)
   set(VERSION_PATCH 8)
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
   set(VERSION_TWEAK 0)   
   set(SHORT_DESCRIPTION "Dashboard")
   set(LONG_DESCRIPTION "Dashboard PlugIn with Tactics for OpenCPN  Provides navigation instruments enhanced with performance functions and alternative input/output functions.")
ENDIF (TACTICS_INSTRUMENTS)
set(CPACK_PACKAGE_CONTACT "Rick Gleason")
set(VERSION_DATE "08/24/2020")
set(OCPN_MIN_VERSION "ov50")
set(OCPN_API_VERSION_MAJOR 1)
set(OCPN_API_VERSION_MINOR 16)

set(PARENT "opencpn")

# SET (CMAKE_BUILD_TYPE Debug)

IF (TACTICS_INSTRUMENTS)
  ADD_DEFINITIONS(-D_INCLUDE_TACTICS_PI_)
ENDIF (TACTICS_INSTRUMENTS)

# The next line allows setup of a local webserver with git for testing purposes.
# The default is shown below, but it is done in cmake/pluginconfigure.cmake now.
# This default is needed to build locally and to build flatpak.
#set(GIT_REPOSITORY_SERVER "github.com")

#No longer needed, queried.
#set(GIT_USER "rgleason")
#set(GIT_REPOSITORY_NAME "testplugin_pi")

# Specifies Cloudsmith upload repository suffix for each catalog
set(PROD "prod")    #Standard Repos
set(BETA "beta")    #Standard Repos
set(ALPHA "alpha")  #Standard Repos

#set(CLOUDSMITH_USER "rick-gleason")
set(CLOUDSMITH_USER "opencpn")
# Set if yourCloudsmith Base Repository name does not match your Git Repository name.

# Set if yourCloudsmith Base Repository name does not match your Git Repository name.
# set(CLOUDSMITH_BASE_REPOSITORY "opencpn-plugins") #Rick's customization
set (CLOUDSMITH_BASE_REPOSITORY "dashboardtactics")  #without the pi 

set(XML_INFO_URL "https://opencpn.org/OpenCPN/plugins/dashboardtactics.html")
set(XML_SUMMARY ${SHORT_DESCRIPTION})
set(XML_DESCRIPTION ${LONG_DESCRIPTION})

##
## ----- Modify section above to include all the details for your plugin ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

option(PLUGIN_USE_SVG "Use SVG graphics" ON)

set(CMAKE_CXX_STANDARD 11)

# Use local version of GLU library
# set(USE_LOCAL_GLU TRUE)
# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")
# Don't use local version of GLU library
set(USE_LOCAL_GLU FALSE)
option(USE_GL "Enable OpenGL support" ON)
message(STATUS "${CMLOC}USE_GL: ${USE_GL}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")

##
## ----- The statements below are used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
##

project(${PACKAGE})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

## =====================================
include("PluginSetup")
## =====================================

## ----- The above is used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

set(CMAKE_VERBOSE_MAKEFILE "Activate verbose mode for make files" ON)

option(Plugin_CXX11 "Use c++11" OFF)

##
## ----- Modify section above if there are special requirements for the plugin ----- ##
## ----- do not change next section - needed to configure build process ----- ##
##
## =====================================
include("PluginConfigure")
## =====================================
##
## ----- do not change section above - needed to configure build process ----- ##
# ----- Change below to match project requirements for source, headers, etc. ----- ##
#

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
    src/wxJSON/jsonval.cpp
    src/wxJSON/jsonreader.cpp
    include/wx/json_defs.h
    include/wx/jsonreader.h
    include/wx/jsonval.h
    include/wx/jsonwriter.h
    
)

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
#	  api-16/ocpn_plugin.h
	  libs/ocpn-api/ocpn_plugin.h
  )

ENDIF (TACTICS_INSTRUMENTS)

SET(SRC_NMEA0183
    src/nmea0183/LatLong.hpp
    src/nmea0183/latlong.cpp
    src/nmea0183/long.cpp
    src/nmea0183/nmea0183.cpp
    src/nmea0183/nmea0183.hpp
    src/nmea0183/Response.hpp
    src/nmea0183/response.cpp
    src/nmea0183/RMB.hpp
    src/nmea0183/rmb.cpp
    src/nmea0183/Sentence.hpp
    src/nmea0183/sentence.cpp
    src/nmea0183/talkerid.cpp
    src/nmea0183/RMC.HPP
    src/nmea0183/rmc.cpp
    src/nmea0183/hexvalue.cpp
    src/nmea0183/lat.cpp
    src/nmea0183/expid.cpp
    src/nmea0183/wpl.hpp
    src/nmea0183/wpl.cpp
    src/nmea0183/rte.hpp
    src/nmea0183/rte.cpp
    src/nmea0183/hdt.hpp
    src/nmea0183/hdt.cpp
    src/nmea0183/hdg.hpp
    src/nmea0183/hdg.cpp
    src/nmea0183/hdm.hpp
    src/nmea0183/hdm.cpp
    src/nmea0183/gll.hpp
    src/nmea0183/gll.cpp
    src/nmea0183/vtg.hpp
    src/nmea0183/vtg.cpp
    src/nmea0183/gga.hpp
    src/nmea0183/gga.cpp
    src/nmea0183/gsv.hpp
    src/nmea0183/gsv.cpp
    src/nmea0183/dbt.cpp
    src/nmea0183/dbt.hpp
    src/nmea0183/dpt.cpp
    src/nmea0183/dpt.hpp
    src/nmea0183/mtw.cpp
    src/nmea0183/mtw.hpp
    src/nmea0183/mwv.cpp
    src/nmea0183/mwv.hpp
    src/nmea0183/vhw.hpp
    src/nmea0183/vhw.cpp
    src/nmea0183/vwr.cpp
    src/nmea0183/vwr.hpp
    src/nmea0183/xte.cpp
    src/nmea0183/xte.hpp
    src/nmea0183/zda.cpp
    src/nmea0183/zda.hpp
    src/nmea0183/rsa.hpp
    src/nmea0183/rsa.cpp
    src/nmea0183/SatInfo.h
    src/nmea0183/mwd.cpp
    src/nmea0183/mwd.hpp
    src/nmea0183/vwt.cpp
    src/nmea0183/vwt.hpp
    src/nmea0183/mta.cpp
    src/nmea0183/mta.hpp
    src/nmea0183/vlw.cpp
    src/nmea0183/vlw.hpp
    src/nmea0183/mda.cpp
    src/nmea0183/mda.hpp
    src/nmea0183/xdr.cpp
    src/nmea0183/xdr.hpp
)

## ===================================================================================
IF (TACTICS_INSTRUMENTS)
  SET(SRCS ${SRC_DASHBOARD} ${SRC_TACTICS} ${SRC_NMEA0183})
ELSE (TACTICS_INSTRUMENTS)
  SET(SRCS ${SRC_DASHBOARD} ${SRC_NMEA0183})
ENDIF (TACTICS_INSTRUMENTS)
## ===================================================================================


# For Json Validation
#set(EXTSRC
#)

# For Json Validation and Odapi
#set(EXTINCLUDE
#    extinclude/ODAPI.h
#    extinclude/ODJSONSchemas.h
#)

# Above From Dashboardtactics
#------------------------------------
add_definitions(-DPLUGIN_USE_SVG)

# For JSON validation

set(EXTINCLUDE_DIR ${EXTINCLUDE_DIR} extinclude libs/ocpn-api/)

#
# ----- If using JSON validation in plugin section below is needed ----- ##
#

# Not doing JSON Validation here.

#
# ----- If using JSON validation in plugin section above is needed ----- ##
#
##
# Modify/configure 'Include' Directories below for the Plugin
##


#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/${EXTINCLUDE_DIR})
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/extinclude)


#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpninclude)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpnsrc/glu/include)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/${EXTINCLUDE_DIR})
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/GL)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/ocpn-api)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/wxJSON/include)

##
## Statement below is required to collect all the set( headers and SRCS  - Adjust as required
## This add_library  can be used in conditionals below.
##

target_sources(${PACKAGE_NAME} PUBLIC ${SRCS} ${HDRS} )

add_definitions(-DTIXML_USE_STL)


# All plugins useing JSON to passing messages should use the validator, which ignores non-conformant messages. 
# Not doing JSON Validation here


##
## ----- Change above to match project requirements ----- ##
## ----- do not change next section - needed to configure build process ----- ##
##
## =====================================

# Needed for android builds
if(QT_ANDROID)
    include_directories(BEFORE ${qt_android_include})
endif(QT_ANDROID)

# Needed for all builds
include("PluginInstall")
include("PluginLocalization")
include("PluginPackage")
## =====================================
##
## ----- do not change section above - needed to configure build process ----- ##
##