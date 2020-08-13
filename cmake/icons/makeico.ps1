# /* $Id: makeico.ps1, v1.0 2019/11/30 VaderDarth Exp $ */
#
# A PowerShell Script to build an icon file *.ico
# convert-command is used, from ImageMagick-package

convert Dashboard_Tactics_256px.png -bordercolor white -border 0 Dashboard_Tactics_128px.png Dashboard_Tactics_64px.png Dashboard_Tactics_32px.png Dashboard_Tactics_24px.png Dashboard_Tactics_16px.png -delete 0 -alpha off -colors 256 Dashboard_Tactics.ico
convert Dashboard_Tactics_toggled_256px.png -bordercolor white -border 0 Dashboard_Tactics_toggled_128px.png Dashboard_Tactics_toggled_64px.png Dashboard_Tactics_toggled_32px.png Dashboard_Tactics_toggled_24px.png Dashboard_Tactics_toggled_16px.png -delete 0 -alpha off -colors 256 Dashboard_Tactics_toggled.ico
