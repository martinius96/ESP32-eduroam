// ---------------------------------------------------------------------------
// Created by Abhijit Bose (boseji) on 22/02/16.
// Copyright 2016 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file rBase64.cpp
//
// @brief 
// Library to provide the BASE64 conversion and vis-versa
//
// @version API 1.1.0 - Adding Generics 20th Mar 2018
//                (Breaking Backward Compatibility due to Memory constrains)
// @version API 1.0.0 - Initial API
//
//
// @author boseji - salearj@hotmail.com
// ---------------------------------------------------------------------------

#include "Arduino.h"
#include "rBase64.h"

const char b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// Internal Buffer
char buf[128];
/* 'Private' declarations */
inline void a3_to_a4(unsigned char * a4, unsigned char * a3);
inline void a4_to_a3(unsigned char * a3, unsigned char * a4);
inline unsigned char b64_lookup(char c);

size_t rbase64_encode(char *output, char *input, size_t inputLen) {
  int i = 0, j = 0;
  size_t encLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];

  while(inputLen--) {
    a3[i++] = *(input++);
    if(i == 3) {
      a3_to_a4(a4, a3);

      for(i = 0; i < 4; i++) {
        output[encLen++] = b64_alphabet[a4[i]];
      }

      i = 0;
    }
  }

  if(i) {
    for(j = i; j < 3; j++) {
      a3[j] = '\0';
    }

    a3_to_a4(a4, a3);

    for(j = 0; j < i + 1; j++) {
      output[encLen++] = b64_alphabet[a4[j]];
    }

    while((i++ < 3)) {
      output[encLen++] = '=';
    }
  }
  output[encLen] = '\0';
  return encLen;
}

size_t rbase64_decode(char * output, char * input, size_t inputLen) {
  int i = 0, j = 0;
  size_t decLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];


  while (inputLen--) {
    if(*input == '=') {
      break;
    }

    a4[i++] = *(input++);
    if (i == 4) {
      for (i = 0; i <4; i++) {
        a4[i] = b64_lookup(a4[i]);
      }

      a4_to_a3(a3,a4);

      for (i = 0; i < 3; i++) {
        output[decLen++] = a3[i];
      }
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) {
      a4[j] = '\0';
    }

    for (j = 0; j <4; j++) {
      a4[j] = b64_lookup(a4[j]);
    }

    a4_to_a3(a3,a4);

    for (j = 0; j < i - 1; j++) {
      output[decLen++] = a3[j];
    }
  }
  output[decLen] = '\0';
  return decLen;
}

size_t rbase64_enc_len(size_t plainLen) {
  size_t n = plainLen;
  return (n + 2 - ((n + 2) % 3)) / 3 * 4;
}

size_t rbase64_dec_len(char * input, size_t inputLen) {
  uint32_t i = 0;
  size_t numEq = 0;
  for(i = inputLen - 1; input[i] == '='; i--) {
    numEq++;
  }

  return (size_t)(((6 * inputLen) / 8) - numEq);
}

inline void a3_to_a4(unsigned char * a4, unsigned char * a3) {
  a4[0] = (a3[0] & 0xfc) >> 2;
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[3] = (a3[2] & 0x3f);
}

inline void a4_to_a3(unsigned char * a3, unsigned char * a4) {
  a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
  a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
  a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

inline unsigned char b64_lookup(char c) {
  int i;
  for(i = 0; i < 64; i++) {
    if(b64_alphabet[i] == c) {
      return i;
    }
  }

  return -1;
}

/* Declaring the Main Class Instance */
rBase64generic<RBASE64_MAX_DEFAULT_SIZE> rbase64;
