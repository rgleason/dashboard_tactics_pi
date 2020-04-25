echo off
chcp 1252
rem Script to start SignalK server node and http-server as localhost-services on Windows
rem Requires node.js latest stable version, npm installed signalk-server and http-server
rem
rem Ensure this Node.js and npm are first in the PATH
set "PATH=%APPDATA%\npm;%~dp0;%PATH%"
setlocal enabledelayedexpansion
pushd "%~dp0"
rem Check out the Node.js version.
set print_version=node.exe -p -e "process.versions.node + ' (' + process.arch + ')'"
for /F "usebackq delims=" %%v in (`%print_version%`) do set version=%%v
rem Print message.
if exist "C:\Program Files\nodejs\npm.cmd" (
  echo Your environment has been set up for using Node.js !version! and npm.
) else (
  echo Your environment has been set up for using Node.js !version! but there is NO npm.
)
popd
endlocal
start signalk-server
start http-server "C:\Program Files (x86)\OpenCPN\plugins\dashboard_tactics_pi\data\instrujs" -p 8088
rem just a short dummy delay to give up the CPU
waitfor /t 5 dummyskstart
start http://localhost:3000
exit
