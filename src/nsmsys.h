#pragma once

#ifndef NSM_SYS_H
#define NSM_SYS_H

#include <emmintrin.h> 
#include <xmmintrin.h>
#include <emmintrin.h>

#if defined( __GNUC__ ) /* GCC compiler is detected */
	#if defined( __MINGW32__ ) /*MinGW compiler is detected*/
		#if defined( __BOOST__ ) /* use boost */
			#include "lib/align/aligned_allocator.hpp"
			#define nsm_alloc( align, size ) boost::alignment::aligned_alloc(size_t(align),size_t(size))
			#define nsm_free( ptr )	boost::alignment::aligned_free( ptr )
		#else
			#include <malloc.h>
			#define nsm_alloc( align, size ) _aligned_malloc(( size ), ( align ))
			#define nsm_free( ptr ) _aligned_free( ptr )
		#endif /*__BOOST__*/
		typedef __int32 nsm32;
		typedef unsigned __int32 nsm32u;
		typedef unsigned __int64 nsm64u;
	#else
		#if defined( __BOOST__ ) /* use boost */
			#include "lib/boost/align/aligned_allocator.hpp"
			#define nsm_alloc( align, size ) boost::alignment::aligned_alloc(size_t(align),size_t(size))
			#define nsm_free( ptr )	boost::alignment::aligned_free( ptr )
		#else
			#include <malloc.h>
			#define nsm_alloc( align, size ) aligned_alloc(( align ), ( size ))
			#define nsm_free( ptr ) free( ptr )
		#endif /*__BOOST__*/
		#include <stdint.h>
		#include <stdlib.h>
		typedef int32_t nsm32;
		typedef uint32_t nsm32u;
		typedef uint64_t nsm64u;
	#endif
#elif defined( _MSC_VER ) /* MSVC compiler is detected */
	#if defined (__INTEL_COMPILER) /* Intel compiler as part of MS Visual studio is detected */
		#if defined( __BOOST__ ) /* use boost */
			#include "lib/boost/align/aligned_allocator.hpp"
			#define nsm_alloc( align, size ) boost::alignment::aligned_alloc(size_t(align),size_t(size))
			#define nsm_free( ptr )	boost::alignment::aligned_free( ptr )
		#else
			#include <malloc.h>
			#define nsm_alloc( align, size ) _mm_malloc(( size ), ( align )) 
			#define nsm_free( ptr ) _mm_free( ptr ) 
		#endif /*__BOOST__*/
	#elif defined (__INTEL_LLVM_COMPILER )
		#if defined( __BOOST__ ) /* use boost */
			#include "lib/boost/align/aligned_allocator.hpp"
			#define nsm_alloc( align, size ) boost::alignment::aligned_alloc(size_t(align),size_t(size))
			#define nsm_free( ptr )	boost::alignment::aligned_free( ptr )
		#else
			#include <malloc.h>
			#define nsm_alloc( align, size ) _mm_malloc(( size ), ( align )) 
			#define nsm_free( ptr ) _mm_free( ptr ) 
		#endif /*__BOOST__*/
	#else
		#if defined( __BOOST__ ) /* use boost */
			#include "lib/boost/align/aligned_allocator.hpp"
			#define nsm_alloc( align, size ) boost::alignment::aligned_alloc(size_t(align),size_t(size))
			#define nsm_free( ptr )	boost::alignment::aligned_free( ptr )
		#else
			#include <malloc.h>
			#define nsm_alloc( align, size ) _aligned_malloc(( size ), ( align )) 
			#define nsm_free( ptr ) _aligned_free( ptr ) 
		#endif /*__BOOST__*/
	#endif
	typedef __int32 nsm32;
	typedef unsigned __int32 nsm32u; 
	typedef unsigned __int64 nsm64u; 
#else
	typedef int nsm32;
	typedef unsigned int nsm32u; 
	typedef unsigned long nsm64u; 
	#include <malloc.h>
	#define nsm_alloc( align, size ) malloc( size ) 
	#define nsm_free( ptr ) free( ptr ) 
	#error "nsm32 type cannot be defined for your host compiler!"
	#error "nsm32u type cannot be defined for your host compiler!"
	#error "nsm64u type cannot be defined for your host compiler!"
#endif

#if defined( __BOOST__ ) /* use boost */
	#include "lib/boost/align/aligned_allocator.hpp"
	#define nsm_vector( type ) std::vector<type,boost::alignment::aligned_allocator<type,16>>
#else
	#define nsm_vector( type ) std::vector<##type>
#endif /*__BOOST__*/

#endif /*NSM_SYS_H*/
