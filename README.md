# nullFS File System

nullFS is an educational research project. It is a full implementation of a Windows installable file system that doesn't do any real work. It will allow full operation of the file system: creating files, deleting files, reading and writing data, etc. But it will not actually store the data anywhere. Read operations will be successful, but will always return zero-filled buffers. Write operations will succeed but not persist the data anywhere.
