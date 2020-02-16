<a href="docs/developers/README.md"><img src="docs/developers/img/message.svg" /></a><br />
# DashT - and OpenCPN plug-in with Dashboard, Tactics, Engine/Energy dials and Time Series DB functions

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/23e5625c7b5a4aa4a3b3696b5a7795d2)](https://app.codacy.com/app/petri38-github/dashboard_tactics_pi?utm_source=github.com&utm_medium=referral&utm_content=canne/dashboard_tactics_pi&utm_campaign=Badge_Grade_Settings)

_dashboard_tactics_pi_ in your OpenCPN plug-in catalog.

## Introduction

_DashT_ [v1.5.11](https://github.com/canne/dashboard_tactics_pi/releases/tag/v1.5.11) was a forerunner in diversifying the data connectivity of a OpenCPN plug-in to enable direct network connections to resources available in a modern boat's computer and network infrastructure.

With the arrival of the next OpenCPN version it embraces [Signal K data format](https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:supplementary_software:signalk). This is excellent news since this is hopefully encouraging you to enable a fast and reliable Signal K data format source in your boat's infrastructure, [Signal K server node](https://github.com/SignalK/signalk-server-node). It has been supported since day zero by _DashT_ allowing it to present enhancements not present in the traditional OpenCPN Dashboard:

* OpenCPN's Dashboard integrated
  * You do not need to swap between the two!
  * No need for Signal K for these instruments, NMEA-0183 is enough

* [Tactics and performance functions intergrated](docs/Tactics.md)
  * Also integrated, no need to swap
  * Works fine with NMEA-0183 but will profit from the faster, timestamped wind data provided by Signal K

* [Engine and Energy dials](https://canne.github.io/dashboard_tactics_pi/docs/webview/README.html)
  * Provides Signal K engine and energy data on latest web technique instruments integrated in OpenCPN Dashboard
  * Best explained by this [short video](https://vimeo.com/391601955)

* [Influx DB 2.0 ouput streaming and export](https://canne.github.io/dashboard_tactics_pi/docs/influxdb/InfluxDBStreamer.html)

  * Time series based databases are essential with the data volumes and rates of Signal K enabled servers
  
  * Real-time storage and retrieval allows monitoring and historical data browsing using tools such as Grafana
  * An all-received-data dump-file can be registered to feed off-line InfluxDB v2.0 time series database
    * Enhances post-race analysis and off-line polar calculations with high-frequency sampled data

* [Signal K data input streaming](https://canne.github.io/dashboard_tactics_pi/docs/signalk/SignalKInputStreamerUsage.html)
  * Direct connection to the delta channel of a Signal K server node providing shortest possible path for:
    * Maximum volume of data ;
    * Lowest possible latency ;
    * Time stamps of all data at source ;
    * Access to Engine, Energy and Status data from NMEA-2000, Bluetooth LE, GPIO over a single interfce.

* [Comprehensive documentation](https://canne.github.io/#:%5B%5BDashboard%2FTactics%20Plugin%20for%20OpenCPN%5D%5D)
  * Detailed User's Manuals
  * Developer's information with performance analysis and debugging tips

## Installation

See the [Releases](https://github.com/canne/dashboard_tactics_pi/releases) pages: pre-build packages for platforms Windows, Mac, Linux Ubununtu/Debian, Raspberry Pi 4.

## Configuration

Please refer to User's Guides accessible from the [documentation pages](https://canne.github.io/#:%5B%5BDashboard%2FTactics%20Plugin%20for%20OpenCPN%5D%5D)

## Compiling

* git clone git://github.com/canne/dashboard_tactics_pi.git

### Windows build

>Under Windows, please get first the library file "_opencpn.lib_" (Visual Studio). It is located in the build directory of [OpenCPN](https://github.com/OpenCPN/OpenCPN) after having built it; or your can search it from the various processes of CI (continuous integration) of this plug-in. Copy the library file matching the target OpenCPN version into the build directory before launching the following:

* cd build
* cmake  -T v140_xp ..
* cmake --build . --target package --config release

### Unix style build

* cd build
* cmake ..
* make
* make package
* sudo make install

### License

The project is licensed under the terms of the GPL v3. The networked parts of the project are derived from and licensed under MIT license.
