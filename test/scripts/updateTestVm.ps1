$testVmName = $env:DRIVER_DEV_VM_NAME
$testPathOnVm = $env:DRIVER_DEV_VM_PATH

if ($testVmName -eq $null -or $testVmName -eq '') {
	throw 'DRIVER_DEV_VM_NAME environment variable is not set'
}

if ($testPathOnVm -eq $null -or $testPathOnVm -eq '') {
	throw 'DRIVER_DEV_VM_PATH environment variable is not set'
}

$testPathOnVm = Join-Path $testPathOnVm 'nullFS'

Copy-VMFile -VMName $testVmName -SourcePath .\obj\x64\Debug\nullFS.sys -DestinationPath $testPathOnVm -FileSource Host -Force
Copy-VMFile -VMName $testVmName -SourcePath .\obj\x64\Debug\nullFS.inf -DestinationPath $testPathOnVm -FileSource Host -Force
Copy-VMFile -VMName $testVmName -SourcePath .\obj\x64\Debug\nullFSIntegrationTests.exe -DestinationPath $testPathOnVm -FileSource Host -Force
