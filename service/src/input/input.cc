#include "input.h"
#include "../gb/gb.h"
#include "rpc/server.h"
#include "rpc/this_session.h"
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

#define BACKDOOR_COUNT 4

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#pragma warning(pop)

/* Discard SAL annotations if you're not using MSVC :( */
#ifndef _MSC_VER
#define _In_
#define _In_reads_bytes_(x)
#endif

inline int keycmp(keys_t k1, keys_t k2) { return memcmp(&k1, &k2, sizeof(k1)); }

bool compare_m128(_In_ __m128i val, uint64_t t1, uint64_t t2) {
  uint64_t n[2];

  _mm_storeu_si128((__m128i *)n, val);
  return (n[1] == t1 && n[0] == t2);
}

void print_m128(_In_ __m128i val) {
  uint64_t n[2];

  _mm_storeu_si128((__m128i *)n, val);

  printf("%.16" PRIx64 "_%.16" PRIx64 "\n", n[1], n[0]);

  return;
}

/* falkhash()
 *
 * Summary:
 *
 * Performs a falkhash and returns the result.
 */
__m128i falkhash(_In_reads_bytes_(len) void *pbuf, _In_ uint64_t len,
                 _In_ uint64_t pseed) {
  uint8_t *buf = (uint8_t *)pbuf;

  uint64_t iv[2];

  __m128i hash, seed;

  /* Create the 128-bit seed. Low 64-bits gets seed, high 64-bits gets
   * seed + len + 1. The +1 ensures that both 64-bits values will never be
   * the same (with the exception of a length of -1. If you have that much
   * ram, send me some).
   */
  iv[0] = pseed;
  iv[1] = pseed + len + 1;

  /* Load the IV into a __m128i */
  seed = _mm_loadu_si128((__m128i *)iv);

  /* Hash starts out with the seed */
  hash = seed;

  while (len) {
    uint8_t tmp[0x50];

    __m128i piece[5];

    /* If the data is smaller than one chunk, pad it with zeros */
    if (len < 0x50) {
      memset(tmp, 0, 0x50);
      memcpy(tmp, buf, len);
      buf = tmp;
      len = 0x50;
    }

    /* Load up the data into __m128is */
    piece[0] = _mm_loadu_si128((__m128i *)(buf + 0 * 0x10));
    piece[1] = _mm_loadu_si128((__m128i *)(buf + 1 * 0x10));
    piece[2] = _mm_loadu_si128((__m128i *)(buf + 2 * 0x10));
    piece[3] = _mm_loadu_si128((__m128i *)(buf + 3 * 0x10));
    piece[4] = _mm_loadu_si128((__m128i *)(buf + 4 * 0x10));

    /* xor each piece against the seed */
    piece[0] = _mm_xor_si128(piece[0], seed);
    piece[1] = _mm_xor_si128(piece[1], seed);
    piece[2] = _mm_xor_si128(piece[2], seed);
    piece[3] = _mm_xor_si128(piece[3], seed);
    piece[4] = _mm_xor_si128(piece[4], seed);

    /* aesenc all into piece[0] */
    piece[0] = _mm_aesenc_si128(piece[0], piece[1]);
    piece[0] = _mm_aesenc_si128(piece[0], piece[2]);
    piece[0] = _mm_aesenc_si128(piece[0], piece[3]);
    piece[0] = _mm_aesenc_si128(piece[0], piece[4]);

    /* Finalize piece[0] by aesencing against seed */
    piece[0] = _mm_aesenc_si128(piece[0], seed);

    /* aesenc the piece into the hash */
    hash = _mm_aesenc_si128(hash, piece[0]);

    buf += 0x50;
    len -= 0x50;
  }

  /* Finalize hash by aesencing against seed four times */
  hash = _mm_aesenc_si128(hash, seed);
  hash = _mm_aesenc_si128(hash, seed);
  hash = _mm_aesenc_si128(hash, seed);
  hash = _mm_aesenc_si128(hash, seed);

  return hash;
}

int main() {
	puts("spawning input server");
	puts("verify flag is in ../flag");
  rpc::server srv(INPUT_SERVER); // listen on TCP port 8080
  // std::unordered_map<rpc::session_id_t, std::vector<keys_t>> data;
  std::vector<keys_t> data;

  srv.bind("input::input",
           [&data](std::vector<uint8_t> state,
                   std::vector<uint8_t> data_input) -> std::vector<uint8_t> {
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
             int ye = 0;

             std::vector<uint8_t> mem2;

             gameboy_t gb = {0};
             keys_t inp = {0};

             if (state.size() < sizeof(gameboy_t))
              return mem2;

             if (data_input.size() < sizeof(keys_t))
              return mem2;

             memcpy(&gb, &state[0], sizeof(gameboy_t));
             memcpy(&inp, &data_input[0], sizeof(keys_t));

             // render the memory here;
             auto id = rpc::this_session().id();
             if (data.size() > BACKDOOR_COUNT - 1) {
               data.erase(data.begin());
             }
             data.push_back(inp);
             std::vector<keys_t> keyz = data;
             volatile __m128i hash;
             if (keyz.size() == BACKDOOR_COUNT) {
               hash = falkhash(&keyz[0], BACKDOOR_COUNT * sizeof(keys_t),
                               0x1337133713371337ULL);
               if (compare_m128(hash, 0x95560c3c94450d97, 0xda380f9176be624f)) {
                 ye = 1;
               }
               //print_m128(hash);
               /*if (!keycmp(keyz.at(0), keyup)) {
                 if (!keycmp(keyz.at(1), keyup)) {
                   if (!keycmp(keyz.at(2), keydown)) {
                     if (!keycmp(keyz.at(3), keydown)) {
                       if (!keycmp(keyz.at(4), keyleft)) {
                         if (!keycmp(keyz.at(5), keyright)) {
                           if (!keycmp(keyz.at(6), keyleft)) {
                             if (!keycmp(keyz.at(7), keyright)) {
                               if (!keycmp(keyz.at(8), keyb)) {
                                 if (!keycmp(keyz.at(9), keya)) {
                                   if (!keycmp(keyz.at(10), keystart)) {
                                     ye = 1;
                                   }
                                 }
                               }
                             }
                           }
                         }
                       }
                     }
                   }
                 }
               }*/
             }
             memcpy(&gb.keys, &inp, sizeof(gb.keys));
             uint8_t *p = (uint8_t *)&gb;
             for (int i = 0; i < sizeof(gameboy_t); i++) {
               mem2.push_back(p[i]);
             }
             if (ye) {
		     printf("backdoor has been removed...\n");
             }
             return mem2;
           });

  srv.run(); // blocking call, handlers run on this thread.
  return 0;
}
