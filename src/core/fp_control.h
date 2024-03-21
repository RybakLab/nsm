// http://software.intel.com/en-us/articles/x87-and-sse-floating-point-assists-in-ia-32-flush-to-zero-ftz-and-denormals-are-zero-daz

#ifndef __FLOATING_POINT_CONTROL_H
#define __FLOATING_POINT_CONTROL_H

#include <xmmintrin.h>
#include <memory.h>

#define X87FLAGBITS			6
#define DAZ_BIT				6
#define FTZ_BIT				15
#define DENORMAL_EXCEPTION_MASK		8
#define UNDERFLOW_EXCEPTION_MASK	11
#define OVERFLOW_EXCEPTION_MASK		10


inline void set_mxcsr_on( int bit_num )
{
	__m128 state[32];
	__int32 x;
	__asm fxsave state     
	memcpy(( void *)&x, ( char *)state+24, 4 );
	x |= ( 1 << bit_num );      
	__asm ldmxcsr   x  
}

inline void set_mxcsr_off( int bit_num )
{
	__m128 state[32];
	__int32 x;
	__asm fxsave state     
	memcpy(( void *)&x, ( char *)state+24, 4 );
	x &= ~( 1 << bit_num );
	__asm ldmxcsr x  
}

inline void clear_flags( void )
{
	__m128 state[32];
	__int32 x;
	__asm fxsave state     
	memcpy(( void *)&x, ( char *)state+24, 4 );
	x = x >> X87FLAGBITS;
	x = x << X87FLAGBITS;
	_asm ldmxcsr   x  
}

inline void make_denormal()
{
	__m128 denormal;
	int den_vec[4] = { 1, 1, 1, 1 };
	memcpy( &denormal, den_vec, sizeof( int )*4 );
	denormal = _mm_add_ps( denormal, denormal );
}

#endif
