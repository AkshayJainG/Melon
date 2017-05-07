
/*
 * Copyright (C) Niklaus F.Schen.
 * Reed-Solomon Code
 */
#ifndef __MLN_RS_H
#define __MLN_RS_H

#include "mln_types.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    mln_size_t      row;
    mln_size_t      col;
    mln_u8ptr_t     data;
    mln_u32_t       isRef:1;
} mln_rs_matrix_t;

typedef struct {
    mln_u8ptr_t     data;
    mln_size_t      len;
    mln_size_t      num;
} mln_rs_result_t;

#define mln_rs_result_getNum(_presult) \
  ((_presult) == NULL? 0: (_presult)->num)

#define mln_rs_result_getLenth(_presult) \
  ((_presult)->len / (_presult)->num)

#define mln_rs_result_getIndexData(_presult,index) \
  ( (_presult) == NULL? NULL: \
      ((_presult)->num <= (index)? NULL: \
        (&((_presult)->data[(index)*((_presult)->len/(_presult)->num)]) )))

extern mln_rs_result_t *
mln_rs_encode(uint8_t **dataVector, size_t len, size_t n, size_t k);
extern void mln_rs_result_free(mln_rs_result_t *result);
extern mln_rs_result_t *
mln_rs_decode(uint8_t **dataVector, size_t len, size_t n, uint8_t **rsVector, size_t k);
extern void mln_rs_matrix_dump(mln_rs_matrix_t *matrix);
#endif

