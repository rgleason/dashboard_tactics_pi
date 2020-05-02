# /* $Id: launchDashNodeApps.ps1, v1.0 2019/11/30 VaderDarth Exp $ */
#
# Windows PowerShell script to launch Node.js based applications for DashT   #
##############################################################################
##### Without arguments #####
#
# Verify that node.js package has been installed 
#
##### [service1][,service2] ... [,serviceN] #####
#
# Enumerated node services are started in DashT InstruJS data directory
#
##############################################################################
# This script is designed to run with -ExecutionPolicy Bypass switch on system
# where PowerShell execution policy is Restricted: system is not altered.
# For debugging, as Admin: Get-ExecutionPolicy -List
#                          <note the current execution policy>
#                          Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
#                          <debug...>
#                          Set-ExecutionPolicy <see above> -Scope CurrentUser
##############################################################################

#
# #####################
# Read in the arguments as comma separated list
# #####################

param( $serviceList )
$nodeServices = @()
ForEach ( $service in $serviceList ) {
    $nodeServices += $service
}

#
function Get-InstalledSoftware {
    <#
    .SYNOPSIS
        Retrieves a list of all software installed
    .EXAMPLE
        Get-InstalledSoftware
        
        This example retrieves all software installed on the local computer
    .PARAMETER Name
        The software title you'd like to limit the query to.
    .PARAMETER Arch
        Eiher "x86" for 32-bit applications or "6432" for applications registering themselves in WOV6432 node 
    #>
    [OutputType([System.Management.Automation.PSObject])]
    [CmdletBinding()]
    param (
        [Parameter()]
        [ValidateNotNullOrEmpty()]
        [string]$Name,
        [Parameter()]
        [ValidateNotNullOrEmpty()]
        [string]$Arch
    )
 
    $retval = @()
    $UninstallKeys = @()
    if ($PSBoundParameters.ContainsKey('Arch')) {
        if ( $Arch -eq "x86" ) {
            $UninstallKeys += "HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall"
        }
        else {
            $UninstallKeys += "HKLM:\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall"
        }
    }
    else {
        $UninstallKeys += "HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall", "HKLM:\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall"
    }
    $null = New-PSDrive -Name HKU -PSProvider Registry -Root Registry::HKEY_USERS
    $UninstallKeys += Get-ChildItem HKU: -ErrorAction SilentlyContinue | Where-Object { $_.Name -match 'S-\d-\d+-(\d+-){1,14}\d+$' } | ForEach-Object { "HKU:\$($_.PSChildName)\Software\Microsoft\Windows\CurrentVersion\Uninstall" }
    if (-not $UninstallKeys) {
        $retval = "No software registry keys found"
    } else {
        foreach ($UninstallKey in $UninstallKeys) {
            if ($PSBoundParameters.ContainsKey('Name')) {
                $WhereBlock = { ($_.PSChildName -match '^{[A-Z0-9]{8}-([A-Z0-9]{4}-){3}[A-Z0-9]{12}}$') -and ($_.GetValue('DisplayName') -like "$Name*") }
            } else {
                $WhereBlock = { ($_.PSChildName -match '^{[A-Z0-9]{8}-([A-Z0-9]{4}-){3}[A-Z0-9]{12}}$') -and ($_.GetValue('DisplayName')) }
            }
            $gciParams = @{
                Path        = $UninstallKey
                ErrorAction = 'SilentlyContinue'
            }
            $selectProperties = @(
                @{n='GUID'; e={$_.PSChildName}}, 
                @{n='Name'; e={$_.GetValue('DisplayName')}}
            )
            $retval += Get-ChildItem @gciParams | Where $WhereBlock | Select-Object -Property $selectProperties

        }
    }
    return $retval
}

function LaunchNodeServicePs1 {
    <#
    .SYNOPSIS
        Launch Node.js service from the user's npm-directory using provided PS1-script 
    .EXAMPLE
        LaunchNodeServicePs1 signalk-server
        
        This example launches the signal-server.ps1 script from the npm-directory
    .PARAMETER ServiceName
        The node service name
    #>
    [OutputType([System.Management.Automation.PSObject])]
    [CmdletBinding()]
    param (
        [Parameter()]
        [ValidateNotNullOrEmpty()]
        [string]$ServiceName
    )
    $ps1exe = "C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe"
    $ps1argtmpl = "-ExecutionPolicy Bypass -File "
    $UserAppData = [Environment]::GetEnvironmentVariable('APPDATA')
    $npmPS1path = "$UserAppData\npm\"

    $ps1path = "$npmPS1path\$ServiceName.ps1"
    $ps1pathExists = Test-Path -Path $ps1path
    if ( $ps1pathExists -eq $False ) {
        echo "`n"
        echo "`n"
        echo " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -`n"
        echo "`n"
        echo "`n"
        echo "                  ******************************`n"
        echo "                  * ERRORS: - cannot continue. *`n"
        echo "                  ******************************`n"
        echo "`n"
        echo "`n"
        echo "- Could not find $nodeService startup-up script of Node.js`n"
        echo "  Searching from`n"
        echo "$ps1path`n"
        echo "`n"
    }
    else {
        $ps1arg = "$psargtmpl $ps1path"
        Start-Process $ps1exe $ps1arg
    }
}

# ####################
# Memorize the context
# ####################

$oldPreference = $ErrorActionPreference
$oldLocation = Get-Location



# Test code, useful only for debugging of the Get-InstalledSoftware() function
# $allInstalledGUIDs = Get-InstalledSoftware
# $allInstalledGUIDs = Get-InstalledSoftware -Arch "x86"
# $allInstalledGUIDs = Get-InstalledSoftware -Arch "6432"
# $allInstalledGUIDs | ForEach-Object { $_.GUID; $_.Name }
# $allInstalledGUIDs[0].GUID



# #######
# Node.js must be installed
# #######

$NodeJsFolder = "nodejs"
$nodeCliCmd = "node.exe"
$nodeCliCmdExists = $False
$nodeVesion = "n/a"
$npmCliCmd = "npm.cmd"
$npmCliCmdExists = $False
$npmVersion = "n/a"

$nodeJsGuidObjArray = Get-InstalledSoftware -Name "Node.js"
if ( $null -ne $nodeJsGuidObjArray ) {

    $ErrorActionPreference = ‘stop’
    try {if(Get-Command $nodeCliCmd){
            $nodeCliCmdExists = $True
            $nodeVersion = (node --version) | Out-String
        }
    }
    Catch {
        $nodeCliCmdExists = $False
    }
    Finally {
        $ErrorActionPreference = $oldPreference
    }
    $ErrorActionPreference = ‘stop’
    try {if(Get-Command $npmCliCmd){
            $npmCliCmdExists = $True
            $npmVersion = (npm --version) | Out-String
        }
    }
    Catch {
        $npmCliCmdExists = $False
    }
    Finally {
        $ErrorActionPreference = $oldPreference
    }

    if ( $null -eq $nodeServices ) {
        echo ""
        echo "Found following instances of Node.js installations:"
        echo ""
    `	ForEach ( $nodeJSGuidObj in $nodeJsGuidObjArray ) {
            Write-Verbose ($nodeJsGuidObjArray[0].GUID, $nodeJsGuidObjArray[0].Name) -Separator " "
        }
        echo ""
        if ( $nodeCliCmdExists -eq $True ) {
            echo "$nodeCliCmd --version returns: $nodeVersion"
        }
        if ( $npmCliCmdExists -eq $True ) {
            echo "Node.js package manager:"
            echo "$npmCliCmd --version returns: $npmVersion"
        }
        if ( ($nodeCliCmdExists -eq $False) -OR ($npmCliCmdExists -eq $False) ) {
            $PathString = [Environment]::GetEnvironmentVariable('PATH')
            $foundidx = ($PathString | Select-String $NodeJsFolder).Matches.Index
            echo "Cannot find $nodeCliCmd and/or $npmCliCmd, check your command PATH:"
            if ( $null -eq $foundidx ) {
                echo "- there is no path with '$NodeJsFolder'"
            }
            else {
                echo "- there is a path with '$NodeJsFolder',please check that path being a valid one."
            }
            echo ""
            echo "$PathString"
            echo ""
        }
        echo ""
        sleep 10
        exit 0
    }
    else {
        echo ""
        echo "Node.js $nodeVersion"
        echo "npm $npmVersion"
    }
}
else {
    if ( $null -eq $nodeServices ) {
        echo ""
        echo "Node.js installation not found from this system."
        echo ""
        sleep 10
        exit 0
    }
    else {
        echo ""
        echo ""
        echo " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
        echo ""
        echo ""
        echo "                  ******************************"
        echo "                  * ERRORS: - cannot continue. *"
        echo "                  ******************************"
        echo ""
        echo ""
        echo "- Could not find a Node.js installationservices cannot be started."
        echo "  Redirecting to OpenCPN documentation for instrutions:"
        echo ""
        echo "https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:supplementary_software:signalk:a3"
        echo ""
        Start "https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:supplementary_software:signalk:a3"
        echo ""
        sleep 10
        exit -1
    }
}

# #############################
# OpenCPN and its DashT plug-in must be installed
# #############################


$DashTwebFolderExists = $False
$DashTwebFolderPath = "C:\Program Files (x86)\OpenCPN\plugins\dashboard_tactics_pi\data\instrujs\www"
$DashTwebFolderExists = Test-Path -Path $DashTwebFolderPath

if ( $DashTwebFolderExists -eq $False ) {
    echo ""
    echo ""
    echo " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
    echo ""
    echo ""
    echo "                  ******************************"
    echo "                  * ERRORS: - cannot continue. *"
    echo "                  ******************************"
    echo ""
    echo ""
    echo "- Could not find a DashT http:// data for InstruJS instruments."
    echo ""
    echo $DashTwebFolderPath
    echo ""
    echo "- Have you installed DashT v2.0 or superior?"
    echo ""
    echo "- Redirecting to the donwload page of the latest release:"
    echo ""
    echo "https://github.com/canne/dashboard_tactics_pi/releases/latest"
    echo ""
    Start "https://github.com/canne/dashboard_tactics_pi/releases/latest"
    echo ""
    sleep 10
    exit -1
}


Set-Location( $DashTwebFolderPath )


# ########################
# Ready to launch services, in given order. If not installed, attempt to install first
# ########################

ForEach ( $nodeService in $nodeServices ) {

    $serviceInstalled = $False

    $ErrorActionPreference = ‘stop’
    try {if(Get-Command $nodeService){
            $serviceInstalled = $True
            $retval = LaunchNodeServicePs1 -ServiceName $nodeService
            if ( $null -ne $retval ) {
                echo ""
                echo "Node service $nodeService launch failed:"
                Write-Verbose ($retval)
                echo ""
            }
        }
    }
    Catch {
        $serviceInstalled = $False
    }
    Finally {
        $ErrorActionPreference = $oldPreference
    }
    if ( $serviceInstalled -eq $False ) {
        echo "Node service $nodeService not installed, attempting to install it:"
        echo ""
        $npmInstallAndScope = "install --global --force"
        $npmInstallPermissionOption = ""
        $npmSpecificVersion = ""
        if ( $nodeService -eq "signalk-server" ) {
            $npmInstallPermissionOption = "--unsafe-perm"
            $npmSpecificVersion = "@1.28.0"
        }
        $npmCmdArgs = "$npmInstallAndScope $npmInstallPermissionOption $nodeService$npmSpecificVersion"
        echo $npmCmdArgs
        $instprocess = (Start-Process -Wait $npmCliCmd $npmCmdArgs)
        $retval = LaunchNodeServicePs1 -ServiceName $nodeService
        if ( $null -ne $retval ) {
            echo ""
            echo "Node service $nodeService installation and launch failed:"
            Write-Verbose ($retval)
            echo ""
        }
        else {
            $serviceInstalled = $True
        }
    }
    if ( $serviceInstalled -eq $True ) {
        if ( $nodeService -eq "signalk-server" ) {
            Start "http://127.0.0.1:3000"
        }
        if ( $nodeService -eq "http-server" ) {
            Start "http://127.0.0.1:8080"
        }
        echo "OK: $nodeService"
        echo ""
    }
}

Set-Location( $oldLocation )
echo "Done."
sleep 3
exit 0
