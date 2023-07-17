[Setup]
AppName=Protokollitaja
AppVersion=0.9.1
DefaultDirName={commonpf}\Protokollitaja
DefaultGroupName=Protokollitaja
UninstallDisplayIcon={app}\Protokollitaja.exe
OutputBaseFilename=Protokollitaja Setup 0.9.1
Compression=lzma2
SolidCompression=yes
OutputDir="C:\tmp\"

[Files]
Source: "C:\Users\Paul\Downloads\Protokollitaja-0.9.1\*"; DestDir: "{app}";Flags: ignoreversion recursesubdirs


[Icons]
Name: "{group}\Protokollitaja"; Filename: "{app}\Protokollitaja.exe"
