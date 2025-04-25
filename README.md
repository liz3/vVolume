# vVolume
A tool that adjusts the output volume of each COM channel of vPilot from the Planes radio voice control individually.

**Note: Certain planes do not send the updates when changing the com volumes. You can also use the Scrollwheel ontop of the bar in the tool to directly adjust it.**

![Screenshot of the tool](/vvolume.png)

## Download and Usage
1. Download the zip from the [Releases](https://github.com/liz3/vVolume/releases/).
2. Run vVolume **as administrator**, it needs admin rights in order to be able to set the volume.

## Building
Its two c# solutions and a CMake c++ Project. Build them and then copy the VoiceLink dll into vVolume and the contents of bin/Release from vPilotAdjust into vVolume/vph

## Support disclaimer
This is a fun thing for me, i will not provide support. If you find bugs open a issue, if you expect them to be fixed fast clone the code and fix it yourself.

# Known issues
* MSFS2024 ini a330: does not communicate volume updates with addons