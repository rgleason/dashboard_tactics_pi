# Dashboard/Tactics Plugin for OpenCPN

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/23e5625c7b5a4aa4a3b3696b5a7795d2)](https://app.codacy.com/app/petri38-github/dashboard_tactics_pi?utm_source=github.com&utm_medium=referral&utm_content=canne/dashboard_tactics_pi&utm_campaign=Badge_Grade_Settings)

dashboard_pi with integrated tactics_pi performance enhancements.

## Introduction

If you are using both built-in Dashboard and the Tactics, you may want to have but one plug-in instead of two to have all available instruments available in one place.

Tactics functions provide information to help you sail better - ranging from true wind data to advanced functions which allows a selection of the best head sail for the next tack. Please read further about [Tactics and performance functions documentation](docs/README.md).

## Installation

You can start without a polar file and just use the basic functions to start with. Go to [dashboard_tactics_pi Releases pages](https://github.com/canne/dashboard_tactics_pi/releases) and grab an installation file which matches your platform - OpenCPN v5 on Windows, Linux or Mac is supported.

## Configuration

Although not mandatory, it is recommended to disable the built-in
Dashboard (we use the same parameters), standalone Tactics plug-in
and WMM_pi plug-in (unless you do not get the magnetic variation
from your instruments).

If you are a user of tactics_pi, its settings are imported - the originals are not modifed - at first startup of this plug-in.
    

## Compiling

* git clone git://github.com/canne/dashboard_tactics_pi.git

Under windows, you must find the file "opencpn.lib" (Visual Studio) which is built in the build directory after compiling opencpn. 
This file must be copied to the plugins build directory.

### Windows build :

* cd ..
* cd build
* cmake  -T v120_xp ..
* cmake --build . --target package --config release

### Unix style build :

* cd ..
* cd build
* cmake ..
* make
* make package
* sudo make install


### License

The code is licensed under the terms of the GPL v3.
