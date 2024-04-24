typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

// legal file attribute types
#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LONG_NAME \
    (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

// long name mask
#define ATTR_LONG_NAME_MASK                                        \
    (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | \
     ATTR_DIRECTORY | ATTR_ARCHIVE)
#define Last_Long_Entry 0x40

// FAT Entry Defines
#define ENDOFFILE 0xFFFFFFF
#define RESERVED_START 0xFFFFFF8
#define RESERVED_END 0xFFFFFFE
#define CLUS_INVALID 0xffffff7

// longdir attr
#define LDIR_NAME_LENGTH 13  // 5+6+2

#define ROUNDUP(a, sz) ((((uintptr_t)a) + (sz)-1) & ~((sz)-1))
// Copied from the manual
typedef struct fat32hdr {
    u8 BS_jmpBoot[3];
    u8 BS_OEMName[8];
    u16 BPB_BytsPerSec;
    u8 BPB_SecPerClus;
    u16 BPB_RsvdSecCnt;
    u8 BPB_NumFATs;
    u16 BPB_RootEntCnt;
    u16 BPB_TotSec16;
    u8 BPB_Media;
    u16 BPB_FATSz16;
    u16 BPB_SecPerTrk;
    u16 BPB_NumHeads;
    u32 BPB_HiddSec;
    u32 BPB_TotSec32;
    u32 BPB_FATSz32;
    u16 BPB_ExtFlags;
    u16 BPB_FSVer;
    u32 BPB_RootClus;
    u16 BPB_FSInfo;
    u16 BPB_BkBootSec;
    u8 BPB_Reserved[12];
    u8 BS_DrvNum;
    u8 BS_Reserved1;
    u8 BS_BootSig;
    u32 BS_VolID;
    u8 BS_VolLab[11];
    u8 BS_FilSysType[8];
    u8 __padding_1[420];
    u16 Signature_word;
} __attribute__((packed)) fat32hdr;

typedef struct FSInfo {
    u32 FSI_LeadSig;
    u8 FSI_Reserved1[480];
    u32 FSI_StrucSig;
    u32 FSI_Free_Count;
    u32 FSI_Nxt_Free;
    u8 FSI_Reserved2[12];
    u32 FSI_TrailSig;
} __attribute__((packed)) FSInfo;

// 描述fat32 directory entry
typedef struct fat32dir {
    u8 DIR_Name[11];
    u8 DIR_Attr;
    u8 DIR_NTRes;
    u8 DIR_CrtTimeTenth;
    u16 DIR_CrtTime;
    u16 DIR_CrtDate;
    u16 DIR_LstAccDate;
    u16 DIR_FstClusHI;
    u16 DIR_WrtTime;
    u16 DIR_WrtDate;
    u16 DIR_FstClusLO;
    u32 DIR_FileSize;
} __attribute__((packed)) fat32dir;

// long name directory entry structure
typedef struct fat32longdir {
    u8 LDIR_Ord;
    u16 LDIR_Name1[5];
    u8 LDIR_Attr;
    u8 LDIR_Type;
    u8 LDIR_Chksum;
    u16 LDIR_Name2[6];
    u16 LDIR_FstClusLO;
    u16 LDIR_Name3[2];
} __attribute__((packed)) fat32longdir;

typedef struct fat32Info_t {
    int RootDirSectors;
    int FATSz;
    int TotSec;
    int DataSec;
    int CountOfClusters;
    int clusSz;
    void* fstclusAddr;
} fat32Info_t;