#ifndef _BMG_
#define _BMG_
#include <kamek.hpp>

struct BMGHeader {  // https://wiki.tockdom.com/wiki/BMG_(File_Format)
    u64 magic;  // MESGbmg1
    u32 fileLength;  // 0x8
    u32 dataBlocksNum;  // 0xC
    u8 encodingFormat;  // 0=Undefined, 1=OneByte (CP1252), 2=TwoBytes (UTF-16), 3=SJIS (Shift-JIS), 4=UTF8. UTF16 in MKWii.
    u8 unknown_0x11[0x20 - 0x1c];
};  // total size 0x20

struct DataBlockHeader {  // same as nw4r::lyt formats such as brlyt
    char magic[4];
    u32 size;
};

struct MessageEntry {
    u32 dat1Offset;  // offset to string
    u8 font;
    u8 padding[3];
};  // 0x8

struct BMGInfo {
    u16 msgCount;
    u16 entrySize;
    u16 groupId;  // 0x4
    u8 defaultColor;
    u8 reserved;
    MessageEntry entries[1];
};

struct BMGInfoBlock {
    DataBlockHeader header;  // INF1
    BMGInfo info;
};

struct BMGData {
    wchar_t stringPool;
};

struct BMGDataBlock {
    DataBlockHeader header;  // DAT1
    BMGData data;
};

struct BMGMessageIds {  // bmgIds = value, MessageId = position of the array
    u16 msgCount;
    u8 format;
    u8 info;
    u8 reserved[4];
    u32 messageIds[1];  // 0x8 idx = bmgId, value = msgId
};

struct BMGMessageIdsBlock {
    DataBlockHeader header;  // MID1
    BMGMessageIds messageIds;
};

class BMGHolder {
   public:
    BMGHolder();  // 805f8b34
    void Load(const char* name);  // 805f8b90
    void Init(const BMGHeader& bmgFile);  // 805f8c00
    s32 GetMsgId(s32 bmgId) const;  // 805f8c88
    wchar_t* GetMsgByMsgId(s32 msgId) const;  // 805f8cf0
    u8* GetFontIndex(u32 msgId) const;  // 805f8d2c see enum FontIndex
    const BMGHeader* bmgFile;  // 0
    const BMGInfo* info;  // 0x4
    const BMGData* data;  // 0x8 string pool
    const u32* str1Block;  // 0xc no idea, doesn't seem to exist in mkw
    const BMGMessageIds* messageIds;  // 0x10
};  // Total Size 0x14
size_assert(BMGHolder, 0x14);

#endif