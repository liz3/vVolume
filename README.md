# vVolume
A tool that adjusts the output volume of each COM channel of vPilot from the Planes radio voice control individually.

## Download and Usage
1. Download the zip from the [Releases](https://github.com/liz3/vVolume/releases/).
2. Run vVolume **as administrator**, it needs admin rights in order to be able to set the volume.

## Building
Its two c# solutions and a CMake c++ Project. Build them and then copy the VoiceLink dll into vVolume and the contents of bin/Release from vPilotAdjust into vVolume/vph

# Known issues
* MSFS2024 ini a330: does not communicate volume updates with addons