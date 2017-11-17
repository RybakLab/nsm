/////////////////////////////////////////////////////////////////////////////
// dataview.cpp
#include "precompile.h"
#include "dataview.h"
#include "simulate.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

/////////////////////////////////////////////////////////////////////////////
// hhn_viewbuffer class
hhn_viewbuffer::hhn_viewbuffer( void )
	: hhn_process(), ParCode(), ViewValue(), StatValue(), 
	ReservedSize( 0 ), CurrData( NULL )
{
}

hhn_viewbuffer::hhn_viewbuffer( const hhn_viewbuffer &buffer )
	: hhn_process( buffer ), ParCode( buffer.ParCode ), ViewValue( buffer.ViewValue ), StatValue( buffer.StatValue ),
	ReservedSize( buffer.ReservedSize ), CurrData( buffer.CurrData )
{
}

hhn_viewbuffer::~hhn_viewbuffer( void )
{
	free();
}

hhn_viewbuffer &hhn_viewbuffer::operator = ( const hhn_viewbuffer &buffer )
{
	hhn_process::operator = ( buffer );
	ReservedSize = buffer.ReservedSize;
	CurrData = buffer.CurrData;
	ParCode = buffer.ParCode;
	ViewValue = buffer.ViewValue;
	StatValue = buffer.StatValue;
	return *this;
}

void hhn_viewbuffer::alloc( void *currdata, const unit_code &code, size_t numsteps, double step )
{
	free();
	CurrData = currdata;
	ParCode = code;
	if( ParCode.is_stat() ){
		vector<int> *data = ( vector<int> *)CurrData;
		for( size_t i = 0; i < data->size(); i++ )
			StatValue.push_back( lvector() );
	}
	else{
		ViewValue.resize( numsteps, 0 );
	}
}

void hhn_viewbuffer::reserve( size_t size )
{
	ReservedSize = size;
	for( size_t i = 0; i < StatValue.size(); i++ ){
		StatValue[i].reserve( ReservedSize );
	}
}

void hhn_viewbuffer::free( void )
{
	ParCode = unit_code();
	CurrData = NULL;
	ViewValue.clear();
	StatValue.clear();
}

void hhn_viewbuffer::save( ostream &file, const hhn_pair<int> &wnd, double step, size_t precstep, CSimulate *manager )
{
	size_t x = size_t( wnd.X/step ); 
	size_t y = size_t( wnd.Y/step );
	if( ParCode.is_stat() ){
		string name = manager->Network.get_nnunit( ParCode )->get_name();
		file << "<Spikes " << name << ">" << endl;
		for( size_t i = 0; i < StatValue.size(); i++ ){
			file << "<Neuron " << i << ">" << endl;
			ios_base::fmtflags old_flags = file.flags( ios::fixed );
			for( size_t j = 0; j < StatValue[i].size(); j++ ){
				if( StatValue[i][j] >= x && StatValue[i][j] <= y ){
					file << setprecision( 1 ) << float( StatValue[i][j]*step ) << endl;
				}
			}
			file.flags( old_flags );
			file << "</Neuron>" << endl;
		}
		file << "</Spikes>" << endl;
	}
	else{
		x = ( x >= ViewValue.size())? ViewValue.size()-1: x;
		y = ( y >= ViewValue.size())? ViewValue.size()-1: y;
		string name;
		ParCode.get_fullname( manager, name );
		file << "<Wave form " << name << ">" << endl;
		ios_base::fmtflags old_flags = file.flags( ios::fixed );
		for( size_t j = x; j < y; j++ ){
			file <<  float( ViewValue[j] ) << endl;
		}
		file.flags( old_flags );
		file << "</Wave form>" << endl;
	}
}

long hhn_viewbuffer::save_header( ostream &file, CSimulate *manager, long num, double *calibr )
{
	long num_col = 0;
	*calibr = 1.;
	if( ParCode.is_stat() ){
		string name = manager->Network.get_nnunit( ParCode )->get_name();
		for( size_t i = 0; i < StatValue.size(); ++i ){
			file << "column_" << num+num_col << " = "<< name << " neuron " << i << "; min = 0; max = 1" <<endl;
			++num_col;
		}
	}
	else{
		string name;
		ParCode.get_fullname( manager, name );
		vector<float>::iterator begin, end;
		float min_val = *std::min_element( ViewValue.begin(), ViewValue.end()-1 );
		float max_val = *std::max_element( ViewValue.begin(), ViewValue.end()-1 );
		double k = max( fabs( double( max_val )), fabs( double( min_val )));
		if( k == 0.0 ){
			*calibr = 0;
			file << "column_" << num << " = "<< name << "; min = " <<  min_val << "; max = " <<  max_val << "; calibr = " <<  *calibr << endl;
		}
		else{
			*calibr = 16384./k;
			file.setf( ios_base::fixed );
			file << "column_" << num << " = "<< name << "; min = " <<  min_val << "; max = " <<  max_val << "; calibr = " << setprecision( 10 ) << 1./( *calibr ) << setprecision( 6 ) << endl;
			file.unsetf( ios_base::fixed );
		}
		num_col = 1;
	}
	return num_col;
}

void hhn_viewbuffer::reg_unit( runman *man )
{
	if( ParCode.is_stat() ){
		::reg_unit( this, hhn_viewbuffer::storedata_stat, _id_PASS_VIEW, -1, man ); 
	}
	else{
		::reg_unit( this, hhn_viewbuffer::storedata_chart, _id_PASS_VIEW, -1, man ); 
	}
}

void hhn_viewbuffer::storedata_stat( size_t currstep, double step )
{
	vector<int> *data = ( vector<int> *)CurrData;
	for( size_t i = 0; i < StatValue.size() && i < data->size(); i++ ){
		if(( *data )[i] ){
			::lock_data();
			if( StatValue[i].size() >= StatValue[i].capacity()-ReservedSize ){
					StatValue[i].reserve( StatValue[i].capacity()+ReservedSize );
			}
			::unlock_data();
			StatValue[i].push_back( currstep );
		}
	}
}

void hhn_viewbuffer::storedata_chart( size_t currstep, double step )
{
	if( currstep < ViewValue.size() ){
		double curr_value = *(( double *)CurrData );
		ViewValue[currstep] = float( curr_value );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChartBuffer class
CChartBuffer::CChartBuffer( CSimulate *manager )
	: CBufferManager( manager )
{
	ReservedSize = 0;
}

CChartBuffer::CChartBuffer( const CChartBuffer &buffer )
	: CBufferManager( buffer ), ParBuffer( buffer.ParBuffer ), TimeScale( buffer.TimeScale )
{
	ReservedSize = buffer.ReservedSize;
}

CChartBuffer::~CChartBuffer( void )
{
	TimeScale.clear();
	ParBuffer.clear();
}

CChartBuffer &CChartBuffer::operator = ( const CChartBuffer &buffer )
{
	CBufferManager::operator = ( buffer );
	ReservedSize = buffer.ReservedSize;
	ParBuffer = buffer.ParBuffer;
	TimeScale = buffer.TimeScale;
	return *this;
}

const void *CChartBuffer::get_buffer( const unit_code &code ) const
{
	for( size_t i = 0; i < ParBuffer.size(); i++ )
		if( ParBuffer[i] == code )
			return ParBuffer[i].get_buffer();
	return NULL;
}

void CChartBuffer::reg_unit( runman *man )
{
	for( size_t i = 0; i < ParBuffer.size(); ++i )
		ParBuffer[i].reg_unit( man );
}

void CChartBuffer::next_step( size_t currstep )
{
	CBufferManager::next_step( currstep );
}

void CChartBuffer::init_views( double step, size_t numsteps, double freq )
{
	CBufferManager::init_views( step, numsteps, freq );
	TimeScale.reserve( numsteps+1 );
	for( size_t i = 0; i < numsteps+1; i++ )
		TimeScale.push_back( float( ViewStep*i ));
	ReservedSize = 2*size_t( freq*step*( numsteps+1 ));
}

void CChartBuffer::release_views( void )
{
	CBufferManager::release_views();
	ParBuffer.clear();
	vector<hhn_viewbuffer> par_tmp;
	ParBuffer.swap( par_tmp );

	TimeScale.clear();
	vector<float> scale_tmp;
	TimeScale.swap( scale_tmp );
}

bool CChartBuffer::add_view( const unit_code &code )
{
	if( get_buffer( code ) == NULL ){
		void *data = NULL;
		nn_unit *object = Manager->Network.get_nnunit( code );
		if( object )
			data = ( void *)( object->select(( unit_code *)&code ));
		else{
			hhn_control *control = Manager->Network.get_crtunit( code );
			if( control )
				data = ( void *)( control->select(( unit_code *)&code ));
#ifdef __MECHANICS__
			else
				data = ( void *)Manager->Walker->select(( unit_code *)&code );
#endif // __MECHANICS__
		}
		if( !data )	return false;
		ParBuffer.push_back( hhn_viewbuffer());
		ParBuffer[ParBuffer.size()-1].alloc( data, code, max_nsteps(), ViewStep );
	}
	return true;
}

void CChartBuffer::save( ostream &file, const vector<unit_code> &buffers, hhn_pair<int> wnd, double prec, int format )
{
	if( ViewStep ){
		size_t prec_bin = ( size_t )( prec > ViewStep? prec/ViewStep : 1 );
		switch( format ){
			case _id_ASCII_GEN: // ASCII (generic)
			{
				file << "<Header>" << endl;
				file << "Generated by " << _ProjectName << endl;
				file << "</Header>" << endl;
				file << "<Record>" << endl;
				if( wnd.X >= wnd.Y || wnd.X <= 0 ){
					wnd.X = 0; wnd.Y = int( ( TimeScale.size()-1 )*ViewStep );
				}
				file << "Saved window = [" << wnd.X << "," << wnd.Y << "] msec" << endl;
				file << "Duration = " << wnd.Y-wnd.X << " msec" << endl;
				if( prec_bin > 1 )
					file << "Precision = " << prec << " msec" << endl;
				else
					file << "Precision = " << ViewStep << " msec" << endl;
				for( size_t j = 0; j < ParBuffer.size(); j++ ){
					for( size_t i = 0; i < buffers.size(); i++ ){
						if( ParBuffer[j] == buffers[i] ){
							ParBuffer[j].save( file, wnd, ViewStep, prec_bin, Manager );
							break;
						}
					}
				}
				file << "</Record>" << endl;
			}
			break;
			case _id_BIN_SCRC: // SCRC (columns)
			{
				file << "<Header>" << endl;
				file << "Generated by " << _ProjectName << endl;
				file << "Duration = " << ( TimeScale.size()-1 )*ViewStep << " msec" << endl;
				if( prec_bin > 1 )
					file << "Precision = " << prec << " msec" << endl;
				else
					file << "Precision = " << ViewStep << " msec" << endl;
				vector<double> calibr;
				for( size_t j = 0, num_col = 0; j < ParBuffer.size(); j++ ){
					calibr.push_back( 0 );
					for( size_t i = 0; i < buffers.size(); i++ ){
						if( ParBuffer[j] == buffers[i] ){
							double k = 1;
							num_col += ParBuffer[j].save_header( file, Manager, num_col, &k );
							calibr[j] = k;
							break;
						}
					}
				}
				file << "</Header>";
				file.put( 0x0A ); // endl
				for( size_t time = 0; time < TimeScale.size()-1; ++time ){
					for( size_t j = 0; j < ParBuffer.size(); j++ ){
						for( size_t i = 0; i < buffers.size(); i++ ){
							if( ParBuffer[j] == buffers[i] ){
								short data = short( ParBuffer[j][time]*calibr[j] );
								file.put( char( data & 0x00FF ));
								file.put((( data >> 8 ) & 0x00FF ));
								break;
							}
						}
					}
				}
			}
			break;
		}
	}
}

#ifdef __MECHANICS__
/////////////////////////////////////////////////////////////////////////////
// CWalkerVertex class
CWalkerVertex::CWalkerVertex( void )
	: Left( 6, hhn_pair<float>() ), Right( 4, hhn_pair<float>() )
{
}

CWalkerVertex::CWalkerVertex( const CWalkerVertex &buffer )
	: Left( buffer.Left ), Right( buffer.Right )
{
}

CWalkerVertex &CWalkerVertex::operator = ( const CWalkerVertex &buffer )
{
	Left = buffer.Left;
	Right = buffer.Right;
	return *this;
}

CWalkerVertex &CWalkerVertex::operator = ( const vertex *ver )
{
 // Trunk
	Left[0].X = ( float )ver[TRUNK_H].X;
	Left[0].Y = ( float )ver[TRUNK_H].Y;
 // Loins
	Left[1].X = ( float )ver[LOINS].X;
	Left[1].Y = ( float )ver[LOINS].Y;
 // Pelvis
	Left[2].X = ( float )ver[HIP].X;
	Left[2].Y = ( float )ver[HIP].Y;
 // Left Thigh
	Left[3].X = ( float )ver[KNEE_L].X;
	Left[3].Y = ( float )ver[KNEE_L].Y;
 // Left Shank
	Left[4].X = ( float )ver[ANKLE_L].X;
	Left[4].Y = ( float )ver[ANKLE_L].Y;
 // Left Foot
	Left[5].X = ( float )ver[TOE__L].X;
	Left[5].Y = ( float )ver[TOE__L].Y;
 // Right Thigh
	Right[0].X = ( float )ver[HIP].X;
	Right[0].Y = ( float )ver[HIP].Y;
 // Right Knee
	Right[1].X = ( float )ver[KNEE_R].X;
	Right[1].Y = ( float )ver[KNEE_R].Y;
 // Right Shank
	Right[2].X = ( float )ver[ANKLE_R].X;
	Right[2].Y = ( float )ver[ANKLE_R].Y;
 // Right Foot
	Right[3].X = ( float )ver[TOE__R].X;
	Right[3].Y = ( float )ver[TOE__R].Y;
	return *this;
}

void CWalkerVertex::move_pos( float x, float y )
{
	for( size_t i = 0; i < Left.size(); ++i ){
		Left[i].X += x;
		Left[i].Y += y;
	}
	for( size_t i = 0; i < Right.size(); ++i ){
		Right[i].X += x;
		Right[i].Y += y;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWalkerBuffer class
CWalkerBuffer::CWalkerBuffer( CSimulate *manager )
	: CBufferManager( manager )
{
	CurrData = NULL;
}

CWalkerBuffer::CWalkerBuffer( const CWalkerBuffer &buffer )
	: CBufferManager( buffer ), Buffer( buffer.Buffer )
{
	CurrData = buffer.CurrData;
}

CWalkerBuffer::~CWalkerBuffer( void )
{
}

CWalkerBuffer &CWalkerBuffer::operator = ( const CWalkerBuffer &buffer )
{
	CBufferManager::operator = ( buffer );
	CurrData = buffer.CurrData;
	Buffer = buffer.Buffer;
	return *this;
}

size_t CWalkerBuffer::max_nsteps( void ) const
{
	return ( size_t )Buffer.size();
}

const void *CWalkerBuffer::get_buffer( void ) const
{
	return &Buffer;
}

void CWalkerBuffer::next_step( size_t currstep )
{
	static CWalkerVertex curr_value;
	if( currstep < Buffer.size()){
		CBufferManager::next_step( currstep );
		curr_value = ( vertex *)CurrData;
//		curr_value.move_pos( -400., 0. );
		Buffer[currstep] = curr_value;
	}
}

void CWalkerBuffer::init_views( double step, size_t numsteps, double freq )
{
	CBufferManager::init_views( step, numsteps, freq );
	CurrData = ( void *)Manager->Walker->select();
	Buffer.resize( numsteps );
}

void CWalkerBuffer::release_views( void )
{
	CBufferManager::release_views();
	CurrData = NULL;
	Buffer.clear();
	vector<CWalkerVertex> buff_tmp;
	Buffer.swap( buff_tmp );
}
#endif // __MECHANICS__
