/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Huxley type neuron               **
 **         Developed by Ilia Rybak and Sergey Markin           **
 ****                    Drive (header)                       ****
 *****************************************************************/
#include "precompile.h"

#include "hhndrive.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

hhn_drive::hhn_drive( void )
{ 
	UnitId = _id_NNDrive;
	ConnectType.del_connect();    // only source
}

hhn_drive::hhn_drive( const string &name )
{
	Name = name;
	UnitId = _id_NNDrive;
	ConnectType.del_connect();    // only source
}

hhn_drive::hhn_drive( const hhn_drive &drive )
	: nn_unit( drive ), hhn_process( drive )
{
}

hhn_drive &hhn_drive::operator = ( const hhn_drive &drive )
{
	hhn_process::operator =( drive );
	nn_unit::operator = ( drive );
	return *this;
}

bool hhn_drive::init( void )
{
	Units.clear();
	Output = 1.0;
	Units.push_back( this );
	return true;
}

bool hhn_drive::load( istream &file )
{
	nn_unit::load( file );
	string str;
	while( file >> str){
		if( str == "Size"){
			int size = 0;
			file >> str >> size;
		}
		else if( str == "</Drive>"){
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Drive>!", "Warning!" );
	return false;
}

void hhn_drive::save( ostream &file )
{
	file << endl << "<Drive ";
	nn_unit::save( file );
	file << "</Drive>" << endl;
}

