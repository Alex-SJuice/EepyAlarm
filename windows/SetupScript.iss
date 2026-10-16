[Setup]
AppName=EepyAlarm
AppVersion=2.2

AppMutex=EepyAlarmMutex,Global\EepyAlarmMutex
SetupMutex=SetupMutex,GlobalSetupMutex

DefaultDirName={localappdata}\Programs\EepyAlarm
DefaultGroupName=EepyAlarm

SetupIconFile=icon.ico
WizardSmallImageFile=icon.png
WizardImageFile=icon.png
WizardStyle=polar
UninstallDisplayIcon={app}\EepyAlarm.exe

AppReadmeFile=https://github.com/Alex-SJuice/EepyAlarm/blob/main/README.md

PrivilegesRequired=lowest

[Files]
Source: "EepyAlarm.exe";  DestDir: "{app}"
Source: "icon.ico";       DestDir: "{app}"
Source: "icon.png";       DestDir: "{app}"

[Icons]
Name: "{group}\EepyAlarm\EepyAlarm"; Filename: "{app}\EepyAlarm.exe"; WorkingDir: "{app}"; IconFilename: "{app}\icon.ico"
Name: "{autostartup}\EepyAlarm"; Filename: "{app}\EepyAlarm.exe"; WorkingDir: "{app}"; IconFilename: "{app}\icon.ico"; Parameters: "c"

[Run]
Filename: "{app}\EepyAlarm.exe"; WorkingDir: "{app}"; Flags: postinstall nowait skipifsilent

[InstallDelete]
Type: files; Name: "{localappdata}\EepyAlarm\config.bin"
Type: dirifempty; Name: "{localappdata}\EepyAlarm"

[UninstallDelete]
Type: files; Name: "{localappdata}\EepyAlarm\config.bin"
Type: dirifempty; Name: "{localappdata}\EepyAlarm"

