#include "PPintrin.h"
using namespace std;


// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
  
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  /*
  PP STUDENTS TODO: Implement your vectorized version of
  clampedExpSerial() here.
  
  Your solution should work for any value of
  N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  */

  __pp_vec_int exp;
  __pp_vec_float val    = _pp_vset_float(0.f);
  __pp_vec_float nine   = _pp_vset_float(9.999999f);
  __pp_vec_int one      = _pp_vset_int(1);
  __pp_vec_int zero     = _pp_vset_int(0);
  __pp_vec_float zero_f = _pp_vset_float(0.f);
  
  __pp_mask maskAll, maskExp, maskIsNegative;
  // All ones
  maskAll = _pp_init_ones();
  
  //if(N%VECTOR_WIDTH!=0){N= VECTOR_WIDTH*(N/VECTOR_WIDTH);}
  int i;
  for (i = 0; i < N; i += VECTOR_WIDTH){
    
    __pp_vec_float result = _pp_vset_float(1.f);
    // 1. exponents load 進 exp
    _pp_vload_int(exp, exponents + i, maskAll); 
    // 2. values load 進 val
    _pp_vload_float(val, values + i, maskAll); 
    
    _pp_vgt_int(maskExp, exp, zero, maskAll);   // if(exp > 0): mask = 1
    // 3. 計算
    while(_pp_cntbits(maskExp)){                // sum(mask) = 0 => stop (代表都算完了)
      _pp_vmult_float(result, result, val, maskExp);  // result = result * val
      _pp_vsub_int(exp, exp, one, maskAll);     // exp = exp - 1
      _pp_vgt_int(maskExp, exp, zero, maskAll); // exp > 0 => mask = 1
    }
    // 4. 最大就9.999999f
    _pp_vgt_float(maskExp, result, nine, maskAll); // result > 9.999999f => result = 9.999999f
    _pp_vset_float(result, 9.999999f, maskExp);

    // 5. Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);

  }
    
  // 6. 要補零的向量
  int r = N%VECTOR_WIDTH; //沒有整除
  if(r){
    // All zeros
    maskIsNegative = _pp_init_ones(0);
    // 要補零的位置mask=true
    for(int j=r;j<VECTOR_WIDTH;j++){
      maskIsNegative.value[j] = true;
    }
    _pp_vstore_float(output + i - VECTOR_WIDTH, zero_f, maskIsNegative);
  }
  

}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{
  /*
  PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  */
  
  __pp_vec_float val, total=_pp_vset_float(0.f);
  __pp_mask maskAll;
  for (int i = 0; i < N; i += VECTOR_WIDTH){
    
    // All ones
    maskAll = _pp_init_ones();
    // 1. values load 進 val
    _pp_vload_float(val, values + i, maskAll);
    // 2. 計算
    for(int j=VECTOR_WIDTH; j!=1; j/=2){        // 只做pow(VECTOR_WIDTH,2)次
      _pp_hadd_float(val, val);
      _pp_interleave_float(val, val);
    }
    // 3. total
    _pp_vadd_float(total,total,val,maskAll);
  }
  // 4. 拿出total第一個值
  return total.value[0];
}