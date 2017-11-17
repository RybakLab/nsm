/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **         Developed by Ilia Rybak and Sergey Markin           **
 ****                    Drive (header)                       ****
 *****************************************************************/
#include "hhnunit.h"

#ifndef _HHN_DRIVE_H_
#define _HHN_DRIVE_H_

class hhn_drive : public nn_unit, public hhn_process{
	public:
		hhn_drive( void );
		hhn_drive( const string &name );
		hhn_drive( const hhn_drive &drive );
		~hhn_drive( void ){};
	public:
		hhn_drive &operator = ( const hhn_drive &drive );
	public:
		void init( void );
		bool load( istream &file );
		void save( ostream &file );
	public: //--- overrided function
		void *select( CHhnControlled *ctrl ){ return &Output; };
		void *select( unit_code *view ){ return &Output; };
};
#endif

