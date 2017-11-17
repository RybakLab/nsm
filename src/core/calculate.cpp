#include "precompile.h"

#include "hhnnetwork.h"
#include "walker.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

#ifdef __MECHANICS__
///////////////////////////////////////////////////////////////////////////////
// walker
void walker::calc_biomech( size_t currstep, double step, hhn_process **start )
{
	(( walker *)*start )->calc_biomech( step );
}
#endif /*__MECHANICS__*/

///////////////////////////////////////////////////////////////////////////////
// control
void hhn_control::control( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hhn_control *ctrl = ( hhn_control *)*start;
		ctrl->control( currstep, step );
	}
}

/////////////////////////////////////////////////////////////////////////////
// viewbuffer
void hhn_viewbuffer::storedata_stat( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hhn_viewbuffer *view = ( hhn_viewbuffer *)*start;
		view->storedata_stat( currstep, step );
	}
}

void hhn_viewbuffer::storedata_chart( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hhn_viewbuffer *view = ( hhn_viewbuffer *)*start;
		view->storedata_chart( currstep, step );
	}
}

/////////////////////////////////////////////////////////////////////////////
// hhn_input
void hhn_input::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hhn_input *inp = ( hhn_input *)*start;
		inp->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// feedback
#ifdef __MECHANICS__
void hhn_feedback::feedback( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hhn_feedback *fb = ( hhn_feedback *)*start;
		fb->feedback( step );
	}
}
#endif // __MECHANICS__

///////////////////////////////////////////////////////////////////////////////
// output (nsm object)
void hhn_output::reset( size_t currstep, double step, hhn_process **start ) // _id_PASS_RESET:
{
	for( ; *start != NULL; ++start ){
		hhn_output *out = ( hhn_output *)*start;
		out->reset( step );
	}
}

void hhn_output::calc_out( size_t currstep, double step, hhn_process **start ) // _id_PASS_NNUNIT
{
	for( ; *start != NULL; ++start ){
		hhn_output *out = ( hhn_output *)*start;
		out->calc_out( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// population
void hhn_populat::reset( size_t currstep, double step, hhn_process **start ) // _id_PASS_RESET
{
	for( ; *start != NULL; ++start ){
		hhn_populat *populat = ( hhn_populat *)*start;
		populat->reset( step );
	}
}

void hhn_populat::calc_out( size_t currstep, double step, hhn_process **start ) // _id_PASS_NNUNIT
{
	for( ; *start != NULL; ++start ){
		hhn_populat *populat = ( hhn_populat *)*start;
		populat->calc_out( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// neurons
void hhn_neuron::pre_y( size_t currstep, double step, hhn_process **start ) // _id_PASS_PREYS
{
	double vs[_id_MAX_SYN] = { 0. };
	hhn_neuron *hhn = ( hhn_neuron *)*start;
	if( *start != NULL ){
		hhn_compart *soma = hhn->Compart[_id_HHN_SOMA];
		for( size_t i = 0; i < _id_MAX_SYN; ++i ){
			vs[i] = soma->Syn[i]->Type->Vs;
		}
	}
	for( ; *start != NULL; ++start ){
		hhn = ( hhn_neuron *)*start;
		hhn->pre_y( step, vs );
	}
}

void hhn_neuron::pre_s( size_t currstep, double step, hhn_process **start ) // _id_PASS_PREYS
{
	double thresh = 0;
	double hv[_id_MAX_SYN] = { 0. },
		   sl[_id_MAX_SYN] = { 0. };
	hhn_neuron *hhn = ( hhn_neuron *)*start;
	if( *start != NULL ){
		thresh = hhn->Populat->Network->Threshold;
		hhn_compart *soma = hhn->Compart[_id_HHN_SOMA];
		for( size_t i = 0; i < _id_MAX_SYN; ++i ){
			hv[i] = soma->SSyn[i]->Hv;
			sl[i] = soma->SSyn[i]->Slp;
		}
	}
	for( ; *start != NULL; ++start ){
		hhn = ( hhn_neuron *)*start;
		hhn->pre_s( step, thresh, hv, sl );
	}
}

void hhn_neuron::pre_h( size_t currstep, double step, hhn_process **start )
{
	double kd[_id_MAX_SYN] = { 0. },
		   td[_id_MAX_SYN] = { 0. };
	hhn_neuron *hhn = ( hhn_neuron *)*start;
	if( *start != NULL ){
		hhn_compart *soma = hhn->Compart[_id_HHN_SOMA];
		for( size_t i = 0; i < _id_MAX_SYN; ++i ){
			kd[i] = soma->DSyn[i]->Kdep;
			td[i] = soma->DSyn[i]->Tdep;
		}
	}
	for( ; *start != NULL; ++start ){
		hhn = ( hhn_neuron *)*start;
		hhn->pre_h( step, kd, td );
	}
}

void hhn_neuron::post_y( size_t currstep, double step, hhn_process **start ) // _id_PASS_POSTY
{
	double expt[_id_MAX_SYN] = { 0. };
	hhn_neuron *hhn = ( hhn_neuron *)*start;
	if( *start != NULL ){
		hhn_compart *soma = hhn->Compart[_id_HHN_SOMA];
		for( size_t i = 0; i < _id_MAX_SYN; ++i ){
			expt[i] = EXP( -step/hhn->Compart[_id_HHN_SOMA]->Syn[i]->Type->Ts );
		}
	}
	for( ; *start != NULL; ++start ){
		hhn = ( hhn_neuron *)*start;
		hhn->post_y( step, expt );
	}
}
///////////////////////////////////////////////////////////////////////////////
// compartments
void hhn_compart::reset( size_t currstep, double step, hhn_process **start ) // _id_PASS_RESET
{
	for( ; *start != NULL; ++start ){
		hhn_compart *cmp = ( hhn_compart *)*start;
		cmp->reset();
	}
}

void hhn_compart::calc_vm( size_t currstep, double step, hhn_process **start ) // _id_PASS_VM
{
	hhn_compart *cmp = ( hhn_compart *)*start;
	double thresh = cmp->Populat->Network->Threshold;
	for( ; *start != NULL; ++start ){
		cmp = ( hhn_compart *)*start;
		cmp->calc_vm( step, thresh );
	}
}

void hhn_soma::calc_vm( size_t currstep, double step, hhn_process **start ) // _id_PASS_VM
{
	hhn_soma *soma = ( hhn_soma *)*start;
	double thresh = soma->Populat->Network->Threshold;
	for( ; *start != NULL; ++start ){
		soma = ( hhn_soma *)*start;
		soma->calc_vm( step, thresh );
	}
}

///////////////////////////////////////////////////////////////////////////////
// ions
void hhn_ions::reset( size_t currstep, double step, hhn_process **start ) // _id_PASS_RESET
{
	for( ; *start != NULL; ++start ){
		hhn_ions *ions = ( hhn_ions *)*start;
		ions->I = 0;
	}
}

void hhn_ions::calc_ieds( size_t currstep, double step, hhn_process **start ) // _id_PASS_IEDS
{
	for( ; *start != NULL; ++start ){
		hhn_ions *ions = ( hhn_ions *)*start;
		ions->calc_eds();
	}
}

#ifndef __RK__
// Simple Euler method
void hhn_ions::calc_idyn( size_t currstep, double step, hhn_process **start ) // _id_PASS_IDYN
{
	for( ; *start != NULL; ++start ){
		hhn_ions *ions = ( hhn_ions *)*start;
		ions->calc_idyn( step );
	}
}
#else
//	RK-2 
void hhn_ions::calc_idyn1( size_t currstep, double step, hhn_process **start ) // _id_PASS_IDYN_K1
{
	for( ; *start != NULL; ++start ){
		hhn_ions *ions = ( hhn_ions *)*start;
		ions->calc_idyn1( step );
	}
}

void hhn_ions::calc_idyn2( size_t currstep, double step, hhn_process **start ) // _id_PASS_IDYN_K2
{
	for( ; *start != NULL; ++start ){
		hhn_ions *ions = ( hhn_ions *)*start;
		ions->calc_idyn2( step );
	}
}
#endif

//////////////////////////////////////////////////////////////////////
// class hnak_pump
void hnak_pump::calc_ipump( size_t currstep, double step, hhn_process **start ) // _id_PASS_IPUMP or _id_PASS_IPUMP_K1
{
	for( ; *start != NULL; ++start ){
		hnak_pump *pump = ( hnak_pump *)*start;
		pump->calc_ipump( step );
	}
}

#ifdef __RK__
void hnak_pump::calc_ipump2( size_t currstep, double step, hhn_process **start ) // _id_PASS_IPUMP_K2
{
	for( ; *start != NULL; ++start ){
		hnak_pump *pump = ( hnak_pump *)*start;
		pump->calc_ipump2( step );
	}
}
#endif /*RK*/

//////////////////////////////////////////////////////////////////////
// class hca_pump
void hca_pump::calc_ipump( size_t currstep, double step, hhn_process **start ) // _id_PASS_IPUMP
{
	for( ; *start != NULL; ++start ){
		hca_pump *pump = ( hca_pump *)*start;
		pump->calc_ipump( step );
	}
}

#ifdef __RK__
void hca_pump::calc_ipump2( size_t currstep, double step, hhn_process **start ) // _id_PASS_IPUMP_K2
{
	for( ; *start != NULL; ++start ){
		hca_pump *pump = ( hca_pump *)*start;
		pump->calc_ipump2( step );
	}
}
#endif /*RK*/

///////////////////////////////////////////////////////////////////////////////
// channels
///////////////////////////////////////////////////////////////////////////////
// generic channel
void gen_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		gen_channel *chan = ( gen_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Na channel (generic)
void hgna_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hgna_channel *chan = ( hgna_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// NaP channel (generic)
void hgnap_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hgnap_channel *chan = ( hgnap_channel *)*start;
		chan->calc_g( step );
	}
}
///////////////////////////////////////////////////////////////////////////////
// Kdr channel (generic) generic Ptak
void hgkdr_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hgkdr_channel *chan = ( hgkdr_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// KA channel (generic)
void hgka_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hgka_channel *chan = ( hgka_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Model of generic K(Ca) channel for Hodgkin-Huxley type neuron 
void hgkca_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hgkca_channel *chan = ( hgkca_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Model of generic Ca activated cation nonspecific channel for Hodgkin-Huxley type neuron
void hgcan_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hgcan_channel *chan = ( hgcan_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// H channel (generic)
void hgh_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hgh_channel *chan = ( hgh_channel *)*start;
		chan->calc_g( step );
	}
}


///////////////////////////////////////////////////////////////////////////////
// Na fast channel (Butera)
void hnaf_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hnaf_channel *chan = ( hnaf_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Nap channel (Butera)
void hnap_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hnap_channel *chan = ( hnap_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Kdr channel (Butera)	generic Ptak
void hkdr_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hkdr_channel *chan = ( hkdr_channel *)*start;
		chan->calc_g( step );
	}
}

#if defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )
//--------- took from spcord project (locomotion)
///////////////////////////////////////////////////////////////////////////////
// Na channel (loco)
void hna_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hna_channel *chan = ( hna_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// K channel (loco & resp)
void hk_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hk_channel *chan = ( hk_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// KA channel (loco)
void hka_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hka_channel *chan = ( hka_channel *)*start;
		chan->calc_g( step );
	}
}
 
///////////////////////////////////////////////////////////////////////////////
// CaL channel (loco)
void hcal_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hcal_channel *chan = ( hcal_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// M channel (loco)
void hm_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hm_channel *chan = ( hm_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// CaT channel (loco)
void hcat_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hcat_channel *chan = ( hcat_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// CaN channel (loco)
void hcan_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hcan_channel *chan = ( hcan_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// KC channel (loco)
void hkc_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hkc_channel *chan = ( hkc_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// KCa channel (loco)
void hkca_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hkca_channel *chan = ( hkca_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// H channel (loco)
void hh_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hh_channel *chan = ( hh_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Leak channel (loco)
void hleak_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hleak_channel *chan = ( hleak_channel *)*start;
		chan->calc_g( step );
	}
}
#endif // defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )

///////////////////////////////////////////////////////////////////////////////
// Soma channel (loco)
void hsoma_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hsoma_channel *chan = ( hsoma_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Dendrite channel (loco)
void hdendr_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hdendr_channel *chan = ( hdendr_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// channels
//--------- took from nsm project (respiration)
#ifdef __RESPIRATION__
///////////////////////////////////////////////////////////////////////////////
// Na fast channel (resp)
void hrnaf_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hrnaf_channel *chan = ( hrnaf_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Nap channel (resp)
void hrnap_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hrnap_channel *chan = ( hrnap_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Kdr channel (resp) generic Ptak
void hrkdr_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hrkdr_channel *chan = ( hrkdr_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// KA channel (resp)
void hrka_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hrka_channel *chan = ( hrka_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// CaL channel (resp)
void hrcal_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hrcal_channel *chan = ( hrcal_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// CaT channel (resp)
void hrcat_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hrcat_channel *chan = ( hrcat_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// KCa channel (resp)
void hrkca_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hrkca_channel *chan = ( hrkca_channel *)*start;
		chan->calc_g( step );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Leak channel (resp)
void hrleak_channel::calc_g( size_t currstep, double step, hhn_process **start )
{
	for( ; *start != NULL; ++start ){
		hrleak_channel *chan = ( hrleak_channel *)*start;
		chan->calc_g( step );
	}
}
#endif // __RESPIRATION__

///////////////////////////////////////////////////////////////////////////////
// synapses
void hhn_synapse::reset( size_t currstep, double step, hhn_process **start ) // _id_PASS_RESET
{
	for( ; *start != NULL; ++start ){
		hhn_synapse *syn = ( hhn_synapse *)*start;
		syn->reset( step );
	}
}

void hhn_synapse::calc_prevm( size_t currstep, double step, hhn_process **start ) // _id_PASS_SYNAPSE
{
	for( ; *start != NULL; ++start ){
		hhn_synapse *syn = ( hhn_synapse *)*start;
		syn->calc_prevm( step );
	}
}
