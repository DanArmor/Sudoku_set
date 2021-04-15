//The source file of custom C library that provides the implementation of numerical set by bit-vectors
//Made by DanArmor
//https://github.com/DanArmor

#define __BITSET_LIB__ //Flag for compilation of library

/*
The list of possible flags:
  ** __BITSET_32__        //If you need to force 32-bit blocks of set
  ** __BITSET_64__        //If you need to force 64-bit blocks of set
  ** __BITSET_SAFE_MODE__ //Forces exit while take any of errors 
  ** __BITSET_OMP__       //forces to use OpenMP and threads
  Using of those flags requires re-compilation of bitset.c with it
*/

    //TODO #define __BITSET_NAMESPACE__

//Flags:
// #define __BITSET_32__
// #define __BITSET_64__
// #define __BITSET_SAFE_MODE__
// #define __BITSET_OMP__

#include "bitset_dev.h"
//You can find the definitions in header of that library (bitset.h)
//Here are details of functionality

/*
  If it possible - the first block of function is error-catcher with two possibilities:
  ** return if some error was taken
  ** OR exit with __BITSET_ERROR_CODE__ if __BITSET_SAFE_MODE__ is defined
*/

const uint SIZE_UINT_BYTES = sizeof(uint);
const uint SIZE_UINT_BITS = sizeof(uint) * 8;
const uint MASK_LAST_LEFT = ~((uint)~0 >> 1);

#ifdef __BITSET_OMP__
  void __BITSET_SET_THREADS_NUMBER__(void)
  {
    omp_set_num_threads(omp_get_max_threads() / 2);
  }
#else
  void __BITSET_SET_THREADS_NUMBER__(void)
  {
    return;
  }
#endif

//Returns 1, if value is in U. Else = 0
static int is_value_in_U(Bit_set *set, long long value){
  if(value < set -> min || value > set -> max){
    return 0;
  }
  return 1;
}

//Catches issues, that connected with values edges of U
static int catch_U_set_edges_err(Bit_set *set, long long value){
  if(!(is_value_in_U(set, value))){
    putserr("Error with edges of Universal Set! Aborted. ");
    printerr_value(value);
    fprintf(stderr, "; MIN:%lld    MAX:%lld\n", set -> min, set -> max);
    #ifdef __BITSET_SAFE_MODE__
      exit(__BITSET_ERROR_CODE__);
    #else
      return 1;
    #endif
  }
  return 0;
}


//Catches issues, that connected with min/max of U.
static int catch_min_over_max_error(long long min, long long max){
  if(max < min){
    putserr("Min bigger then max!\n");
    #ifdef __BITSET_SAFE_MODE__
      exit(__BITSET_ERROR_CODE__);
    #else
      return 1;
    #endif
  }
  return 0;
}

Bit_set create_set(long long min, long long max){
  if(catch_min_over_max_error(min, max)){
      putserr("Returned special error set\n");
      Bit_set set_error = {NULL, 0, 1, 0, 0};
      return set_error;
  }
  Bit_set set = {NULL, 0, max - min + 1, min, max};
  set.values = (uint*)calloc((max - min) / SIZE_UINT_BITS + 1, SIZE_UINT_BYTES);
  return set;
}

void add_set(Bit_set *set, long long value){
  if(catch_U_set_edges_err(set, value)){
    return;
  }
  uint value_i = value - set -> min;
  if((set -> values[value_i / SIZE_UINT_BITS] & MASK_LAST_LEFT >> value_i % SIZE_UINT_BITS) == 0) set -> amount++;
  set -> values[value_i / SIZE_UINT_BITS] |= MASK_LAST_LEFT >> value_i % SIZE_UINT_BITS;
}

int is_in_set(Bit_set *set, long long value){
  if(is_value_in_U(set, value)){
    uint value_i = value - set -> min;
    if(set->values[value_i / SIZE_UINT_BITS] & MASK_LAST_LEFT >> value_i % SIZE_UINT_BITS){
      return 1;
    }
  }
  return 0;
}

void delete_from_set(Bit_set *set, long long value){
  if(catch_U_set_edges_err(set, value)){
    return;
  }
  uint value_i = value - set -> min;
  if(set -> values[value_i / SIZE_UINT_BITS] & MASK_LAST_LEFT >> value_i % SIZE_UINT_BITS) set -> amount--;
  set -> values[value_i / SIZE_UINT_BITS] &= ~(MASK_LAST_LEFT >> value_i % SIZE_UINT_BITS);
}

void output_set(Bit_set *set){
  char is_empty = 1;
  uint mask;
  uint max_blocks = (set -> max - set -> min) / SIZE_UINT_BITS;
  uint max_elements = set -> amount;
  printf("{ ");
  for(r_uint i = 0; i <= max_blocks && max_elements; i++){
    if(set -> values[i] == 0){
      continue;
    }
    is_empty = 0;
    mask = MASK_LAST_LEFT;
    for(r_uint j = 0; j < SIZE_UINT_BITS && max_elements; j++){
      if(mask & set -> values[i]){
        max_elements--;
        printf("%lld, ", set -> min + i * SIZE_UINT_BITS + j);
      }
      mask >>= 1;
    }
  }
  if(is_empty)
    printf("\b}");
  else
    printf("\b\b }");
}

void complement_set(Bit_set *set){
  uint max_blocks = (set -> max - set -> min) / SIZE_UINT_BITS;
  #ifdef __BITSET_OMP__
    #pragma omp parallel for
  #endif
  for(r_uint i = 0; i <= max_blocks; i++){
    set -> values[i] = ~set -> values[i];
  }
  set -> amount = set -> max_amount - set -> amount;
}

void destroy_set_values(Bit_set *set){
  free(set -> values);
}
