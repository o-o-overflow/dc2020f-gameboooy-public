#include "../gb/gb.h"
#include "rpc/client.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <ctype.h>
#include <stdio.h>

using std::fstream;
using std::string;
using std::vector;

uint16_t key_to_short(keys_t key) {
	uint16_t k;
	memcpy(&k, &key, sizeof(keys_t));
	return k;
}

void hexdump(void *pAddressIn, long lSize) {
  char szBuf[100];
  long lIndent = 1;
  long lOutLen, lIndex, lIndex2, lOutLen2;
  long lRelPos;
  struct {
    char *pData;
    unsigned long lSize;
  } buf;
  unsigned char *pTmp, ucTmp;
  unsigned char *pAddress = (unsigned char *)pAddressIn;

  buf.pData = (char *)pAddress;
  buf.lSize = lSize;

  while (buf.lSize > 0) {
    pTmp = (unsigned char *)buf.pData;
    lOutLen = (int)buf.lSize;
    if (lOutLen > 16)
      lOutLen = 16;

    // create a 64-character formatted output line:
    sprintf(szBuf,
            " >                            "
            "                      "
            "    %08lX",
            pTmp - pAddress);
    lOutLen2 = lOutLen;

    for (lIndex = 1 + lIndent, lIndex2 = 53 - 15 + lIndent, lRelPos = 0;
         lOutLen2; lOutLen2--, lIndex += 2, lIndex2++) {
      ucTmp = *pTmp++;

      sprintf(szBuf + lIndex, "%02X ", (unsigned short)ucTmp);
      if (!isprint(ucTmp))
        ucTmp = '.'; // nonprintable char
      szBuf[lIndex2] = ucTmp;

      if (!(++lRelPos & 3)) // extra blank after 4 bytes
      {
        lIndex++;
        szBuf[lIndex + 2] = ' ';
      }
    }

    if (!(lRelPos & 3))
      lIndex--;

    szBuf[lIndex] = '<';
    szBuf[lIndex + 1] = ' ';

    printf("%s\n", szBuf);

    buf.pData += lOutLen;
    buf.lSize -= lOutLen;
  }
}

vector<uint8_t> vec_from_gb(gameboy_t gb) {
  vector<uint8_t> v;
  uint8_t *p = (uint8_t *)&gb;
  for (int i = 0; i < sizeof(gameboy_t); i++) {
    v.push_back(p[i]);
  }
  return v;
}

vector<uint8_t> vec_from_key(uint16_t key) {
  vector<uint8_t> v;
  uint8_t *p = (uint8_t *)&key;
  for (int i = 0; i < sizeof(keys_t); i++) {
    v.push_back(p[i]);
  }
  return v;
}

gameboy_t gb_from_vec(vector<uint8_t> vec) {
  gameboy_t gb;
  memcpy(&gb, &vec[0], sizeof(gameboy_t));
  return gb;
}

void dumpregs(vector<uint8_t> vec) {
  gameboy_t gb = gb_from_vec(vec);
  printf("PC: 0x%x\n", gb.cpu.registers.PC);
  printf("SP: 0x%x\n", gb.cpu.registers.SP);
}

int main(int argc, char **argv) {
  int fwport = 0;
  char *host;
  if (argc == 4) {
    host = argv[2];
    fwport = atoi(argv[3]);
  } else if (argc == 3) {
    host = argv[2];
  }
  std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) {
    return 0;
  }
  vector<uint8_t> cart;
  for (int i = 0; i < size; i++) {
    cart.push_back(buffer.data()[i]);
  }
  gameboy_t gb;
  rpc::client input(host, fwport ? fwport : INPUT_SERVER);
  keys_t keyup;
  keys_t keydown;
  keys_t keyleft;
  keys_t keyright;
  keys_t keyb;
  keys_t keya;
  keys_t keystart;
  keys_t keyselect;
  memset(&keyup, 0, sizeof(keyup));
  memset(&keydown, 0, sizeof(keyup));
  memset(&keyleft, 0, sizeof(keyup));
  memset(&keyright, 0, sizeof(keyup));
  memset(&keyb, 0, sizeof(keyup));
  memset(&keya, 0, sizeof(keyup));
  memset(&keystart, 0, sizeof(keyup));
  memset(&keyselect, 0, sizeof(keyup));
  keyup.KEY2.up = 1;
  keydown.KEY2.down = 1;
  keyleft.KEY2.left = 1;
  keyright.KEY2.right = 1;
  keyb.KEY1.b = 1;
  keya.KEY1.a = 1;
  keystart.KEY1.start = 1;
  keyselect.KEY1.select = 1;
  uint16_t up = key_to_short(keyup);
  uint16_t down = key_to_short(keydown);
  uint16_t left = key_to_short(keyleft);
  uint16_t right = key_to_short(keyright);
  uint16_t b = key_to_short(keyb);
  uint16_t a = key_to_short(keya);
  uint16_t start = key_to_short(keystart);
  uint16_t select = key_to_short(keyselect);
  std::vector<uint8_t> result = vec_from_gb(gb);
  result = input.call("input::input", vec_from_gb(gb), vec_from_key(start & select & down & right))
               .as<vector<uint8_t>>();
  result = input.call("input::input", vec_from_gb(gb), vec_from_key(right & start & a))
               .as<vector<uint8_t>>();
  result = input.call("input::input", vec_from_gb(gb), vec_from_key(right & select & a))
               .as<vector<uint8_t>>();
  result = input.call("input::input", vec_from_gb(gb), vec_from_key(right & left & start & b & select))
               .as<vector<uint8_t>>();
  result = input.call("input::input", vec_from_gb(gb), vec_from_key(start))
               .as<vector<uint8_t>>();
  const char *ch = (const char *)result.data() + sizeof(gameboy_t);
  std::string s (ch, result.size() - sizeof(gameboy_t));
  std::cout << s << std::endl;
}
