/*  Copyright (c) 2023  Hunter Whyte
    See page 5077 of AM355x TRM "26.1.11 Table of Contents" 
    In order to boot using MMC in raw mode a 'table of contents' (TOC) header is
    needed, the exact contents of this header are outlined in the TRM, this is
    a small program to generate the TOC header in a slightly nicer way, rather
    than just using a hex editor.
    The TOC is 512 bytes long and consists of a maximum of 2 TOC items 
    (32 bytes long each), located one after the other. The second TOC item must
    be filled by 0xFF. The rest of the 512 byte TOC must be filled with zeroes.
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void write_repeated(uint8_t value, uint32_t repeats, FILE* fp) {
  for (int i = 0; i < repeats; i++) {
    fwrite(&value, 1, 1, fp);
  }
}

int main(int argc, char** argv) {

  if (argc != 2) {
    printf("argument of destination required\n");
    printf("usage: ./gen_raw <output.img>\n");
    return 0;
  }


  /* 12 bytes long including null terminator*/
  const char* filename = "CHSETTINGS";

  /* start and size */
  uint32_t start = 0x00000040;
  uint32_t size = 0x0000000C;

  /* magic values */
  uint32_t magic1 = 0xC0C0C0C1;
  uint32_t magic2 = 0x00000100;

  FILE* fout;
  fout = fopen(argv[1], "wb");

  /* start of TOC */
  /* 0x00: start */
  fwrite(&start, 4, 1, fout); /* offset 0x00 */
  /* 0x04: size */
  fwrite(&size, 4, 1, fout);
  /* 0x08-0x10: 0x00 */
  write_repeated(0, 12, fout);
  /* 0x14: CHSETTINGS */
  fwrite(filename, 11, 1, fout);
  write_repeated(0, 1, fout);
  /* 0x20-0x40 0xFF */
  write_repeated(0xFF, 32, fout);
  /* 0x40: 0xC0C0C01*/
  fwrite(&magic1, 4, 1, fout); /* write offset 0x40 with magic number 1*/
  /* 0x44: 0x00000100*/
  fwrite(&magic2, 4, 1, fout); /* write offset 0x44 with magic number 1*/
  /* 0x48-0x200: 0x00 */
  write_repeated(0, 440, fout); /* write the remaining bytes with 0 */
  /* end of TOC */

  fclose(fout);

  return 0;
}