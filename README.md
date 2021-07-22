# nullFS File System

nullFS is an educational research project. It is a full implementation of a Windows installable file system that doesn't do any real work. It will allow full operation of the file system: creating files, deleting files, reading and writing data, etc. But it will not actually store the data anywhere. Read operations will be successful, but will always return zero-filled buffers. Write operations will succeed but not persist the data anywhere.

## Testing Environment

1. Windows 10 VM
1. Copy files to test VM:
   * `nullFS.sys`
   * `nullFS.inf`
   * `nullFSIntegrationTests.exe`
1. Enable driver verifier
   * `verifier.exe /flags standard /driver nullFS.sys`
1. Enable debug output filter
    * `reg.exe add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Debug Print Filter" /v IHVDRIVER /t REG_DWORD /d 0xFFFFFFFF`
1. Restart VM
1. Attach a kernel debugger
1. Run tests
   * `nullFSIntegrationTests.exe --gtest_filter=*`
