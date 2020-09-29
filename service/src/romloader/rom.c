#include <math.h>
#include "../gb/gb.h"

#include "rom.h"

const char *romTypeString[256] = {
    [ROM_PLAIN] = "ROM_PLAIN",
    [ROM_MBC1] = "ROM_MBC1",
    [ROM_MBC1_RAM] = "ROM_MBC1",
    [ROM_MBC1_RAM_BATT] = "ROM_MBC1_RAM_BATT",
    [ROM_MBC2] = "ROM_MBC2",
    [ROM_MBC2_BATTERY] = "ROM_MBC2_BATTERY",
    [ROM_RAM] = "ROM_RAM",
    [ROM_RAM_BATTERY] = "ROM_RAM_BATTERY",
    [ROM_MMM01] = "ROM_MMM01",
    [ROM_MMM01_SRAM] = "ROM_MMM01_SRAM",
    [ROM_MMM01_SRAM_BATT] = "ROM_MMM01_SRAM_BATT",
    [ROM_MBC3_TIMER_BATT] = "ROM_MBC3_TIMER_BATT",
    [ROM_MBC3_TIMER_RAM_BATT] = "ROM_MBC3_TIMER_RAM_BATT",
    [ROM_MBC3] = "ROM_MBC3",
    [ROM_MBC3_RAM] = "ROM_MBC3_RAM",
    [ROM_MBC3_RAM_BATT] = "ROM_MBC3_RAM_BATT",
    [ROM_MBC5] = "ROM_MBC5",
    [ROM_MBC5_RAM] = "ROM_MBC5_RAM",
    [ROM_MBC5_RAM_BATT] = "ROM_MBC5_RAM_BATT",
    [ROM_MBC5_RUMBLE] = "ROM_MBC5_RUMBLE",
    [ROM_MBC5_RUMBLE_SRAM] = "ROM_MBC5_RUMBLE_SRAM",
    [ROM_MBC5_RUMBLE_SRAM_BATT] = "ROM_MBC5_RUMBLE_SRAM_BATT",
    [ROM_POCKET_CAMERA] = "ROM_POCKET_CAMERA",
    [ROM_BANDAI_TAMA5] = "ROM_BANDAI_TAMA5",
    [ROM_HUDSON_HUC3] = "ROM_HUDSON_HUC3",
    [ROM_HUDSON_HUC1] = "ROM_HUDSON_HUC1",
};

// XXX this should probably have a trivial stack bof?
unsigned char loadROM(gameboy_t *gb, uint8_t *data, size_t size, char **log) {
  //char name[17];
  char *name;
  enum romType type;
  int romSize;
  int ramSize;

  size_t length;
  char *lz = *log;
  lz[0] = 0;

  unsigned char header[0x180];
  length = size;
  if (length < 0x180) {

    strcat(lz, "rom too small...");
    //return 0;
  }

  memcpy(header, data, 0x180);

  /*for (i = 0; i < 16; i++) {
    if (header[i + ROM_OFFSET_NAME] == 0x80 ||
        header[i + ROM_OFFSET_NAME] == 0xc0)
      name[i] = '\0';
    else
      name[i] = header[i + ROM_OFFSET_NAME];
  }*/
  name = strdup ((char *)&header[ROM_OFFSET_NAME]);
  sprintf(lz + strlen(lz), "Internal ROM name: ");
  sprintf(lz + strlen(lz), name);
  sprintf(lz + strlen(lz), "\n");
  free (name);

  type = header[ROM_OFFSET_TYPE];

  if (!romTypeString[type]) {
    printf("Unknown ROM type: %#02x\n", type);
    return 0;
  }

  sprintf(lz + strlen(lz), "ROM type: %s\n", romTypeString[type]);

  if (type != ROM_PLAIN) {
    sprintf(lz + strlen(lz), "Only 32KB games with no mappers are supported!\n");
    return 0;
  }

  romSize = header[ROM_OFFSET_ROM_SIZE];

  if ((romSize & 0xF0) == 0x50)
    romSize = (int)pow(2.0, (double)(((0x52) & 0xF) + 1)) + 64;
  else
    romSize = (int)pow(2.0, (double)(romSize + 1));

  sprintf(lz + strlen(lz), "ROM size: %dKB\n", romSize * 16);
  volatile uint8_t rom_buf[romSize * 16 * 1024];
  memcpy((void *)rom_buf, data, size);

  if (romSize * 16 != 32) {
    sprintf(lz + strlen(lz), "Only 32KB games with no mappers are supported!\n");
    //return 0;
  }

  if (length != romSize * 16 * 1024) {
	  sprintf(lz + strlen(lz), "ROM filesize does not equal ROM size!\n");
    // fclose(f);
    // return 0;
  }

  ramSize = rom_buf[ROM_OFFSET_RAM_SIZE];

  ramSize = (int)pow(4.0, (double)(ramSize)) / 2;

  sprintf (lz + strlen(lz), "RAM size: %dKB\n", ramSize);

  ramSize = ceil(ramSize / 8.0f);

  /*cart = malloc(length);
  if(!cart) {
          printf("Could not allocate memory!\n");
          fclose(f);
          return 0;
  }*/

  memcpy(gb->memory.cart, data, length);

  return 1;
}

void unloadROM(void) {
  // free(cart);
}
