#ifndef _IPC_
#define _IPC_
#include <types.hpp>

namespace IOS {
const int ipcMaxPath = 64;
const int ipcMaxFileName = 13;
typedef char IPCFileName[ipcMaxFileName];
typedef char IPCPath[ipcMaxPath];

extern "C" s32 IOS_Open(const char* path, s32 mode);

enum Error {
    ERROR_PERMISSION_DENIED = -1,
    ERROR_FILE_EXISTS = -2,
    ERROR_INVALID_ARG = -4,
    ERROR_FILE_NOT_FOUND = -6,
    ERROR_BUSY = -8
};

enum Mode {
    MODE_NONE = 0x0,
    MODE_READ = 0x1,
    MODE_WRITE = 0x2,
    MODE_READ_WRITE = MODE_READ | MODE_WRITE
};

enum IOCtlType {
    IOCTL_FS_FORMAT = 0x1,
    IOCTL_FS_GETSTATS = 0x2,
    IOCTL_FS_CREATEDIR = 0x3,
    IOCTL_FS_READDIR = 0x4,
    IOCTL_FS_SETATTR = 0x5,
    IOCTL_FS_GETATTR = 0x6,
    IOCTL_FS_DELETE = 0x7,
    IOCTL_FS_RENAME = 0x8,
    IOCTL_FS_CREATEFILE = 0x9,
    IOCTL_FS_SETFILEVERCTRL = 0xA,
    IOCTL_FS_GETFILESTATS = 0xB,
    IOCTL_FS_GETUSAGE = 0xC,
    IOCTL_FS_SHUTDOWN = 0xD,

    IOCTL_HID4_GET_DEVICE_CHANGE = 0x0,
    IOCTL_HID4_INTERRUPT_IN = 0x3,
    IOCTL_HID4_INTERRUPT_OUT = 0x4,
    IOCTL_HID4_GET_VERSION = 0x6,
    IOCTL_HID5_GET_VERSION = 0x0,
    IOCTL_HID5_GET_DEVICE_CHANGE = 0x1,
    IOCTL_HID5_GET_DEVICE_PARAMETERS = 0x3,
    IOCTL_HID5_ATTACH_FINISH = 0x6,
    IOCTL_HID5_SET_RESUME = 0x10,
    IOCTL_HID5_INTERRUPT = 0x13,
};

enum SeekType {
    SEEK_START,
    SEEK_CURRENT,
    SEEK_END
};

struct IOCtlvRequest {
    void* address;
    u32 size;
};

struct RenameFile {
    char oldName[ipcMaxPath];
    char newName[ipcMaxPath];
};
struct Attr {
    u32 uid;
    u16 gid;
    char filepath[ipcMaxPath];
    u8 userperms;
    u8 groupperms;
    u8 otherperms;
    u8 attributes;
    // paddin
};
struct ReadDirectory {
    IOCtlvRequest requests[4];
    u32 entriesCount;
};
struct Usage {
    IOCtlvRequest requests[4];
    u32 usage1;
    u8 padding[28];
    u32 usage2;
};
struct FileStats {
    u32 size;
    u32 pos;
    u32 unknown_0x4[5];
};
struct Request {
    union {
        char path[ipcMaxPath];
        RenameFile rename;
        Attr attr;
        ReadDirectory readDir;
        Usage usage;
        FileStats stats;
    };
};  // total size 0x80
typedef void (*AsyncCallback)(s32 ret, void* arg);

s32 Open(const char* path, Mode mode);
s32 Read(s32 fd, void* buffer, s32 length);
s32 Write(s32 fd, const void* buffer, s32 length);
s32 Seek(s32 fd, s32 offset, SeekType whence);  // returns length until the end, best to get file length
s32 Close(s32 fd);
s32 IOCtl(s32 fd, IOCtlType ioctl, void* buffer_in, s32 len_in, void* buffer_io, s32 len_io);
s32 IOCtlAsync(s32 fd, IOCtlType ioctl, void* buffer_in, s32 len_in, void* buffer_io, s32 len_io, AsyncCallback cb, void* ctxt);

s32 IOCtlAsync();  // 80194158
s32 IOCtlv(s32 fd, IOCtlType ioctl, s32 countIv, s32 countIO, IOCtlvRequest* argv);
s32 Open2ndInst(const char* path, Mode mode);
extern s32 fs_fd;

}  // namespace IOS
#endif