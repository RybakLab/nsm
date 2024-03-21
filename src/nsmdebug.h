/*
  nsmdebug.h file contains declarations and macroses for debugging
  ------------------------------------------------------------------------------------------------------
  This file is part of NSM project supported by NIH/DHHS/PHS grant #R01NS100928.
  NSM is free software: you can redistribute it and/or modify it under the terms of the Lesser GNU
  General Public License as published by the Free Software Foundation, either version 2.1 of the License,
  or any later version.
  ------------------------------------------------------------------------------------------------------
  Copyright (C) 2018-2019 Sergey Markin
*/
#pragma once

#ifndef __NSM_DEBUG_H
#define __NSM_DEBUG_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// comment/uncomment the definitions below to exclude different modules
// of simulation engine from calculations
#define __IONS__ON__
#define __CHAN__ON__
#define __COMPART__ON__
#define __HNN__ON__
#define __POPULAT__ON__
#define __OUTPUT__ON__
#define __CACHE__ON__
#define __CONTROL__ON__
#define __CONNECT__ON__
#define __CONNECT_COLLAPSE_ON__
/////////////////////////////////////////////////////////////////////////////
// uncomment the definitions below to turn on debugging mode
#if !defined( __nsm_debug )
	#define __nsm_debug
#endif
/////////////////////////////////////////////////////////////////////////////
// specify debugging macroses
#if defined ( __nsm_debug )
	/////////////////////////////////////////////////////////////////////////////
	// check out the alignment of pointer ptr
	#define _pointer_db( ptr, align ) \
		{ \
			assert( ptr != nullptr ); \
			auto _ptr_ = reinterpret_cast<std::uintptr_t>( ptr ); \
			assert( _ptr_%( align ) == 0 ); \
		}
	/////////////////////////////////////////////////////////////////////////////
	// define _print_db macros
	#define _log_ get_logfile()
	/* base on Richard Hansen trick (see: https://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick) */
	// expands to the first argument
	#define FIRST_ARG_HELPER( first, ... ) first
	// if there's only one argument, expands to nothing;  if there is more
	// than one argument, expands to a comma followed by everything but
	// the first argument; only supports up to 9 arguments.
	#define SELECT_10TH( a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ... ) a10
	#define NUM(...) SELECT_10TH(__VA_ARGS__, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE,\
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, ONE, throwaway)
	#define REST_HELPER_TWOORMORE( first, ... ) , __VA_ARGS__
	#define REST_HELPER_ONE( first )
	#define REST_HELPER2( qty, ... ) REST_HELPER_##qty( __VA_ARGS__ )
	#define REST_HELPER( qty, ...) REST_HELPER2( qty, __VA_ARGS__)
	#define __FIRST_ARG(...) FIRST_ARG_HELPER( __VA_ARGS__, throwaway )
	#define __REST_ARGS(...) REST_HELPER( NUM( __VA_ARGS__ ), __VA_ARGS__)
	#define _print_db_helper( fmp, ... ) fprintf( _log_, "warning <" fmp __REST_ARGS(__VA_ARGS__));\
					fprintf( _log_, "> in file %s; line %d:\n", __FILE__, __LINE__ )
	#define _print_db( ... ) _print_db_helper( __FIRST_ARG( __VA_ARGS__ ), ...)
#else
	#define _pointer_db( ptr, align )
	#define _print_db( ... )
#endif /*__nsm_debug*/

extern FILE *get_logfile( void ); // get pointer to log file
extern bool open_logfile( void ); // open log file
extern bool close_logfile( void ); // close log file

#endif /*__NSM_DEBUG_H*/
