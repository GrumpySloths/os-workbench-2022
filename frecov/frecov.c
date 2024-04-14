#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

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
#define ATTR_LONG_NAME (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

// long name mask
#define ATTR_LONG_NAME_MASK (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID \
                             | ATTR_DIRECTORY | ATTR_ARCHIVE)
#define Last_Long_Entry 0x40

#define ENDOFFILE 0xFFFFFFF
// Copied from the manual
struct fat32hdr {
  u8  BS_jmpBoot[3];
  u8  BS_OEMName[8];
  u16 BPB_BytsPerSec;
  u8  BPB_SecPerClus;
  u16 BPB_RsvdSecCnt;
  u8  BPB_NumFATs;
  u16 BPB_RootEntCnt;
  u16 BPB_TotSec16;
  u8  BPB_Media;
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
  u8  BPB_Reserved[12];
  u8  BS_DrvNum;
  u8  BS_Reserved1;
  u8  BS_BootSig;
  u32 BS_VolID;
  u8  BS_VolLab[11];
  u8  BS_FilSysType[8];
  u8  __padding_1[420];
  u16 Signature_word;
} __attribute__((packed));

struct FSInfo{ 
  u32 FSI_LeadSig;
  u8  FSI_Reserved1[480];
  u32 FSI_StrucSig;
  u32 FSI_Free_Count;
  u32 FSI_Nxt_Free;
  u8  FSI_Reserved2[12];
  u32 FSI_TrailSig;
} __attribute__((packed));

//描述fat32 directory entry
struct fat32dir {
  u8  DIR_Name[11];
  u8  DIR_Attr;
  u8  DIR_NTRes;
  u8  DIR_CrtTimeTenth;
  u16 DIR_CrtTime;
  u16 DIR_CrtDate;
  u16 DIR_LstAccDate;
  u16 DIR_FstClusHI;
  u16 DIR_WrtTime;
  u16 DIR_WrtDate;
  u16 DIR_FstClusLO;
  u32 DIR_FileSize;
} __attribute__((packed));

// long name directory entry structure
struct fat32longdir {
  u8  LDIR_Ord;
  u16 LDIR_Name1[5];
  u8  LDIR_Attr;
  u8  LDIR_Type;
  u8  LDIR_Chksum;
  u16 LDIR_Name2[6];
  u16 LDIR_FstClusLO;
  u16 LDIR_Name3[2];
} __attribute__((packed));



void *map_disk(const char *fname);
void print_long_name(struct fat32longdir*longdir);
struct fat32dir * get_RootDir(struct fat32hdr *hdr);
struct fat32dir *ClusToDir(struct fat32hdr *hdr,int ClusId);
u32 DirToClus(struct fat32dir*dir);
static int EntCnt = 0;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s fs-image\n", argv[0]);
    exit(1);
  }

  setbuf(stdout, NULL);

  assert(sizeof(struct fat32hdr) == 512); // defensive

  // map disk image to memory
  struct fat32hdr *hdr = map_disk(argv[1]);

  // TODO: frecov
  // determined CountOfClusters
  int RootDirSectors=((hdr->BPB_RootEntCnt*32)+(hdr->BPB_BytsPerSec-1))/hdr->BPB_BytsPerSec;
  int FATSz=hdr->BPB_FATSz32;
  int TotSec = hdr->BPB_TotSec32;
  int DataSec = TotSec - (hdr->BPB_RsvdSecCnt + (hdr->BPB_NumFATs * FATSz) +
                          RootDirSectors);
  int CountOfClusters = DataSec / hdr->BPB_SecPerClus;
  printf("CountOfClusters: %d\n", CountOfClusters);
  //打印BPB_FATSz32
  printf("BPB_FATSz32: %d\n", hdr->BPB_FATSz32);
  //获取FSInfo的地址并将其属性进行打印
  printf("BPB_FSInfo: %d\n", hdr->BPB_FSInfo);
  struct FSInfo *fsi = (struct FSInfo *)((char *)hdr + hdr->BPB_FSInfo * hdr->BPB_BytsPerSec);
  printf("FSI_LeadSig: %x\n", fsi->FSI_LeadSig);
  printf("FSI_StrucSig: %x\n", fsi->FSI_StrucSig);
  printf("FSI_Free_Count: %d\n", fsi->FSI_Free_Count);
  printf("FSI_Nxt_Free: %d\n", fsi->FSI_Nxt_Free);
  printf("FSI_TrailSig: %x\n", fsi->FSI_TrailSig);

  printf("RootClus: %d\n", hdr->BPB_RootClus);
  //获取根目录的地址并将其属性打印出来
  u32 FirstDataSector = hdr->BPB_RsvdSecCnt + hdr->BPB_NumFATs * hdr->BPB_FATSz32 + RootDirSectors;
  struct fat32dir *rootdir = get_RootDir(hdr);
  printf("RootDir attr: %d\n", rootdir->DIR_Attr);
  printf("RootDir filesize: %d\n", rootdir->DIR_FileSize);
  printf("Rootdir name:%s\n", rootdir->DIR_Name);

  //根据FstClusLO 和 FstClusHI 计算出下一个cluster的地址
  u32 NextCluster = DirToClus(rootdir);
  struct fat32dir*nextdir=ClusToDir(hdr,NextCluster);

  //根据NextCluster计算对应的fat entry
  int FATOffset = NextCluster * 4;
  int FATSecNum = hdr->BPB_RsvdSecCnt + FATOffset / hdr->BPB_BytsPerSec;
  int FATEntOffset = FATOffset % hdr->BPB_BytsPerSec;
  u32 *FAT = (u32 *)((char *)hdr + FATSecNum * hdr->BPB_BytsPerSec);
  printf("Entry value: %x\n", FAT[FATEntOffset / 4] & ENDOFFILE);
  u32 FATValue = FAT[FATEntOffset / 4] & ENDOFFILE;

  //根据FaTValue 循环打印fat list，直至遇到ENDOFFILE
  while (FATValue < ENDOFFILE) {
    FATOffset = FATValue * 4;
    FATSecNum = hdr->BPB_RsvdSecCnt + FATOffset / hdr->BPB_BytsPerSec;
    FATEntOffset = FATOffset % hdr->BPB_BytsPerSec;
    FAT = (u32 *)((char *)hdr + FATSecNum * hdr->BPB_BytsPerSec);
    printf("Entry value: %x\n", FAT[FATEntOffset / 4] & ENDOFFILE);
    FATValue = FAT[FATEntOffset / 4] & ENDOFFILE;
  }

  //根据FATValue计算下一个cluster的地址
  // NextCluster = FATValue;
  // NextSector = ((NextCluster - 2) * hdr->BPB_SecPerClus) + FirstDataSector;
  // NextDirAddr = NextSector * hdr->BPB_BytsPerSec;
  // nextdir = (struct fat32dir *)((char *)hdr + NextDirAddr);


  // 打印nextdir文件大小
  printf("NextDir filesize: %d\n", nextdir->DIR_FileSize);
  struct fat32dir* temp = nextdir;

  int cnt= 0;
  while (nextdir[EntCnt].DIR_Attr && !nextdir[EntCnt].DIR_NTRes&&EntCnt<128) {
    if (nextdir[EntCnt].DIR_Attr == ATTR_LONG_NAME) {
      print_long_name((struct fat32longdir *)&nextdir[EntCnt]);
    } else {
      printf("Short name: %s EntCnt:%d\n", nextdir[EntCnt].DIR_Name, EntCnt);
    }
    EntCnt++;
    cnt++;
  }
  printf("EntCnt:%d,cnt:%d\n", EntCnt,cnt);


  munmap(hdr, hdr->BPB_TotSec32 * hdr->BPB_BytsPerSec);
}

void *map_disk(const char *fname) {
  int fd = open(fname, O_RDWR);

  if (fd < 0) {
    perror(fname);
    goto release;
  }

  off_t size = lseek(fd, 0, SEEK_END);
  if (size == -1) {
    perror(fname);
    goto release;
  }

  struct fat32hdr *hdr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (hdr == (void *)-1) {
    goto release;
  }

  close(fd);

  if (hdr->Signature_word != 0xaa55 ||
      hdr->BPB_TotSec32 * hdr->BPB_BytsPerSec != size) {
    fprintf(stderr, "%s: Not a FAT file image\n", fname);
    goto release;
  }
  return hdr;

release:
  if (fd > 0) {
    close(fd);
  }
  exit(1);
}

//定义一个函数，打印fat32 directory entry's long name
void print_long_name(struct fat32longdir*longdir){
  //check last name entry mask
  // assert(longdir->LDIR_Ord & Last_Long_Entry);
  if(!(longdir->LDIR_Ord & Last_Long_Entry)){
      EntCnt++;
      return;
  }
  //get last id
  int n=longdir->LDIR_Ord^Last_Long_Entry;
  //from 1 to n print the name
  //设置一个数组来存储long name,其最大长度为255
  u16 name[255];

  int cur = 0;
  for (int i = n - 1; i >= 0; i--) {
    //get name1
    for(int j=0;j<5;j++){
      name[cur++]=longdir[i].LDIR_Name1[j];
    }
    //get name2
    for(int j=0;j<6;j++){
      name[cur++]=longdir[i].LDIR_Name2[j];
    }
    //get name3
    for(int j=0;j<2;j++){
      name[cur++]=longdir[i].LDIR_Name3[j];
    }
  }
  //print the name,遇到null打印换行符后停止
  for (int i = 0; name[i] != 0;i++){
      printf("%c", name[i]);
  }
  printf("\n");

  // longdir += n;
  EntCnt += n;
}

struct fat32dir* get_RootDir(struct fat32hdr*hdr){

  int RootDirSectors=((hdr->BPB_RootEntCnt*32)+(hdr->BPB_BytsPerSec-1))/hdr->BPB_BytsPerSec; 

  u32 RootClus = hdr->BPB_RootClus;
  u32 FirstDataSector = hdr->BPB_RsvdSecCnt + hdr->BPB_NumFATs * hdr->BPB_FATSz32 + RootDirSectors;
  u32 FirstSectorofCluster = ((RootClus - 2) * hdr->BPB_SecPerClus) + FirstDataSector;
  u32 RootDirAddr = FirstSectorofCluster * hdr->BPB_BytsPerSec;
  struct fat32dir *rootdir = (struct fat32dir *)((char *)hdr + RootDirAddr);

  return rootdir;
}

struct fat32dir* ClusToDir(struct fat32hdr*hdr,int ClusId){

  int RootDirSectors=((hdr->BPB_RootEntCnt*32)+(hdr->BPB_BytsPerSec-1))/hdr->BPB_BytsPerSec; 
  u32 FirstDataSector = hdr->BPB_RsvdSecCnt + hdr->BPB_NumFATs * hdr->BPB_FATSz32 + RootDirSectors; 
  u32 NextSector = ((ClusId - 2) * hdr->BPB_SecPerClus) + FirstDataSector;
  u32 NextDirAddr = NextSector * hdr->BPB_BytsPerSec;
  struct fat32dir *nextdir = (struct fat32dir *)((char *)hdr + NextDirAddr);

  return nextdir;
}

u32 DirToClus(struct fat32dir*dir){
  u32 ClusId = (dir->DIR_FstClusHI << 16) + dir->DIR_FstClusLO;
  return ClusId;
}