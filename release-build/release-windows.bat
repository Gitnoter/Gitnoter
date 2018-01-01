rd /s/q .\release\Gitnoter
rd /s/q .\release\Gitnoter.zip
md .\release\Gitnoter

copy /y ..\build-Gitnoter-Desktop_Qt_5_9_3_MSVC2015_32bit-Release\release\Gitnoter.exe .\release\Gitnoter\

windeployqt.exe .\release\Gitnoter\Gitnoter.exe --release

xcopy .\window-missing-dll .\release\Gitnoter\ /s/f/h/y

7z.exe a -tzip -r .\release\Gitnoter-windows-.zip .\release\Gitnoter