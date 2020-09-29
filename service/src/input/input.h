#ifndef __INPUT_H_
#define __INPUT_H_
#define __LITTLE_ENDIAN__ 1
typedef struct keys1 {
#ifdef __LITTLE_ENDIAN__
  unsigned char a : 1;
  unsigned char b : 1;
  unsigned char select : 1;
  unsigned char start : 1;
#else
  unsigned char start : 1;
  unsigned char select : 1;
  unsigned char b : 1;
  unsigned char a : 1;
#endif
} keys1_t;

typedef struct keys2 {
#ifdef __LITTLE_ENDIAN__
  unsigned char right : 1;
  unsigned char left : 1;
  unsigned char up : 1;
  unsigned char down : 1;
#else
  unsigned char down : 1;
  unsigned char up : 1;
  unsigned char left : 1;
  unsigned char right : 1;
#endif
} keys2_t;

typedef struct keys {
  union {
    struct {
      union {
		keys1_t KEY1;
        unsigned char keys1 : 4;
      };

      union {
        keys2_t KEY2;
        unsigned char keys2 : 4;
      };
    };

    unsigned char c;
  };
} keys_t;
#endif
