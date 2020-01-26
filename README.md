<a href="docs/developers/README.md"><img src="docs/developers/img/message.svg" /></a><br />
# Dashboard/Tactics Plugin for OpenCPN

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/23e5625c7b5a4aa4a3b3696b5a7795d2)](https://app.codacy.com/app/petri38-github/dashboard_tactics_pi?utm_source=github.com&utm_medium=referral&utm_content=canne/dashboard_tactics_pi&utm_campaign=Badge_Grade_Settings)

dashboard_pi with integrated tactics_pi performance enhancements.

## Introduction

If you are using both the built-in Dashboard and the Tactics plug-ins in OpenCPN v5 or greater, you may want to have but just one plug-in instead of two. All available instruments will be in one place. Even if you are not interested in Tactics functions, Dashboard has also improved and has new functions, like export/import and input/output streaming. This version comes with more resources allowing it to be expandandable. It can host even more (!) instruments, also those talking NMEA-2000, such as engine monitoring, mast rotation, etc.

Tactics functions provide information to help you sail better - ranging from true wind data to advanced functions which allows a selection of the best head sail for the next tack. Please read further about it and other advanced features here:
* [Tactics and performance functions](docs/Tactics.md)
* [Influx DB 2.0 ouput streaming and export](https://canne.github.io/dashboard_tactics_pi/docs/influxdb/InfluxDBStreamer.html)
* [Signal K data input streaming](https://canne.github.io/dashboard_tactics_pi/docs/signalk/SignalKInputStreamerUsage.html)
* [all docs](docs/README.md) - [(HTML/PDF formats)](https://canne.github.io/#:%5B%5BDashboard%2FTactics%20Plugin%20for%20OpenCPN%5D%5D)

## Installation

You can start without a polar file and just use the basic functions to start with. Go to [dashboard_tactics_pi Releases pages](https://github.com/canne/dashboard_tactics_pi/releases) and grab an installation file which matches your platform - OpenCPN v5 on Windows, Linux or Mac is supported.

## Configuration

Although not mandatory, it is recommended to disable the Dashboard coming with the OpenCPN (this plug-in is using the same parameters), standalone Tactics plug-in and WMM_pi plug-in (unless you do not get the magnetic variation from your instruments).

If you are a user of tactics_pi, its settings are imported at first startup of this plug-in. Tactics plug-in's settings are not modified so that you can switch back to it. The original Dashboard will also work with the modified parameters - if you have used Tactics instruments or enhanced instruments not in the original Dashboard, they would appear empty and you need to delete them manually and restart OpenCPN to get back to Dashboard-only configuration. For this reason, saving of your ini- or config-file is recommended for this reason if you just want to try out this plug-in.

## Compiling

* git clone git://github.com/canne/dashboard_tactics_pi.git

Under windows, you must find the file "opencpn.lib" (Visual Studio) located in the build directory after compiling opencpn. 
This file must be copied to the plugins build directory.

### Windows build

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

The code is licensed under the terms of the GPL v3.
