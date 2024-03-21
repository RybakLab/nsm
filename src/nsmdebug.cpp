/*
  nsmdebug.cpp
  ------------------------------------------------------------------------------------------------------
  This file is part of NSM project supported by NIH/DHHS/PHS grant #R01NS100928.
  NSM is free software: you can redistribute it and/or modify it under the terms of the Lesser GNU
  General Public License as published by the Free Software Foundation, either version 2.1 of the License,
  or any later version.
  ------------------------------------------------------------------------------------------------------
  Copyright (C) 2018-2019 Sergey Markin
*/
#include "precompile.h"
#include "nsmdebug.h"

#include <stdlib.h>
#include <time.h>

static FILE *nsm_log = NULL;

FILE *get_logfile( void )
{
#if defined( __nsm_debug )
	return ( NULL == nsm_log )? stderr: nsm_log;
#else
	fprintf( stderr, "warning: the debugging regime is off; use flag __nsm_debug to build the project" );
	return NULL;
#endif /*__nsm_debug*/
}

bool open_logfile( void )
{
#if defined( __nsm_debug )
	const char *filename = "nsmdb.log";
	if( NULL == nsm_log ){
		errno_t err;
		if(( err = fopen_s( &nsm_log, filename, "a+")) != 0 ){
			struct timespec ts;
			fprintf( _log_, "=============== log file %s is opened ===============\n", filename );
			if( timespec_get( &ts, TIME_UTC )){
				struct tm curr_time;
				char buff[100];
				//TODO: portability issue: gmtime_s has non-standard prototypes
				if(( err = gmtime_s( &curr_time, &ts.tv_sec )) != 0 ){ 
					strftime( buff, sizeof buff, "%D %T", &curr_time );
					fprintf( _log_, "current time: %s UTC\n", buff );
				}
			}
			return true;
		}else{
			nsm_log = NULL;
			fprintf( stderr, "open_log: cannot open log file %s\n", filename );
			return false;
		}
	}
	fprintf( stderr, "open_log: the log file is already opened" );
	return true;
#else
	fprintf( stderr, "warning: the debugging regime is off; use flag __nsm_debug to build the project" );
	return false;
#endif /*__nsm_debug*/
}

bool close_logfile( void )
{
#if defined( __nsm_debug )
	if( NULL != nsm_log ){
		fclose( nsm_log );
		nsm_log = NULL;
		return true;
	}
	return false;
#else
	fprintf( stderr, "warning: the debugging regime is off; use flag __nsm_debug to build the project" );
	return false;
#endif /*__nsm_debug*/
}
