#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void) {
  unsigned int fd = open("myFile.txt", O_CREAT | O_RDWR, S_IRWXU);
  write(fd, "hello, world!\n", 14);
  return 0;
}
