/******************************************************************************
 * File: outemplate.cpp                                                       *
 *----------------------------------------------------------------------------*
 * Sergey Markin, 2005 University of Manitoba                                 *
 ******************************************************************************/
#include "precompile.h"

#include "outemplate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG

t_output::t_output( uni_template *parent, int is_active )
        : uni_template( parent, is_active )
{
	TypeNameS = TypeName = "NSM Object";
	TypeSynonym.push_back( "Object" );
	TypeSynonym.push_back( "Output" );
	DList.insert( make_pair( "Time constant up (msec)\nTup", &T1 ));
	DList.insert( make_pair( "Time constant down(msec)\nTdown", &T2 ));
	DList.insert( make_pair( "Threshold\nThreshold", &Threshold ));
	T1 = 500;
	T2 = 10;
	Threshold = 0.;
}

t_output::t_output( const t_output &output )
        : uni_template( output )
{
	DList.insert( make_pair( "Time constant up (msec)\nTup", &T1 ));
	DList.insert( make_pair( "Time constant down(msec)\nTdown", &T2 ));
	DList.insert( make_pair( "Threshold\nThreshold", &Threshold ));
	T1 = output.T1;
	T2 = output.T2;
	Threshold = output.Threshold;
}

t_output &t_output::operator = ( const t_output &output )
{
	uni_template::operator =( output );
	T1 = output.T1;
	T2 = output.T2;
	Threshold = output.Threshold;
	return *this;
}
