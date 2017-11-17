/////////////////////////////////////////////////////////////////////////////
// mpi_calculate.cpp
#include "precompile.h"

#include "mpi_layer.h"
#include "runman.h"

#ifdef __MPI__

void mpi_in::calculate( int pass, double step, hhn_process *start )
{
	if( pass != _id_PASS_MPIIN )
		return;
	for( mpi_in *in = ( mpi_in *)start; in != NULL; in = ( mpi_in *)in->_next ){
		for( unsigned int i = 0; i < _id_MAX_SYN; i++ ){
			if( in->Proc[i] ){
				if( in->IsY[i] ){
					double t = in->Syn[i].Ts;
					if( t > 0.0 )
						in->Y[i] *= exp(-step/t );
					in->Output[i] = in->Y[i];
				}
				if( in->IsH[i] ){
					double alpha = 1./( 1+in->DSyn[i].Kdep*in->Y[i] );
					double t = in->DSyn[i].Tdep*alpha;
					in->H[i] = EXP_EULER( in->H[i], alpha, step, t );
					in->OutputD[i] = in->Y[i]*in->H[i];
				}
			}
		}
	}
}

void mpi_out::calculate( int pass, double step, hhn_process *start )
{
	if( pass != _id_PASS_MPIOUT )
		return;
	for( mpi_out *out = ( mpi_out *)start; out != NULL; out = ( mpi_out *)out->_next ){
		if( out->Proc ){
			out->Output += *out->Input;
		}
	}
}

#endif // __MPI__
