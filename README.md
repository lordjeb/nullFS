# nullFS File System

nullFS is an educational research project. It is a full implementation of a Windows installable file system that doesn't do any real work. It will allow full operation of the file system: creating files, deleting files, reading and writing data, etc. But it will not actually store the data anywhere. Read operations will be successful, but will always return zero-filled buffers. Write operations will succeed but not persist the data anywhere.

## Testing Environment

1. Windows 10/11 VM (x64 only)
1. Copy files to test VM:
   * `nullFS.sys`
   * `nullFS.inf`
   * `nullFSIntegrationTests.exe`
   * You can use `.\test\scripts\updateTestVm.ps1` to copy these files, but must set the DRIVER_DEV_VM_NAME and DRIVER_DEV_VM_PATH environment variables first.
1. Install driver by double-clicking `nullFS.inf`
1. Enable driver verifier
   * `verifier.exe /flags standard /driver nullFS.sys`
1. Restart VM
1. Attach a kernel debugger
1. [Optional] Enable wmitrace output to debugger
    * `!wmitrace.enable nullFS 4FE5B599-1715-4296-B97E-FE7EB2E28FC3 -level 5`
    * `!wmitrace.start nullFS`
    * `!wmitrace.kdtracing nullFS 1`
1. Run tests
   * `nullFSIntegrationTests.exe --gtest_filter=*`

## Future work items

1. Create a debugger extension to dump the file system state
