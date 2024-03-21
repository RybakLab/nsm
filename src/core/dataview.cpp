/////////////////////////////////////////////////////////////////////////////
// dataview.cpp
#include "precompile.h"
#include "dataview.h"
#include "simulate.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

/////////////////////////////////////////////////////////////////////////////
// hhn_viewbuffer class
hhn_viewbuffer &hhn_viewbuffer::operator = ( const hhn_viewbuffer &buffer )
{
	free_buffer();
	hhn_process::operator = ( buffer );
	ReservedSize = buffer.ReservedSize;
	CurrData = buffer.CurrData;
	ParCode = buffer.ParCode;
	ViewValue = buffer.ViewValue;
	StatValue = buffer.StatValue;
	NumSteps = buffer.NumSteps;
	return *this;
}

void hhn_viewbuffer::set_buffer( void *currdata, const unit_code &code, size_t size )
{
	CurrData = currdata;
	ParCode = code;
	NumSteps = size;
}

void hhn_viewbuffer::alloc_buffer( void )
{
	if( ParCode.is_stat() ){
		for( size_t i = 0; i < StatValue.size(); StatValue[i].clear(), ++i );
		StatValue.clear();
		if( CurrData && ReservedSize > 0 ){
			StatValue.reserve((( lvector *)CurrData )->size() );
			std::generate_n( std::back_inserter( StatValue ), (( lvector *)CurrData )->size(), []{ return lvector(); });
			for( size_t i = 0; i < StatValue.size(); ++i ){
				StatValue[i].reserve( ReservedSize );
			}
		}
	}
	else{
		ViewValue.clear();
		if( NumSteps > 0 ){
			ViewValue.reserve( NumSteps );
			std::generate_n(std::back_inserter( ViewValue ), NumSteps, []{ return 0.f; });
		}
	}
}

void hhn_viewbuffer::free_buffer( void )
{
	ParCode = unit_code();
	CurrData = NULL;
	NumSteps = 0;
	nsm_vector(float) chart_tmp;
	nsm_vector(lvector) stat_tmp;
	ViewValue.clear();
	for( size_t i = 0; i < StatValue.size(); StatValue[i].clear(), ++i );
	StatValue.clear();
	ViewValue.swap( chart_tmp );
	StatValue.swap( stat_tmp );
}

void hhn_viewbuffer::save( ostream &file, const hhn_pair<int> &wnd, double step, size_t prec_t, size_t prec_a, CSimulate *manager )
{
	size_t x = size_t( wnd.X/step ); x = ( x >= ViewValue.size())? ViewValue.size()-1: x;
	size_t y = size_t( wnd.Y/step ); y = ( y >= ViewValue.size())? ViewValue.size()-1: y;
	if( ParCode.is_stat() ){
		string name = manager->Network.get_nnunit( ParCode )->get_name();
		file << "<Spikes " << name << ">" << endl;
		for( size_t i = 0; i < StatValue.size(); i++ ){
			file << "<Neuron " << i << ">" << endl;
			ios_base::fmtflags old_flags = file.flags( ios::fixed );
			for( size_t j = 0; j < StatValue[i].size(); j++ ){
				if( StatValue[i][j] >= x && StatValue[i][j] <= y ){
					file << setprecision( prec_t ) << float( StatValue[i][j]*step ) << endl;
				}
			}
			file.flags( old_flags );
			file << "</Neuron>" << endl;
		}
		file << "</Spikes>" << endl;
	}
	else{
		string name;
		ParCode.get_fullname( manager, name );
		file << "<Wave form " << name << ">" << endl;
		ios_base::fmtflags old_flags = file.flags( ios::fixed );
		// TODO the parameter prec_t can effect the total number of points stored in the file, 
		// currently every point stores, but the loop may be implemented as for( size_t j = x; j < y; j += prec_t ){ /*...*/ }
		for( size_t j = x; j < y; j++ ){
			file << setprecision( prec_a ) << float( ViewValue[j] ) << endl;
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
	assert( currstep < ViewValue.size() );
	ViewValue[currstep] = float((( double * )CurrData )[0] );
}

/////////////////////////////////////////////////////////////////////////////
// CChartBuffer class
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

void CChartBuffer::init_all_buffers( double step, size_t numsteps, double freq )
{
	if( numsteps > 0 ){
		CBufferManager::init_all_buffers( step, numsteps, freq );
		TimeScale.reserve( numsteps );
		size_t i = 0; std::generate_n( std::back_inserter( TimeScale) , numsteps, [&i,&step]{ return float( step*i++ ); });
		ReservedSize = 2*size_t( freq*step*( numsteps ));
	}
}

void CChartBuffer::release_all_buffers( void )
{
	ParBuffer.clear();
	TimeScale.clear();
	nsm_vector(hhn_viewbuffer) par_tmp;
	nsm_vector(float) scale_tmp;
	ParBuffer.swap( par_tmp );
	TimeScale.swap( scale_tmp );
	CBufferManager::release_all_buffers();
}

void CChartBuffer::alloc_all_buffers( void )
{
	CBufferManager::alloc_all_buffers();
	for( size_t i = 0; i < ParBuffer.size(); ++i ){
		ParBuffer[i].alloc_buffer();
	}
}

bool CChartBuffer::add_buffer( const unit_code &code )
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
#if defined (__MECHANICS_2D__)
			else
				data = ( void *)Manager->Walker->select(( unit_code *)&code );
#elif defined (__MECHANICS_3D__)
			// TODO select the variables and parameters of 3d model
#endif // __MECHANICS_2D__
		}
		if( !data )	return false;
		ParBuffer.push_back( hhn_viewbuffer());
		ParBuffer.back().set_buffer( data, code, max_nsteps() );
	}
	return true;
}

void CChartBuffer::save( ostream &file, const vector<unit_code> &buffers, hhn_pair<int> wnd, double prec_t, int prec_a , int format )
{
	if( ViewStep ){
		size_t prec_bin = ( size_t )( prec_t > ViewStep? ceil( prec_t/ViewStep ): 1 );
		size_t prec_num = ( size_t )( prec_a <= 0 )? 6: prec_a;
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
					file << "Precision = " << prec_t << " msec" << endl;
				else
					file << "Precision = " << ViewStep << " msec" << endl;
				for( size_t j = 0; j < ParBuffer.size(); j++ ){
					for( size_t i = 0; i < buffers.size(); i++ ){
						if( ParBuffer[j] == buffers[i] ){
							ParBuffer[j].save( file, wnd, ViewStep, prec_bin, prec_num, Manager );
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
					file << "Precision = " << prec_t << " msec" << endl;
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

#if defined (__MECHANICS_2D__)
/////////////////////////////////////////////////////////////////////////////
// CWalkerVertex class
CWalkerVertex::CWalkerVertex( void )
{
}

CWalkerVertex::CWalkerVertex( const CWalkerVertex &buffer )
{
	memcpy( Walker, buffer.Walker, sizeof( hhn_pair<float> )*10 );
}

CWalkerVertex &CWalkerVertex::operator = ( const CWalkerVertex &buffer )
{
	memcpy( Walker, buffer.Walker, sizeof( hhn_pair<float> )*10 );
	return *this;
}

CWalkerVertex &CWalkerVertex::operator = ( const vertex *ver )
{
	// Trunk
	Walker[0].X = (float)ver[TRUNK_H].X;
	Walker[0].Y = (float)ver[TRUNK_H].Y;
	// Loins
	Walker[1].X = (float)ver[LOINS].X;
	Walker[1].Y = (float)ver[LOINS].Y;
	// Pelvis
	Walker[2].X = (float)ver[HIP].X;
	Walker[2].Y = (float)ver[HIP].Y;
	// Left Thigh
	Walker[3].X = (float)ver[KNEE_L].X;
	Walker[3].Y = (float)ver[KNEE_L].Y;
	// Left Shank
	Walker[4].X = (float)ver[ANKLE_L].X;
	Walker[4].Y = (float)ver[ANKLE_L].Y;
	// Left Foot
	Walker[5].X = (float)ver[TOE__L].X;
	Walker[5].Y = (float)ver[TOE__L].Y;
	// Right Thigh
	Walker[6].X = (float)ver[HIP].X;
	Walker[6].Y = (float)ver[HIP].Y;
	// Right Knee
	Walker[7].X = (float)ver[KNEE_R].X;
	Walker[7].Y = (float)ver[KNEE_R].Y;
	// Right Shank
	Walker[8].X = (float)ver[ANKLE_R].X;
	Walker[8].Y = (float)ver[ANKLE_R].Y;
	// Right Foot
	Walker[9].X = (float)ver[TOE__R].X;
	Walker[9].Y = (float)ver[TOE__R].Y;
	return *this;
}

void CWalkerVertex::move_pos( float x, float y )
{
	for (size_t i = 0; i < 10; ++i) {
		Walker[i].X += x;
		Walker[i].Y += y;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWalkerBuffer class


CWalkerBuffer &CWalkerBuffer::operator = ( const CWalkerBuffer &buffer )
{
	CBufferManager::operator = ( buffer );
	CurrData = buffer.CurrData;
	Buffer = buffer.Buffer;
	return *this;
}

const void *CWalkerBuffer::get_buffer( void ) const
{
	return &Buffer;
}

void CWalkerBuffer::next_step( size_t currstep )
{
	if( currstep < Buffer.size()){
		CBufferManager::next_step( currstep );
		Buffer[currstep] = ( vertex *)CurrData;
	}
}

void CWalkerBuffer::init_all_buffers( double step, size_t numsteps, double freq )
{
	CBufferManager::init_all_buffers( step, numsteps, freq );
	Buffer.reserve( numsteps );
	CurrData = (void *)Manager->Walker->select();
	CWalkerVertex curr_value; curr_value = ( vertex *)CurrData;
	std::generate_n( std::back_inserter( Buffer ), numsteps, [&curr_value]{ return curr_value; });
}

void CWalkerBuffer::alloc_all_buffers( void )
{
	CBufferManager::alloc_all_buffers();
}

void CWalkerBuffer::release_all_buffers( void )
{
	CurrData = NULL;
	Buffer.clear();
	nsm_vector(CWalkerVertex) buff_tmp;
	Buffer.swap( buff_tmp );
	CBufferManager::release_all_buffers();
}
#elif defined (__MECHANICS_3D__)
#endif // __MECHANICS_2D__
