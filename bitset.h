/*
  bitset.h

    Made by DanArmor
    https://github.com/DanArmor

  The custom C library with implementation of numerical set by bit-vectors

*/

#pragma once
#ifndef __BITSET_DEFINED__ // Include guard for 3rd party interop
#define __BITSET_DEFINED__

#include <stdint.h>
#include <inttypes.h>

#if (defined(_WIN64) || defined(__BITSET_64__)) && !defined(__BITSET_32__)
  #define PARAMETER_ERR "%"PRIuFAST64
  typedef uint_fast64_t uint;
#else
  #define PARAMETER_ERR "%"PRIuFAST32
  typedef uint_fast32_t uint;
#endif

//Sets the max number of used threads as a half of max 
//Executes before main()
//If __BITSET_OMP__ was turned off - it will do nothing
extern void __BITSET_SET_THREADS_NUMBER__(void) __attribute__ ((constructor));

//We take that values from implementation (bitset.c)
extern const uint SIZE_UINT_BYTES;
extern const uint SIZE_UINT_BITS;
extern const uint MASK_LAST_LEFT; //10000.....

//This structure is an implementation of numerical set by bit-vectors
typedef struct Bit_set{
  uint *values; //This is a dynamic array of uint(as bit-vectors).
  unsigned long long amount; //Amount of current values of set
  unsigned long long max_amount; //Max amount of feasible values of set
  long long min; //This is a min value of universal set.
  long long max; //This is a max value of universal set.
} Bit_set;

//Returns a bitset, that has universal set as [min;max]
//Be careful. It creates a dynamic array
Bit_set create_set(long long min, long long max);

//Adds a value into a set
void add_set(Bit_set *set, long long value);

//Retuns 1, if value is in set. Else = 0
int is_in_set(Bit_set *set, long long value);

//Outputs a set into stdout.
void output_set(Bit_set *set);

//Performs a complement operation on a set
void complement_set(Bit_set *set);

//Deletes a value from a set
void delete_from_set(Bit_set *set, long long value);

//Deallocates the space previously allocated for values of set
void destroy_set_values(Bit_set *set);

#endif //__BITSET_DEFINED__
