#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

int main(int argc, char **argv) {

  int h = 200, w = 320;
  int buf_size = h * w * sizeof(u8);

  u8 *buf = malloc(buf_size);

  printf("buf: w=%d x h=%d (%d B), ptr=%p\n", w, h, buf_size, buf);

  if (buf) free(buf);
  return 0;
}
