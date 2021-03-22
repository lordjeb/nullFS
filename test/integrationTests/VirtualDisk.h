#pragma once

class VirtualDisk
{
public:
    VirtualDisk(const std::wstring& vhdFilename);
    virtual ~VirtualDisk();

    void AssignDriveLetter(const std::wstring& driveLetter);
    void Attach();
    void Create(ULONGLONG diskSizeInBytes);
    void Delete();
    void Detach();
    void Open();

private:
    const std::wstring vhdFilename_;
    wil::unique_hfile vhd_;

    void Initialize();
    std::wstring GetVolumeName();
    std::wstring WaitForVolumeName(long waitTimeMs);
};
