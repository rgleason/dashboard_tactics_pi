<a href="docs/developers/README.md"><img src="docs/developers/img/message.svg" /></a><br />
# _DashT_ - an OpenCPN plug-in with Dashboard, Tactics, Engine/Energy dials and Time Series DB functions with history line graphs

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/23e5625c7b5a4aa4a3b3696b5a7795d2)](https://app.codacy.com/app/petri38-github/dashboard_tactics_pi?utm_source=github.com&utm_medium=referral&utm_content=canne/dashboard_tactics_pi&utm_campaign=Badge_Grade_Settings)

_dashboard_tactics_pi_ in your OpenCPN plug-in catalog.

## Introduction

_DashT_ [v1.5.11](https://github.com/canne/dashboard_tactics_pi/releases/tag/v1.5.11) was a forerunner in diversifying the data connectivity of a OpenCPN plug-in to enable direct network connections to resources available in a modern boat's computer and network infrastructure. This version, of which you can find installation packages for early adopters in [Releases](https://github.com/canne/dashboard_tactics_pi/releases) continues this philosophy and presents new features by collecting timestamped open data and making it available to external applications and data retrieval systems.

The next OpenCPN version, currently in 5.1beta is embracing the open marine data format [Signal K](https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:supplementary_software:signalk). This hopefully encourage you to consider to install the fast, ubiquitous and reliable Signal K data format source in your boat's infrastructure: [Signal K server node](https://github.com/SignalK/signalk-server-node). As before, it is fully supported by _DashT_ as an alternative and direct data source, allowing the creation of enhancements which are not present in the traditional OpenCPN Dashboard:

* OpenCPN's Dashboard integrated

* [Tactics and performance functions intergrated](docs/Tactics.md)

  * No need to swap between the two plug-ins!
  
>Dashboard and Tactics instruments do not require Signal K data but they work also with NMEA-0183 data from OpenCPN alone

* [Signal K data input streaming](https://canne.github.io/dashboard_tactics_pi/docs/signalk/SignalKInputStreamerUsage.html)

  * Direct connection to the delta channel of a Signal K server node providing shortest possible path for:

    * Maximum volume of data ;
    * Lowest possible latency ;
    * Time stamps at source ;
    * Access to Engine, Energy and Status data from NMEA-2000, Bluetooth LE, GPIO over a single interface.

* [Engine and Energy dials](https://canne.github.io/dashboard_tactics_pi/docs/webview/README.html)

  * Provides Signal K engine and energy data on dials built using latest web techniques but fully integrated in OpenCPN Dashboard
  * Best explained by this [short video](https://vimeo.com/391601955) (on a very small display)

* [Influx DB 2.0 ouput streaming and export](https://canne.github.io/dashboard_tactics_pi/docs/influxdb/InfluxDBStreamer.html)

  * Time series based databases are essential with the data volumes and rates produced by Signal K enabled servers
  
  * Real-time storage and retrieval allows long-term monitoring and historical data browsing using external tools such as Grafana
  
  * Short-term historical data line graphs provided and integrated in OpenCPN Dashboard for race time or performance run real-time monitoring of key parameters
  
  * An all-received-data containing file can be registered to feed, off-line or after race InfluxDB v2.0 time series database
  
    * Enhances post-race analysis and off-line polar calculations with high sampling rate and accurately timestamped data

* [Comprehensive documentation](https://canne.github.io/#DashT%20-%20plug-in%20for%20OpenCPN)

  * Detailed User's Manuals
  * Developer's information with performance analysis and debugging tips

## Installation

See the [Releases](https://github.com/canne/dashboard_tactics_pi/releases) pages: pre-build packages for platforms Windows, Mac, Linux Ubuntu/Debian, Raspberry Pi 4.

## Configuration

Please refer to User's Guides accessible from the [documentation pages](https://canne.github.io/#DashT%20-%20plug-in%20for%20OpenCPN)

## Compiling

* git clone git://github.com/canne/dashboard_tactics_pi.git

### Windows build

>This build is tested with Visual Studio 2019 on Windows 10

Please get first the library file "_opencpn.lib_". It is located in the build directory of [OpenCPN](https://github.com/OpenCPN/OpenCPN) after having built it; or your can search it from the various processes of CI (continuous integration) of this plug-in. Or try [this one](https://github.com/canne/dashboard_tactics_pi/releases/download/0.0.1/opencpn.lib).

>Starting from v2.0 this plug-in has moved in Windows entirely to use [vcpkg](https://github.com/Microsoft/vcpkg) for dependencies - hence the directive below. As usual the most annoying part is the wxWidgets package which cannot be STL-enabled, unfortunately in OpenCPN - in DashT it can: to help in this dilemma, there is a non-STL port with CMake find-script available [here](https://github.com/canne/dashboard_tactics_pi/releases/download/0.0.1/vcpkgOpenCPNwxWidgetsNoSTLport.7z). Of course, one can install wxWidgets manually in C: - by no means DashT depends on its location; _vcpkg_ is only for build process on Windows to provide libraries readily available on a Linux system:

* cd build
* cp ..\..\mydirectory\opencpn.lib .
* cmake -G "Visual Studio 16 2019" -A Win32 -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake ..
* cmake --build . --target package --config release

### Debian/Linux style build

* cd build
* cmake ..
* make
* make package
* sudo gdebi packagename-version-etc.deb

### License

The project is licensed under the terms of the GPL v3. The networked parts of the project are derived from and licensed under MIT license.
