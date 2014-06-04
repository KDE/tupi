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
Name: "{app}\data\help"
Name: "{app}\data\help\css"
Name: "{app}\data\help\en"
Name: "{app}\data\help\es"
Name: "{app}\data\help\examples"
Name: "{app}\data\help\gl"
Name: "{app}\data\help\images"
Name: "{app}\data\help\ru"
Name: "{app}\data\palettes"
Name: "{app}\data\storyboard"
Name: "{app}\data\themes"
Name: "{app}\data\themes\default"
Name: "{app}\data\themes\default\cursors"
Name: "{app}\data\themes\default\icons"
Name: "{app}\data\themes\default\images"
Name: "{app}\data\translations"
Name: "{app}\data\xml"
Name: "{app}\data\xml\ca"
Name: "{app}\data\xml\cs"
Name: "{app}\data\xml\da"
Name: "{app}\data\xml\de"
Name: "{app}\data\xml\en"
Name: "{app}\data\xml\es"
Name: "{app}\data\xml\gl"
Name: "{app}\data\xml\pt"
Name: "{app}\data\xml\ru"
Name: "{app}\data\xml\uk"
Name: "{app}\plugins"
Name: "{app}\lib"
Name: "{app}\lib\qt5"

[Files]
Source: "bin\*"; DestDir: "{app}\bin"
Source: "bin\platforms\*"; DestDir: "{app}\bin\platforms"
Source: "data\help\css\*"; DestDir: "{app}\data\help"
Source: "data\help\en\*"; DestDir: "{app}\data\help"
Source: "data\help\es\*"; DestDir: "{app}\data\help"
Source: "data\help\examples\*"; DestDir: "{app}\data\help"
Source: "data\help\gl\*"; DestDir: "{app}\data\help"
Source: "data\help\images\*"; DestDir: "{app}\data\help"
Source: "data\help\ru\*"; DestDir: "{app}\data\help"
Source: "data\palettes\*"; DestDir: "{app}\data\palettes"
Source: "data\storyboard\*"; DestDir: "{app}\data\storyboard"
Source: "data\themes\default\cursors\*"; DestDir: "{app}\data\themes\default\cursors"
Source: "data\themes\default\icons\*"; DestDir: "{app}\data\themes\default\icons"
Source: "data\themes\default\images\*"; DestDir: "{app}\data\themes\default\images"
Source: "data\translations\*"; DestDir: "{app}\data\translations"
Source: "data\xml\ca\*"; DestDir: "{app}\data\xml\ca"
Source: "data\xml\cs\*"; DestDir: "{app}\data\xml\cs"
Source: "data\xml\da\*"; DestDir: "{app}\data\xml\da"
Source: "data\xml\de\*"; DestDir: "{app}\data\xml\de"
Source: "data\xml\en\*"; DestDir: "{app}\data\xml\en"
Source: "data\xml\es\*"; DestDir: "{app}\data\xml\es"
Source: "data\xml\gl\*"; DestDir: "{app}\data\xml\gl"
Source: "data\xml\pt\*"; DestDir: "{app}\data\xml\pt"
Source: "data\xml\ru\*"; DestDir: "{app}\data\xml\ru"
Source: "data\xml\uk\*"; DestDir: "{app}\data\xml\uk"
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

