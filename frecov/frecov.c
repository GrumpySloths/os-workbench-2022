#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "fat32.h"
#include "bitmap.h"
#include "queue.h"
#include <stdbool.h>

//构建枚举类,0:dentrys 1:bmp header 2:bmp data 3:free cluster
typedef enum{
  DENTRYS=0,
  BMP_HEADER=1,
  BMP_DATA=2,
  FREE_CLUSTER=3
}cluster_type;

void *map_disk(const char *fname);
struct fat32dir * get_RootDir(fat32hdr *hdr);
struct fat32dir *ClusToDir(fat32hdr *hdr,int ClusId);
u32 DirToClus(fat32dir*dir);
u32 NextClus(fat32hdr *hdr, u32 ClusId);
void FileSch(fat32hdr*hdr,fat32dir*dir,char*dirpath);
void dfs(fat32hdr *hdr, u32 cluster, u32 isdir);
void scan(fat32hdr *hdr,Queue*queue,u32*cluster_status);

static int EntCnt = 0;
static u32 NextCluster = 0;
static fat32Info_t *fat32Info=NULL;

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
  u32 FirstDataSector = hdr->BPB_RsvdSecCnt + hdr->BPB_NumFATs * hdr->BPB_FATSz32 + fat32Info->RootDirSectors;
  u32 FirstDirAddr = FirstDataSector * hdr->BPB_BytsPerSec;
  void* fstclusAddr=(void*)((char*)hdr+FirstDirAddr);

  fat32Info=&(struct fat32Info_t){.CountOfClusters=CountOfClusters,.DataSec=DataSec,
                                        .FATSz=FATSz,.RootDirSectors=RootDirSectors,
                                        .TotSec=TotSec,.fstclusAddr=fstclusAddr};

  printf("CountOfClusters: %d\n", CountOfClusters);

  char path[20] = "./res/";

#ifdef DEBUG
  printf("CountOfClusters: %d\n", CountOfClusters);
  //打印BPB_FATSz32
  printf("BPB_FATSz32: %d\n", hdr->BPB_FATSz32);
  //获取FSInfo的地址并将其属性进行打印
  printf("BPB_FSInfo: %d\n", hdr->BPB_FSInfo);
#endif

  struct FSInfo *fsi = (struct FSInfo *)((char *)hdr + hdr->BPB_FSInfo * hdr->BPB_BytsPerSec);

#ifdef DEBUG  
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
  NextCluster = DirToClus(rootdir);
  struct fat32dir*nextdir=ClusToDir(hdr,NextCluster);


  u32 FATValue=NextClus(hdr,NextCluster);

  //根据FaTValue 循环打印fat list，直至遇到ENDOFFILE
  while (FATValue <CLUS_INVALID) {
    printf("Entry value: %x\n", FATValue );
    FATValue = NextClus(hdr, FATValue);
  }

  printf("Entry value: %x\n", FATValue );
  // 打印nextdir文件大小
  printf("NextDir filesize: %d\n", nextdir->DIR_FileSize);
  struct fat32dir* temp = nextdir;

  dfs(hdr, NextCluster, 1);
#endif

  Queue *queue_dirs = (Queue *)malloc(sizeof(Queue));
  initQueue(queue_dirs);
  //构建长度为CountOfClusters的数组，用于记录每个cluster的状态
  u32 *cluster_status = (u32 *)malloc(sizeof(u32) * CountOfClusters);
  memset(cluster_status, 0, sizeof(u32) * CountOfClusters);

  scan(hdr,queue_dirs,cluster_status);
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

//判断cluster是否属于目录项
bool Isdentrys(fat32hdr*hdr,void*clusaddr){
    fat32dir*dirs=(fat32dir*)clusaddr;
    //遍历dirs,如果有3个以上dir name包含"BMP"，则返回true，否则返回false
    int cnt=0;
    char *substr = "BMP";
    for(int i=0;i<hdr->BPB_BytsPerSec*hdr->BPB_SecPerClus/sizeof(fat32dir);i++){
        char *name=(char*)dirs[i].DIR_Name;
        //容错处理，判断name末尾是否是'\0'
        if(name[strlen(name)]!='\0') continue;
        if(name[0]==0x00||name[0]==0xe5) continue;
        if(dirs[i].DIR_Attr==ATTR_LONG_NAME)
            continue;

        if(strncmp(name+8,substr,3)==0){
            printf("name:%s\n", name);
            cnt++;
        }
    }

    return cnt>=6;

}
//逐cluster 扫描data section
void scan(fat32hdr*hdr,Queue*queue,u32*cluster_status){
  //获取data section的首地址
  u32 FirstDataSector = hdr->BPB_RsvdSecCnt + hdr->BPB_NumFATs * hdr->BPB_FATSz32 + fat32Info->RootDirSectors;
  u32 FirstDirAddr = FirstDataSector * hdr->BPB_BytsPerSec;
  char* fstclusAddr=(char*)hdr+FirstDirAddr;
  // char*fstclusAddr=fat32Info->fstclusAddr;

  u32 clusSize=hdr->BPB_BytsPerSec*hdr->BPB_SecPerClus;

  int cnt=0;

  for (void *clusaddr = fstclusAddr; cnt< fat32Info->CountOfClusters;clusaddr+=clusSize,cnt++){
    //对每个cluster进行分类
    if(((bitmap_file_header*)clusaddr)->bfType==0x4d42){
      //bitmap file
      bitmap_file_header *bfh=(bitmap_file_header*)clusaddr;
      cluster_status[cnt] = BMP_HEADER;
      continue;
    }
    //判断是否是dentrys
    if(Isdentrys(hdr,clusaddr)){
      cluster_status[cnt]=DENTRYS;
      enqueue(queue, cnt);
      printf("dentrys cluster:%d\n", cnt);
      continue;
    }
  }
}
void get_longname(fat32longdir*longdir,int n,char*longname){
    int cur=(n-1)*LDIR_NAME_LENGTH;

    for(int i=0;i<5;i++){
        longname[cur++]=longdir->LDIR_Name1[i];
    }
    for (int i = 0; i < 6;i++){
        longname[cur++] = longdir->LDIR_Name2[i];
    }
    for (int i = 0; i < 2;i++){
        longname[cur++] = longdir->LDIR_Name3[i];
    } 
}

void dfs(fat32hdr*hdr,u32 cluster,u32 isdir){

    char longname[255];

    for (; cluster < CLUS_INVALID; cluster = NextClus(hdr, cluster)) {
        if(isdir){
          int ndents = (hdr->BPB_SecPerClus * hdr->BPB_BytsPerSec) /
                        sizeof(struct fat32dir);
          fat32dir *dirs = ClusToDir(hdr, cluster);

          for (int d = 0; d < ndents; d++) {
              if (dirs[d].DIR_Name[0] == 0x00)
                  break;
              if (dirs[d].DIR_Name[0] == 0xe5 ||
                  dirs[d].DIR_Name[0]=='.')
                  continue;
              //long name condition 
              if(dirs[d].DIR_Attr==ATTR_LONG_NAME){
                int n;
                fat32longdir *longdir = (fat32longdir *)&dirs[d];
                if (longdir->LDIR_Ord & Last_Long_Entry) {
                    n=longdir->LDIR_Ord ^ Last_Long_Entry;
                }else{
                    n = longdir->LDIR_Ord;
                }
                get_longname(longdir,n,longname);
                continue;
              }
              // 打印name
              int filesize = dirs[d].DIR_FileSize / 1024;
              int ClusSize=hdr->BPB_BytsPerSec*hdr->BPB_SecPerClus;
              int cluscnt=ROUNDUP(dirs[d].DIR_FileSize,ClusSize)/(hdr->BPB_BytsPerSec*hdr->BPB_SecPerClus);

              printf("Short name: %s  long name:%s  cnt:%d\n", dirs[d].DIR_Name,
                     longname, EntCnt++);
              printf("file size:%d kb, cluster count:%d\n", dirs[d].DIR_FileSize/1024,cluscnt);
              FileSch(hdr, &dirs[d], "./res/");
              dfs(hdr, DirToClus(&dirs[d]), 0);
          }
        }else{
          printf(" #%d ", cluster);
        }
    }
    printf("\n");
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
// 依据FAT表的值，返回下一个cluster的地址
u32 NextClus(struct fat32hdr*hdr,u32 ClusId){


  u32 *FAT = (u32 *)((char *)hdr + hdr->BPB_RsvdSecCnt * hdr->BPB_BytsPerSec);
  u32 FATValue = FAT[ClusId];

  return FATValue;
}

void get_filename(fat32dir *dir, char *name) { 
    int cur = 0;
    for (int i = 0; i < 11;i++){
      if(i==8)
          name[cur++] = '.';
      if(dir->DIR_Name[i]!=' ')
          name[cur++] = dir->DIR_Name[i];
    }
    name[cur] = '\0';
}
void FileSch(fat32hdr*hdr,fat32dir*dir,char*dirpath){
    //获取dir name，并添加.bmp后缀
    if(dir->DIR_Name[0]=='.'||dir->DIR_Name[0]==0xe5
        ||dir->DIR_Name[0]==0x00)
        return;
    char name[30];
    int cluscnt = 0;
    strcpy(name, dirpath);
    get_filename(dir, name+strlen(dirpath));


    FILE *fp=fopen(name,"wb");

    u32 fstclus=DirToClus(dir);
    while(fstclus<CLUS_INVALID){
        if(fstclus>=RESERVED_START && fstclus<=RESERVED_END){
            printf("Reserved cluster\n");
            break;
        }
        printf("##%d  ", fstclus);
        cluscnt++;
        // 将fstclus中的内容写入文件
        void *head = (void *)ClusToDir(hdr,fstclus);
        fwrite(head,hdr->BPB_BytsPerSec,hdr->BPB_SecPerClus,fp);

        fstclus=NextClus(hdr,fstclus);
    }
    printf("\n");
    printf("cluscnt:%d\n", cluscnt);
    fclose(fp);
}