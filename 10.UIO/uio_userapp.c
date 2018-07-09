#include  <stdlib.h>
#include  <stdio.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <sys/io.h>

int main()
{
  int fd;
  unsigned int nint;
  if ((fd = open("/dev/uio0", O_RDONLY)) < 0) 
  {
    perror("Failed to open /dev/uio0\n");
    exit(EXIT_FAILURE);
  }
  while (read(fd, &nint, sizeof(nint)) >= 0)
  printf("%s:Interrupts: %d\n",__func__,  nint);
  return 0;
}
