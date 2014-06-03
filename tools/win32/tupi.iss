[Setup]
AppName=Tupi
AppVersion=0.2-4
DefaultDirName={pf}\Tupi
DefaultGroupName=Tupi Open 2D Magic
OutputDir=c:\tupi\installer
ChangesEnvironment=yes

[Dirs]
Name: "{app}\bin"
Name: "{app}\bin\platforms"
Name: "{app}\data"
Name: "{app}\data\ca"
Name: "{app}\data\cs"
Name: "{app}\data\da"
Name: "{app}\data\de"
Name: "{app}\data\en"
Name: "{app}\data\es"
Name: "{app}\data\gl"
Name: "{app}\data\help"
Name: "{app}\data\help\css"
Name: "{app}\data\help\en"
Name: "{app}\data\help\es"
Name: "{app}\data\help\examples"
Name: "{app}\data\help\gl"
Name: "{app}\data\help\images"
Name: "{app}\data\help\ru"
Name: "{app}\data\palettes"
Name: "{app}\data\pt"
Name: "{app}\data\ru"
Name: "{app}\data\storyboard"
Name: "{app}\data\themes"
Name: "{app}\data\themes\default"
Name: "{app}\data\themes\default\cursors"
Name: "{app}\data\themes\default\icons"
Name: "{app}\data\themes\default\images"
Name: "{app}\data\translations"
Name: "{app}\data\uk"
Name: "{app}\plugins"
Name: "{app}\lib"
Name: "{app}\lib\qt5"

[Files]
Source: "bin\*"; DestDir: "{app}\bin"
Source: "bin\platforms\*"; DestDir: "{app}\bin\platforms"
Source: "data\ca\*"; DestDir: "{app}\data\ca"
Source: "data\cs\*"; DestDir: "{app}\data\cs"
Source: "data\da\*"; DestDir: "{app}\data\da"
Source: "data\de\*"; DestDir: "{app}\data\de"
Source: "data\en\*"; DestDir: "{app}\data\en"
Source: "data\es\*"; DestDir: "{app}\data\es"
Source: "data\gl\*"; DestDir: "{app}\data\gl"
Source: "data\help\css\*"; DestDir: "{app}\data\help"
Source: "data\help\en\*"; DestDir: "{app}\data\help"
Source: "data\help\es\*"; DestDir: "{app}\data\help"
Source: "data\help\examples\*"; DestDir: "{app}\data\help"
Source: "data\help\gl\*"; DestDir: "{app}\data\help"
Source: "data\help\images\*"; DestDir: "{app}\data\help"
Source: "data\help\ru\*"; DestDir: "{app}\data\help"
Source: "data\palettes\*"; DestDir: "{app}\data\palettes"
Source: "data\pt\*"; DestDir: "{app}\data\pt"
Source: "data\ru\*"; DestDir: "{app}\data\ru"
Source: "data\storyboard\*"; DestDir: "{app}\data\storyboard"
Source: "data\themes\default\cursors\*"; DestDir: "{app}\data\themes\default\cursors"
Source: "data\themes\default\icons\*"; DestDir: "{app}\data\themes\default\icons"
Source: "data\themes\default\images\*"; DestDir: "{app}\data\themes\default\images"
Source: "data\translations\*"; DestDir: "{app}\data\translations"
Source: "data\uk\*"; DestDir: "{app}\data\uk"
Source: "plugins\*"; DestDir: "{app}\plugins"
Source: "lib\*"; DestDir: "{app}\lib"
Source: "lib\qt5\*"; DestDir: "{app}\lib\qt5"

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_HOME"; ValueData: "{app}"
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\lib;{app}\lib\qt5"
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_SHARE"; ValueData: "{app}\data"
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_PLUGIN"; ValueData: "{app}\plugins"

[Icons]
Name: "{group}\Tupi"; Filename: "{app}\bin\tupi.exe"; IconFilename: "{app}\bin\tupi.ico"

