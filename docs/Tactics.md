# Dashboard/Tactics Plugin for OpenCPN

## Tactics active performance functions

[Attached Tactics User's Guide](tactics_pi.pdf) is highly recommended reading, written by the original author
of the algorithms. Below some key features.

### Tactics functions and instruments
* Calculate true wind data : TWA, TWD, TWS from true heading (HDT), speed through water (STW) and app. Wind speed (AWS), with optional correction by heel-angle. Calculation is enabled via a preference setting and disables available true wind data from the bus throughout the tactics_pi plugin.
* Calculate the “leeway”, the boat drift based on heel. A common formula is used for that purpose.
* Calculate the surface sea current and display it as single instruments (current speed/direction) as part of the “Bearing compass” or as overlay on the chart (semi transparent). The routines take boat heel and leeway into account. If you don't have a heel sensor, there is a simply workaround, see manual. Current display on the chart can be disabled by a preference setting.
* Calculate and display the boat laylines for the current tack, and the same TWA on the other tack. Sea current is taken into account, if available ! Boat laylines may be toggled on/off. Adjustable length and max. width (triangle, with one corner at the boat) of boat laylines. The layline width reflects the boat's yawing (COG changes over time).
* You can load a polar file and calculate/display performance data, like actual VMG (velocity made good up-/downwind), Target-VMG, Target-TWA  (the opt. TWA up-/downwind) angle and speed, CMG (course made good towards a waypoint), Target-CMG (opt. CMG) angle and speed, polar speed (the speed you should be able to sail at current TWA/TWS based on your polar), etc.
* The plug-in can display the actual polar as overlay on the chart, including markers for Target-VMG/CMG angles. Just adopt your course and place your heading pointer on one of the markers, and you sail Target-VMG/CMG based on your polar.
* One temporary tactics waypoint can be set and the plug-in ca display the laylines to the mark, based on a Target-TWA calculation, while taking your polar into account.
* The plug-in has a “dial instruments” called “Bearing compass”. Boat true heading (HDT)  points “up”, it shows the boat laylines as well, the surface current, a pointer to the waypoint (either set manually as the temporary Tactics waypoint or read from a NMEA RMB sentence), a needle both for AWA and TWA, and markers for the Target-VMG/CMG angles.
* The “Polar compass” instrument is graphically displaying the actual polar ring and markers for Bearing to WP, Target-VMG angles and Target-CMG angles.
* The “Average Wind” instrument with an adjustable averaging time, which displays graphically the average wind and its deviations to port / starboard
* The plug-in can create specific NMEA performance records with the purpose to export them to the displays of your instruments. One can now, e.g. calculate the polar target speed in the plugin and send it to your instrument display outside in the cockpit. \*\*\* Currently only available for NKE, but may be enhanced in the future \ *\*\*
* There are various settings, which has been grouped in a separate tab. To access the preferences screen, right mouse click on the tactics_pi window, then select “Preferences ...”

### Prerequisites
* You will  need to activate OpenGL, if you want to use the chart based functions
* SOG and COG from the GPS
* preferably True Heading from an electronic compass. --> if not available, magnetic heading will do, as long as you have magnetic variance available (either from the GPS RMC sentence or from the wmm_pi plugin)
* Boat speed through water from a log / “paddlewheel” sensor
* Apparent wind angle and apparent wind speed
* Heel sensor which supplies your boat heel angle to O as XDR sentence - if not available, there is a workaround with a manual input
* \>\>\>\> You need a **polar file of your boat** to use any polar based performance calculation <<<< - If you do not have a polar file for your boat, see the data of weather_plugin_pi to find a closest one.
* Please calibrate AWA, Compass HDG/HDT,  STW (Speed through water), and AWS (apparent wind speed) as good as possible. Especially the compass heading calibration tends to be neglected. It is vital for the proper surface current calculation.

### License

The Tactics code is licensed under the terms of the GPL v3 or, at your will, a later version.
