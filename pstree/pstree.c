#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])
{
  printf("argc=%d \n", argc);
  for (int i = 0; i < argc; i++)
  {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("hello world\n");
  // printf("hello world");
  assert(!argv[argc]);
  char *folderPath = "/proc/";
  listSubfolders(folderPath);
  // pid_t pid = getpid();
  // printf("pid:%d \n", pid);

  return 0;
}

void listSubfolders(const char *folderPath)
{
  DIR *dir = opendir(folderPath);
  if (dir == NULL)
  {
    perror("Error opening folder");
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_DIR)
    { // Check if it's a directory
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
      {
        printf("%s\n", entry->d_name);
      }
    }
  }

  closedir(dir);
}
