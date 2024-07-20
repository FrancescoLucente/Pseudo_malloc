#include <assert.h>
#include "bit_map.h"
#include <stdio.h>

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
  return (bits+7)/8;
}

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, char* buffer){
  if (!buffer) {
        printf("Errore: buffer non allocato\n");
        return;
    }
    printf("BitMap_init: buffer address = %p, num_bits = %d\n", (void *)buffer, num_bits);
    bit_map->buffer = buffer;
    bit_map->num_bits = num_bits;
    bit_map->buffer_size = BitMap_getBytes(num_bits);
    printf("BitMap inizializzata con num_bits: %d, buffer_size: %d\n", num_bits, bit_map->buffer_size);
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap *bit_map, int bit_num, int status) {
  // get byte
  int byte_num = bit_num >> 3;
  assert(byte_num < bit_map->buffer_size);
  int bit_in_byte = byte_num & 0x03;
  if (status) {
    bit_map->buffer[byte_num] |= (1 << bit_in_byte);
  } else {
    bit_map->buffer[byte_num] &= ~(1 << bit_in_byte);
  }
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap *bit_map, int bit_num) {
  int byte_num = bit_num >> 3;
  assert(byte_num < bit_map->buffer_size);
  int bit_in_byte = byte_num & 0x03;
  return (bit_map->buffer[byte_num] & (1 << bit_in_byte)) != 0;
}

void BitMap_print(BitMap* bitmap){
  int blocks=0;
  for(int i = 0; i < bitmap->num_bits; ++i){
    if(BitMap_bit(bitmap, i)==1) blocks++;
  }
  printf("blocks alloocated:%d\n",blocks);
}

