#include <ps4.h>

#include "common.h"

#define PS4UPDATE_FILE "/update/PS4UPDATE.PUP"
#define PS4UPDATE_TEMP_FILE "/update/PS4UPDATE.PUP.net.temp"

void write_blob(const char *path, const void *blob, const size_t blobsz) {
  if (!path || !blob || blobsz == 0) {
    printf_notification("write_blob 参数无效");
    return;
  }

  if (file_exists(path)) {
    unlink(path);
  }
  int fd = open(path, O_CREAT | O_RDWR, 0777);
  printf_debug("文件描迸符 %s %d\n", path, fd);
  if (fd > 0) {
    ssize_t written = write(fd, blob, blobsz);
    if (written != (ssize_t)blobsz) {
      printf_notification("部分写入 %s: %zd/%zu 字节", path, written, blobsz);
    }
    close(fd);
  } else {
    printf_notification("写入 %s 失败！\n文件描述符 %d", path, fd);
  }
}

void kill_proc(const char *proc) {
  if (!proc) {
    return;
  }
  const int party = findProcess(proc);
  printf_debug("%s 进程 ID: %d\n", proc, party);
  if (party > 0) {
    const int k = kill(party, SIGKILL);
    printf_debug("发送 SIGKILL(%d) 给 %s(%d)\n", k, proc, party);
  }
}

void block_updates(void) {
  // Delete existing updates/blocker and recreate
  unlink(PS4UPDATE_FILE);
  rmdir(PS4UPDATE_FILE);
  mkdir(PS4UPDATE_FILE, 777);

  unlink(PS4UPDATE_TEMP_FILE);
  rmdir(PS4UPDATE_TEMP_FILE);
  mkdir(PS4UPDATE_TEMP_FILE, 777);

  // Unmount update directory. From etaHEN
  if ((int)unmount("/update", 0x80000LL) < 0) {
    unmount("/update", 0);
  }
}
