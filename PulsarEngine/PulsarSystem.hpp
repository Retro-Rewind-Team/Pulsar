#ifndef _PULSAR_
#define _PULSAR_

#include <kamek.hpp>
#include <core/egg/mem/ExpHeap.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
#include <MarioKartWii/UI/Text/Text.hpp>
#include <Extensions/LECODE/LECODEMgr.hpp>
#include <Debug/Debug.hpp>
#include <IO/IO.hpp>
#include <Info.hpp>
#include <Config.hpp>
#include <Network/Network.hpp>
#include <Network/MatchCommand.hpp>
#include <Gamemodes/OnlineTT/OnlineTT.hpp>

namespace Pulsar {
namespace KO {
class Mgr;
}  // namespace KO

class ConfigFile;

enum Context {
    PULSAR_CT = 0,
    PULSAR_200,
    PULSAR_200_WW,
    PULSAR_FEATHER,
    PULSAR_UMTS,
    PULSAR_SMTS,
    PULSAR_KOFINAL,
    PULSAR_MEGATC,
    PULSAR_HAW,
    PULSAR_MIIHEADS,
    PULSAR_MODE_OTT,
    PULSAR_MODE_KO,
    PULSAR_CHARRESTRICTLIGHT,
    PULSAR_CHARRESTRICTMID,
    PULSAR_CHARRESTRICTHEAVY,
    PULSAR_KARTRESTRICT,
    PULSAR_BIKERESTRICT,
    PULSAR_500,
    PULSAR_THUNDERCLOUD,
    PULSAR_REGS,
    PULSAR_RETROS,
    PULSAR_CTS,
    PULSAR_ALL,
    PULSAR_CHANGECOMBO,
    PULSAR_ITEMBOXRESPAWN,
    PULSAR_EXTENDEDTEAMS,
};

enum Context2 {
    PULSAR_TRANSMISSIONINSIDE,
    PULSAR_TRANSMISSIONOUTSIDE,
    PULSAR_TRANSMISSIONVANILLA,
    PULSAR_ITEMMODERANDOM,
    PULSAR_ITEMMODEBLAST,
    PULSAR_ITEMMODERAIN,
    PULSAR_ITEMMODESTORM,
    PULSAR_ITEMMODENONE
};

class System {
   protected:
    System();

   public:
    // System functions
    void Init(const ConfigFile& conf);
    void InitInstances(const ConfigFile& conf, IOType type);
    void InitIO(IOType type) const;
    void InitCups(const ConfigFile& conf);
    void InitSettings(const u16* totalTrophyCount) const;
    void UpdateContext();
    static void UpdateContextWrapper();
    static void ClearOttContext();

   protected:
    // Virtual
    virtual void AfterInit() {};

   public:
    static System* sInstance;

    virtual void SetUserInfo(Network::ResvInfo::UserInfo& userInfo) {};
    virtual bool CheckUserInfo(const Network::ResvInfo::UserInfo& userInfo) { return true; };
    // Deprecated because you can now freely expand ROOM packets and do what you need to with them
    // virtual u8 SetPackROOMMsg() { return 0; } //Only called for hosts
    // virtual void ParsePackROOMMsg(u8 msg) {}  //Only called for non-hosts
    const Info& GetInfo() const { return this->info; }

    bool IsContext(Context context) const { return (this->context & (1 << context)) != 0; }
    bool IsContext(Context2 context2) const { return (this->context2 & (1 << context2)) != 0; }
    static s32 OnSceneEnter(Random& random);

    const char* GetModFolder() const { return modFolderName; }
    static void CreateSystem();

    // Network
    static asmFunc GetRaceCount();

    // Modes
    static asmFunc GetNonTTGhostPlayersCount();

    // BMG
    const BMGHolder& GetBMG() const { return customBmgs; }
    /*
    #define PatchRegion(addr)\
        static inline u64 GetWiimmfiRegionStatic##addr(u64 src) {\
            register const Info *info = &System::sInstance->GetInfo();\
            asmVolatile(lwz r7, Info.wiimmfiRegion(info););\
            return src;\
        };\
        kmBranch(addr, GetWiimmfiRegionStatic##addr);\
        kmPatchExitPoint(GetWiimmfiRegionStatic##addr, ##addr + 4);
    */
    // VARIABLES
    EGG::ExpHeap* const heap;  // 0x4
    EGG::TaskThread* const taskThread;  // 0x8
    // Constants

   public:
    char modFolderName[IOS::ipcMaxFileName + 1];  // 0xC
    u8 padding[2];
    Info info;  // 0x1c
    u32 context;
    u32 context2;

   public:
    // Network variables only set when reading a ROOM packet that starts the GP; they are only ever used in UpdateState; no need to clear them as ROOM will reupdat ethem
    Network::Mgr netMgr;

    TTMode ttMode;

    // LECODE data
    LECODE::Mgr lecodeMgr;

    // Modes
    KO::Mgr* koMgr;
    bool ottHideNames;
    OTT::Mgr ottMgr;
    u8 nonTTGhostPlayersCount;  // because a ghost can be added in vs, racedata's playercount is not reliable

   private:
    // Custom BMGS
    BMGHolder customBmgs;
    BMGHeader* rawBmg;

   public:
    // string pool
    static const char pulsarString[];
    static const char CommonAssets[];
    static const char breff[];
    static const char breft[];
    static const char* ttModeFolders[];

    struct Inherit {
        // static_assert(is_base_of<System, Child>::value, "Pulsar::System is not a parent of your class");
        typedef System* (*CreateFunc)();
        Inherit(CreateFunc func) {
            // static_assert(inherit == nullptr, "Can only inherit once from Pulsar::System");
            create = func;
            inherit = this;
        }
        CreateFunc create;
    };
    static Inherit* inherit;
    friend class Info;
};
}  // namespace Pulsar

#endif