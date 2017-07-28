@Echo off
setlocal

set VMTARGET=WIN10-TEST
msbuild.exe nullFS.sln /m /t:Rebuild /p:Platform=x64 /p:Configuration=Debug /v:minimal
net use \\%VMTARGET%\nullFS /user:%VMTARGET%\Admin F5L0gix
robocopy bld \\%VMTARGET%\nullFS\bld /MIR /NJH /NJS /W:3 /R:10

endlocal
