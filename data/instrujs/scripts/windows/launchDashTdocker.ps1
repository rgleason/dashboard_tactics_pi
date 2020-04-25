# /* $Id: launchDashTdocker.ps1, v1.0 2019/11/30 VaderDarth Exp $ */
#
# Windows PowerShell script to launch Docker container services for DashT
##############################################################################
#
##### without arguments, or with argument "start" #####
#
# Three services will be created on the local network of this computer
# ('127.0.0.1' or 'localhost'):
# - web:
#   nginx server, serving ports
#   80: serving instrujs HTML and *.js (Javascript) files to OpenCPN
#   8089: CORS proxy connecting *.js code with the database service
# - graphs:
#   Grafana for the visualization of the data - independently of OpenCPN
#   3000
# - db:
#   InfluxDB v2 time series database server with incoprated data visualization
#   9999
#
# If the above services are already running, no action is taken
#
##### with argument "stop" #####
#
# The above services will be stopped if they are running
##############################################################################
#
$param = $args[0]

####### Prerequisites
$somethingMissing = $False
function Check_Program_Installed( $programName ) {
$x86_check = ((Get-ChildItem "HKLM:Software\Microsoft\Windows\CurrentVersion\Uninstall") |
Where-Object { $_."Name" -like "*$programName*" } ).Length -gt 0;
if(Test-Path 'HKLM:Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall')  
{
$x64_check = ((Get-ChildItem "HKLM:Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall") |
Where-Object { $_."Name" -like "*$programName*" } ).Length -gt 0;
}
return $x86_check -or $x64_check;
}
# Check if Docker Desktop for Windows has been installed
$oldPreference = $ErrorActionPreference
$ErrorActionPreference = ‘stop’
$dockerName = "Docker Desktop"
$dockerOK = Check_Program_Installed($dockerName)
if ( $dockerOK -ne $True ) {
    $somethingMissing = $True
    }
# check if Docker CLI exists or is in the path
$dockerCliCmd = "docker.exe"
$dockerCliCmdExists = $False
try {if(Get-Command $dockerCliCmd){
    $dockerCliCmdExists = $True
    }
}
Catch {}
Finally {
 $ErrorActionPreference = $oldPreference
}
if ( $dockerCliCmdExists -ne $True ) {
    $somethingMissing = $True
    }
# check if Docker Composer programs exists or is in the path
$composerCmd = "docker-compose.exe"
$composerCmdExists = $False
try {if(Get-Command $composerCmd){
    $composerCmdExists = $True
    }
}
Catch {}
Finally {
 $ErrorActionPreference = $oldPreference
}
if ( $composerCmdExists -ne $True ) {
    $somethingMissing = $True
    }
# Check if the docker-compose.yml configuration file is in expected location
$composerConfDir = "~\instrujs"
$composerConfFile = "docker-compose.yml"
$composerConfPath = "$composerConfDir\$composerConfFile"
$composerConfPathOK = Test-Path($composerConfPath)
if ( $composerConfPathOK -ne $True ) {
    $somethingMissing = $True
    }
# If an issue, give a report
if ( $somethingMissing -eq $True ) {
    echo "ERRORS: - cannot continue."
    if ( $dockerOK -ne $True ) {
        echo "- Cannot find '$dockerName', please install first"
        }
    else {
        if ( $dockerCliCmdExists -ne $True ) {
            echo "- Cannot find '$dockerCliCmd' (CLI) (while '$dockerName' is installed): probably not in the path."
            }
        if ( $composerCmdExists -ne $True ) {
            echo "- Cannot find '$composerCmd' (while '$dockerName' is installed): probably not in the path."
            }
        }
    if ( $composerConfPathOK -ne $True ) {
        echo "- Cannot find Docker Composer configuration file: '$composerConfPath'"
        }
    exit -1
    }
#
####### Prerequisites PASS
#
$oldLocation = Get-Location
Set-Location( $composerConfDir )

#
# Ready to start or stop the composer services
#

$composerArgUp     = "up"
$composerArgInBgnd = "-d"
$composerArgDown   = "down"

if ( $param -ne "stop" ) {
    echo "Creating and starting composed DashT service containers..."
    $composerArg = "$composerArgUp $composerArgInBgnd"
    }
else {
    echo "Stopping and removing composed DashT service containers..."
    $composerArg = $composerArgDown
}

Start-Process -Wait $composerCmd $composerArg

# Set InfluxDB image container time is as the time obtained from the host
# This is Hyper-V sleep issue. Courtesy to https://dockr.ly/3eJNErk
# One just need to add "T" in the date to make it ISO compatible.

### NOTE: DEPENDENCY TO THE INFLUXDB OFFICIAL IMAGE NAME ###
$influxImgName = "quay.io/influxdb/influxdb:2.0.0-beta"
if ( $param -ne "stop" ) {
    sleep 3
    echo "Adjusting Hyper-V sleep affected date and time (UTC) on all InfluxDB containers..."
    $datetime = Get-Date
    $dt = $datetime.ToUniversalTime().ToString('yyyy-MM-ddTHH:mm:ss')
    echo $dt
    $dockerCliCmdArgs = "run --net=host --ipc=host --uts=host --pid=host --security-opt=seccomp=unconfined --privileged --rm $influxImgName date -s $dt"
    Start-Process -Wait $dockerCliCmd $dockerCliCmdArgs
    }

# Add Grafana plug-in for InfluxDB data query format (Flux) as data source.
# Grana stores this if local datavolume is used, but if volume is volatile,
# it will be lost. It is bettero to install it every time.

### NOTE: DEPENDENCY TO THE CONTAINER NAME IN docker-compose.yml ###
$grafanaContainerName = "dasht_grafana"
if ( $param -ne "stop" ) {
    ### NOTE: DEPENDENCY TO THE GRAFANA PLUG-IN LOCAL INSTALLATION DIRECTORY ###
    # If not bind but mount volume; or if no volume: installed always
    $grafanaLocalBindFluxPluginDescrFile = "~\instrujs\grafana\data\plugins\grafana-influxdb-flux-datasource\docker-compose.yml"
    $hasGrafanaLocalBindFluxPluginDescrFile = Test-Path($grafanaLocalBindFluxPluginDescrFile)
    if ( $hasGrafanaLocalBindFluxPluginDescrFile -eq $False ) {
        sleep 2
        echo "(re)Installing InfluxDB Flux as datasource plug-in into DashT Grafana service..."
        $dockerCliCmdArgs = "exec -it $grafanaContainerName grafana-cli plugins install grafana-influxdb-flux-datasource"
        Start-Process -Wait $dockerCliCmd $dockerCliCmdArgs
       }
    }

# Done
Set-Location( $oldLocation )
echo "Done."
exit 0