Set fileSystem = CreateObject("Scripting.FileSystemObject")

zlibDir = "c:\GnuWin32\bin"
quazipDir = "c:\Quazip\lib"
sourceDir = "c:\tupi\sources\tupi\src"
tupiDir = "c:\Kartoon"

If fileSystem.FolderExists(tupiDir) Then
fileSystem.DeleteFolder tupiDir
End If

fileSystem.CreateFolder tupiDir
fileSystem.CreateFolder tupiDir & "\bin"
fileSystem.CreateFolder tupiDir & "\bin\platforms"
fileSystem.CreateFolder tupiDir & "\data"
fileSystem.CreateFolder tupiDir & "\plugins"
fileSystem.CreateFolder tupiDir & "\lib"
fileSystem.CreateFolder tupiDir & "\lib\qt5"
fileSystem.CreateFolder tupiDir & "\lib\ffmpeg"

fileSystem.CopyFile "c:\Qt-5.3.0\5.3\mingw482_32\plugins\platforms\qwindows.dll", tupiDir & "\bin\platforms\qwindows.dll"
fileSystem.CopyFile zlibDir & "\zlib1.dll", tupiDir & "\lib\zlib1.dll"
fileSystem.CopyFile quazipDir & "\quazip.dll", tupiDir & "\lib\quazip.dll"
fileSystem.CopyFile "c:\tupi\sources\tupi\tools\win32\tupi.iss", tupiDir & "\tupi.iss"
fileSystem.CopyFile "c:\tupi\sources\tupi\tools\win32\tupi.bat", tupiDir & "\bin\tupi.bat"
fileSystem.CopyFile sourceDir & "\shell\release\tupi.exe", tupiDir & "\bin\tupi.exe"
fileSystem.CopyFile sourceDir & "\framework\tcore\release\tupifwcore.dll", tupiDir & "\lib\tupifwcore.dll"
fileSystem.CopyFile sourceDir & "\framework\tgui\release\tupifwgui.dll", tupiDir & "\lib\tupifwgui.dll"
fileSystem.CopyFile sourceDir & "\store\release\tupistore.dll", tupiDir & "\lib\tupistore.dll"
fileSystem.CopyFile sourceDir & "\libbase\release\tupibase.dll", tupiDir & "\lib\tupibase.dll"
fileSystem.CopyFile sourceDir & "\libtupi\release\tupi.dll", tupiDir & "\lib\tupi.dll"
fileSystem.CopyFile sourceDir & "\libui\release\tupigui.dll", tupiDir & "\lib\tupigui.dll"
fileSystem.CopyFile sourceDir & "\net\release\tupinet.dll", tupiDir & "\lib\tupinet.dll"
fileSystem.CopyFile sourceDir & "\components\animation\release\tupianimation.dll", tupiDir & "\lib\tupianimation.dll"
fileSystem.CopyFile sourceDir & "\components\colorpalette\release\tupicolorpalette.dll", tupiDir & "\lib\tupicolorpalette.dll"
fileSystem.CopyFile sourceDir & "\components\export\release\tupiexport.dll", tupiDir & "\lib\tupiexport.dll"
fileSystem.CopyFile sourceDir & "\components\exposure\release\tupiexposure.dll", tupiDir & "\lib\tupiexposure.dll"
fileSystem.CopyFile sourceDir & "\components\help\release\tupihelp.dll", tupiDir & "\lib\tupihelp.dll"
fileSystem.CopyFile sourceDir & "\components\import\release\tupimport.dll", tupiDir & "\lib\tupimport.dll"
fileSystem.CopyFile sourceDir & "\components\library\release\tupilibrary.dll", tupiDir & "\lib\tupilibrary.dll"
fileSystem.CopyFile sourceDir & "\components\paintarea\release\tupipaintarea.dll", tupiDir & "\lib\tupipaintarea.dll"
fileSystem.CopyFile sourceDir & "\components\pen\release\tupipen.dll", tupiDir & "\lib\tupipen.dll"
fileSystem.CopyFile sourceDir & "\components\scenes\release\tupiscenes.dll", tupiDir & "\lib\tupiscenes.dll"
fileSystem.CopyFile sourceDir & "\components\timeline\release\tupitimeline.dll", tupiDir & "\lib\tupitimeline.dll"
fileSystem.CopyFile sourceDir & "\components\twitter\release\tupitwitter.dll", tupiDir & "\lib\tupitwitter.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\common\release\tupiplugincommon.dll", tupiDir & "\lib\tupiplugincommon.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\color\release\tupicoloringtool.dll", tupiDir & "\plugins\tupicoloringtool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\compound\release\tupicompoundtool.dll", tupiDir & "\plugins\tupicompoundtool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\eraser\release\tupierasertool.dll", tupiDir & "\plugins\tupierasertool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\fill\release\tupifilltool.dll", tupiDir & "\plugins\tupifilltool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\geometric\release\tupigeometrictool.dll", tupiDir & "\plugins\tupigeometrictool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\ink\release\tupiinktool.dll", tupiDir & "\plugins\tupiinktool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\nodes\release\tupinodestool.dll", tupiDir & "\plugins\tupinodestool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\opacity\release\tupiopacitytool.dll", tupiDir & "\plugins\tupiopacitytool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\pencil\release\tupipenciltool.dll", tupiDir & "\plugins\tupipenciltool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\polyline\release\tupipolylinetool.dll", tupiDir & "\plugins\tupipolylinetool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\position\release\tupipositiontool.dll", tupiDir & "\plugins\tupipositiontool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\rotation\release\tupirotationtool.dll", tupiDir & "\plugins\tupirotationtool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\scale\release\tupiscaletool.dll", tupiDir & "\plugins\tupiscaletool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\selection\release\tupiselectiontool.dll", tupiDir & "\plugins\tupiselectiontool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\shear\release\tupisheartool.dll", tupiDir & "\plugins\tupisheartool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\text\release\tupitexttool.dll", tupiDir & "\plugins\tupitexttool.dll"
fileSystem.CopyFile sourceDir & "\plugins\tools\view\release\tupiviewtool.dll", tupiDir & "\plugins\tupiviewtool.dll"
fileSystem.CopyFile sourceDir & "\plugins\export\imageplugin\release\tupiimageplugin.dll", tupiDir & "\plugins\tupiimageplugin.dll"
fileSystem.CopyFile sourceDir & "\plugins\export\libavplugin\release\tupilibavplugin.dll", tupiDir & "\plugins\tupilibavplugin.dll"

fileSystem.CopyFolder sourceDir & "\shell\data", tupiDir & "\data"
fileSystem.CopyFolder sourceDir & "\components\help\help", tupiDir & "\data\help"
fileSystem.CopyFolder sourceDir & "\components\colorpalette\palettes", tupiDir & "\data\palettes"
fileSystem.CopyFolder sourceDir & "\themes", tupiDir & "\data\themes"
fileSystem.CopyFolder "c:\tupi\sources\qt5", tupiDir & "\lib\qt5"
fileSystem.CopyFolder "c:\tupi\sources\ffmpeg", tupiDir & "\lib\ffmpeg"

MsgBox "Copy is done! :D"
