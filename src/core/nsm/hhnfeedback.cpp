//////////////////////////////////////////////////////////////////////
// hhnfeedback.cpp
#include "precompile.h"

#ifdef __MECHANICS__

#include "hhnfeedback.h"
#include "runman.h"
#include "walker.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

//////////////////////////////////////////////////////////////////////
// hhn_feedback
double hhn_feedback::_FbNull = 0.0;

//////////////////////////////////////////////////////////////////////
// constructors/destructor
hhn_feedback::hhn_feedback( void )
	: MechName( "None" ), Kg( 0.0 ), Pv( 0.0 ), Kv( 0.0 ),
	Kl( 0.0 ), Kemg( 0.0 ), Kc( 0.0 ), Kf( 0.0 ), Ftr( 0.0 )
{
	UnitId = _id_NNFeedback;
	ConnectType.del_connect();    // only source
	for( size_t i = 0; i < _id_NUM_FBS; FbBuff[i] = &_FbNull, ++i );
}

hhn_feedback::hhn_feedback( const string &name )
	: MechName( "None" ), Kg( 0.0 ), Pv( 0.0 ), Kv( 0.0 ),
	Kl( 0.0 ), Kemg( 0.0 ), Kc( 0.0 ), Kf( 0.0 ), Ftr( 0.0 )
{
	Name = name;
	UnitId = _id_NNFeedback;
	ConnectType.del_connect();    // only source
	for( size_t i = 0; i < _id_NUM_FBS; FbBuff[i] = &_FbNull, ++i );
}

hhn_feedback::hhn_feedback( const hhn_feedback &feedback )
	: nn_unit( feedback ), hhn_process( feedback ),
	MechName( feedback.MechName ), Kg( feedback.Kg ), Pv( feedback.Pv ), Kv( feedback.Kv ),
	Kl( feedback.Kl ), Kemg( feedback.Kemg ), Kc( feedback.Kc ), Kf( feedback.Kf ), Ftr( feedback.Ftr )
{
	for( size_t i = 0; i < _id_NUM_FBS; FbBuff[i] = &_FbNull, ++i );
}

//////////////////////////////////////////////////////////////////////
// operators
hhn_feedback &hhn_feedback::operator = ( const hhn_feedback &feedback )
{
	hhn_process::operator =( feedback );
	nn_unit::operator = ( feedback );
	MechName = feedback.MechName;
	Kg = feedback.Kg;
	Pv = feedback.Pv;
	Kv = feedback.Kv;
	Kl = feedback.Kl;
	Kemg = feedback.Kemg;
	Kc = feedback.Kc;
	Kf = feedback.Kf;
	Ftr = feedback.Ftr;
	for( size_t i = 0; i < _id_NUM_FBS; FbBuff[i] = &_FbNull, ++i );
	return *this;
}

//////////////////////////////////////////////////////////////////////
// methods
void hhn_feedback::init( void )
{
	Units.clear();
	Units.push_back( this );
	Output = 1.0;
	CtrlOut = 1.0;
	for( size_t i = 0; i < _id_NUM_FBS; FbBuff[i] = &_FbNull, ++i );
}

void hhn_feedback::attach( zmuscle *muscle, touch *grf )
{
	if( Kf > 0.0 && muscle ){		//Ib
		FbBuff[_id_FbF] = &( muscle->F );
		FbBuff[_id_FbFmax] = &( muscle->P0 );
		double f = *( FbBuff[_id_FbF] )/(*( FbBuff[_id_FbFmax] ))-Ftr;
		Output = Kf*f;
		if( Output < 0.0 ) Output = 0.0;
	}
	else if( Kv > 0.0 && muscle ){	//Ia
		FbBuff[_id_FbEMG] = ( muscle->MN );
		FbBuff[_id_FbV] = &( muscle->V );
		FbBuff[_id_FbL] = &( muscle->L );
		Ltr = muscle->get_lrest();
		double v = *( FbBuff[_id_FbV] ); v = sign( v )*pow( abs( v )/Ltr, Pv );
		double d = ( *( FbBuff[_id_FbL] )-Ltr )/Ltr;
		double emg = *( FbBuff[_id_FbEMG] );
		Output = Kv*v+Kl*d+Kemg*emg+Kc;
		if( Output < 0.0 ) Output = 0.0;
	}
	else if( Kl > 0.0 && muscle ){	//II
		FbBuff[_id_FbEMG] = ( muscle->MN );
		FbBuff[_id_FbL] = &( muscle->L );
		Ltr = muscle->get_lrest();
		double d = (*( FbBuff[_id_FbL] )-Ltr )/Ltr;
		double emg = *( FbBuff[_id_FbEMG] );
		Output = Kl*d+Kemg*emg+Kc;
		if( Output < 0.0 ) Output = 0.0;
	}
	else if( Kg > 0.0 && grf){		// GRF
		FbBuff[_id_FbTGX] = &( grf->Fx );
		FbBuff[_id_FbTGY] = &( grf->Fy );
		double tgx = *( FbBuff[_id_FbTGX] );
		double tgy = *( FbBuff[_id_FbTGY] );
		Output = Kg*sqrt( tgx*tgx+tgy*tgy );
	}
}

void hhn_feedback::reg_unit( runman *man )
{
	::reg_unit( this, hhn_feedback::feedback, _id_PASS_NNUNIT, -1, man ); 
}

void hhn_feedback::feedback( double step )
{
	if( Kf > 0.0 ){		//Ib
		double f = *( FbBuff[_id_FbF] )/(*( FbBuff[_id_FbFmax] ))-Ftr; // ask sasha if the muscle force is always positive?
		Output = Kf*f;
		if( Output < 0.0 ) Output = 0.0;
	}
	else if( Kv > 0.0 ){	//Ia
		double v = *( FbBuff[_id_FbV] ); v = sign( v )*pow( abs( v )/Ltr, Pv );
		double d = ( *( FbBuff[_id_FbL] )-Ltr )/Ltr;
		double emg = *( FbBuff[_id_FbEMG] );
		Output = Kv*v+Kl*d+Kemg*emg+Kc;
		if( Output < 0.0 ) Output = 0.0;
	}
	else if( Kl > 0.0 ){	//II
		double d = (*( FbBuff[_id_FbL] )-Ltr )/Ltr;
		double emg = *( FbBuff[_id_FbEMG] );
		Output = Kl*d+Kemg*emg+Kc;
		if( Output < 0.0 ) Output = 0.0;
	}
	else if( Kg > 0.0 ){	// GRF
		double tgx = *( FbBuff[_id_FbTGX] );
		double tgy = *( FbBuff[_id_FbTGY] );
		Output = Kg*sqrt( tgx*tgx+tgy*tgy );
	}
	else{
		Output = 1.0;
	}
	Output *= CtrlOut;
}

bool hhn_feedback::load( istream &file )
{
	string str;
	nn_unit::load( file );
	while( file >> str){
		if( str == "Size"){
			int size = 0;
			file >> str >> size;
		}
		else if( str == "InName"){
			file >> str >> ws;
			getline( file, MechName );
		}
		else if( str == "Kg"){
			file >> str >> Kg;
		}
		else if( str == "Pv"){
			file >> str >> Pv;
		}
		else if( str == "Kv"){
			file >> str >> Kv;
		}
		else if( str == "Kl"){
			file >> str >> Kl;
		}
		else if( str == "Kemg"){
			file >> str >> Kemg;
		}
		else if( str == "Kc"){
			file >> str >> Kc;
		}
		else if( str == "Kf"){
			file >> str >> Kf;
		}
		else if( str == "Ftr"){
			file >> str >> Ftr;
		}
		else if( str == "</Feedback>"){
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Feedback>!", "Warning!" );
	return false;
}

void hhn_feedback::save( ostream &file )
{
	file << endl << "<Feedback ";
	nn_unit::save( file );
	file << "InName = " << MechName << endl;
	file << "Kg = " << Kg << endl;
	file << "Pv = " << Pv << endl;
	file << "Kv = " << Kv << endl;
	file << "Kl = " << Kl << endl;
	file << "Kemg = " << Kemg << endl;
	file << "Kc = " << Kc << endl;
	file << "Kf = " << Kf << endl;
	file << "Ftr = " << Ftr << endl;
	file << "</Feedback>" << endl;
}

#endif // __MECHANICS__
