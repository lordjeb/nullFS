@Echo off
msbuild.exe nullFS.sln /t:Rebuild /p:Platform=x64 /p:Configuration=Debug /v:minimal
net use \\vm-win8\nullfs /user:Share asdf
robocopy bld \\vm-win8\nullfs\bld /MIR /NJH /NJS /W:3 /R:10
