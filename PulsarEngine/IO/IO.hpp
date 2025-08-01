#ifndef _PULSARIO_
#define _PULSARIO_

#include <kamek.hpp>
#include <core/rvl/devfs/isfs.hpp>
#include <core/egg/mem/Heap.hpp>
#include <core/egg/Thread.hpp>

namespace Pulsar {

const int maxFileCount = 100;
typedef char FileName[255];

enum IOType {
    IOType_RIIVO = 0,
    IOType_ISO = 1,
    IOType_DOLPHIN = 2,
    IOType_SD = 3
};

enum FileMode {
    FILE_MODE_NONE,
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_READ_WRITE = FILE_MODE_READ | FILE_MODE_WRITE
};

class IO {
    struct CreateRequest {
        CreateRequest() : isFree(true) {};
        bool isFree;
        char path[IOS::ipcMaxPath];
    };
    static void CreateFolderAsync(CreateRequest* request);

   public:
    static inline s32 OpenFix(const char* path, IOS::Mode mode) {
        asmVolatile(stwu sp, -0x0020(sp););
        IOS::Open2ndInst(path, mode);
    }

    virtual bool OpenFile(const char* path, u32 mode) = 0;
    virtual bool CreateAndOpen(const char* path, u32 mode) = 0;
    virtual bool RenameFile(const char* oldPath, const char* newPath) const = 0;

    virtual bool FolderExists(const char* path) const = 0;
    virtual bool CreateFolder(const char* path) = 0;
    virtual void ReadFolder(const char* path) = 0;

    static IO* sInstance;
    static IO* CreateInstance(IOType type, EGG::Heap* heap, EGG::TaskThread* const taskThread);
    template <typename T>
    T* Alloc(u32 size) const { return EGG::Heap::alloc<T>(nw4r::ut::RoundUp(size, 0x20), 0x20, this->heap); }
    virtual s32 GetFileSize() = 0;

    virtual s32 Read(u32 size, void* bufferIn) = 0;
    virtual void Seek(u32 offset) = 0;
    virtual s32 Write(u32 length, const void* buffer) = 0;
    virtual s32 Overwrite(u32 length, const void* buffer) = 0;
    virtual void Close() = 0;

    const int GetFileCount() const { return this->fileCount; }
    const char* GetFolderName() const { return this->folderName; };
    // void RequestCreateFolder(const char* path); //up to 2 simultaneous
    virtual void CloseFolder() = 0;
    void PrintFullFilePath(char* path, const char* fileName) const {
        snprintf(path, IOS::ipcMaxPath, "%s/%s", &this->folderName, fileName);
    }
    void GetFolderFilePath(char* dest, u32 index) const {
        this->PrintFullFilePath(dest, reinterpret_cast<const char*>(&this->fileNames[index]));
    }
    const char* GetFileName(u32 index) const {
        return reinterpret_cast<const char*>(&this->fileNames[index]);
    }

    s32 ReadFolderFileFromPath(void* buffer, const char* path, u32 maxLength);

    s32 ReadFolderFile(void* bufferIn, u32 index, u32 maxLength) {
        char path[IOS::ipcMaxPath];
        this->GetFolderFilePath(path, index);
        return this->ReadFolderFileFromPath(bufferIn, path, maxLength);
    }
    s32 ReadFolderFileFromName(void* bufferIn, const char* name, u32 maxLength) {
        char path[IOS::ipcMaxPath];
        this->PrintFullFilePath(path, name);
        return this->ReadFolderFileFromName(bufferIn, path, maxLength);
    }

    const IOType type;

   protected:
    IO(IOType type, EGG::Heap* heap, EGG::TaskThread* taskThread) : type(type), heap(heap), taskThread(taskThread) {
        folderName[0] = '\0';
    }
    void Bind(const char* path) { strncpy(this->folderName, path, IOS::ipcMaxPath); }
    void CloseFile() { this->Close(); }

    EGG::Heap* heap;
    EGG::TaskThread* const taskThread;
    char folderName[IOS::ipcMaxPath];
    u32 fileCount;
    IOS::IPCPath* fileNames;
    CreateRequest requests[2];
};

}  // namespace Pulsar

#endif