# Sweet Crush
Sweet Crush is an open source, cross-platform Match-3 game.

![Screenshot](misc/screenshot.png)

## Features
* Match3 game mechanics
* Cartoon graphics
* Music and sound effects
* English, Spanish and Italian localization

## Credits
* Code by [Stefano Lanza](steflanz@gmail.com)
* Graphics by [GameArtGuppy](https://www.gameartguppy.com)
* Music by [Patrick De Arteaga](https://patrickdearteaga.com)

## Requirements
* Windows 10
* Android 4.4

## Installation
* Clone the github repository to a folder (REPO) on your local machine
* For a Windows build
  * Open SweetCrush.sln in Visual Studio 2019
  * Build the solution in either Release or Debug configuration
  * Run the built executable (SweetCrush.exe or SweetCrushd.exe) in the bin folder
* For an Android build
  * Open SweetCrush_android.sln in Visual Studio 2019
  * Build the solution in either Release or Debug configuration, and either ARM or x86 depending on your target device
  * In Visual Studio, select Tools/Android/Android Adb Command Prompt
  * In the command prompt, go to the REPO/android folder
  * Run signAPKs.bat to sign the ARM and x86 APKs built with the Release configuration
  * Copy the signed APK (either the x86 or ARM one) to your device, install it and play

## Technical Info
* Compiled with Visual Studio Professional 2019
  * Windows SDK 10.0.14393
  * Platform toolsets v142 and LLVM
  * C++ 17 
* Third party libraries
  * SDL 2.0.12
  * SDL Mixer 2.0.4
  * SDL Image 2.0.6
  * zlib
  * inih - simple .INI file parser

## Future Work
* New game mechanics
* More animations, power ups and sound effects
* Scoreboard
* iOS port

## Contributions
I need help for the following:
* Design the game mechanics
* Port the game to iOS
