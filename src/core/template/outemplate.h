/******************************************************************************
 * File: outemplate.h                                                         *
 *----------------------------------------------------------------------------*
 * Sergey Markin, 2005 University of Manitoba                                 *
 ******************************************************************************/
#ifndef _OUTPUT_TEMPLATE_H_
#define _OUTPUT_TEMPLATE_H_

#include "unitemplate.h"

class t_output : public uni_template{
	public: //--- constructor
		t_output( uni_template *parent, int is_active = GRID_ADD_ROW );
		t_output( const t_output &output );
virtual	~t_output( void ){};
	public:
		t_output &operator = ( const t_output &output );
	protected:
		t_data T1;
		t_data T2;
		t_data Threshold;
};

#endif // _OUTPUT_TEMPLATE_H_
