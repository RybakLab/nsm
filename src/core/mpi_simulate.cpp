/////////////////////////////////////////////////////////////////////////////
// mpi_simulate.cpp
#include "precompile.h"
#include "mpi_simulate.h"

#ifdef __MPI__

mpi_subnet::mpi_subnet( void )
{
}

mpi_subnet::mpi_subnet( const mpi_subnet &net )
	: IProc( net.IProc ), OProc( net.OProc ), IOMap( net.IOMap ), Subnet( net.Subnet )
{
}

mpi_subnet::~mpi_subnet( void )
{
}

mpi_subnet &mpi_subnet::operator = ( const mpi_subnet &net )
{
	IProc = net.IProc;
	OProc = net.OProc;
	IOMap = net.IOMap;
	Subnet = net.Subnet;
	return *this;
}

int mpi_subnet::idin( const string &name, int typenn )
{
	for( size_t i = 0; i < IOMap.size(); ++i ){
		if( name == IOMap[i].Name && typenn == IOMap[i].TypeNN )
			return IOMap[i].ID;
	}
	return -1;
}

int mpi_subnet::idout( const string &name )
{
	for( size_t i = 0; i < IOMap.size(); ++i ){
		if( name == IOMap[i].Name && -1 == IOMap[i].TypeNN )
			return IOMap[i].ID;
	}
	return -1;
}

bool mpi_subnet::load_mod( const char *filename )
{
	if( Subnet.load_mod( filename )){
		return add_hlayers();
	}
	return false;
}

void mpi_subnet::save_dat( const char *filename, double prec )
{
	Subnet.save_dat( filename, prec );
}

void mpi_subnet::init( long seed )
{
	Subnet.init( seed );
	add_hlayers();
}

bool mpi_subnet::add_hlayers( void )
{
	vector<string> src_name = Subnet.Network.nnconnect().name_src();
	vector<string> trg_name = Subnet.Network.nnconnect().name_trg();
	vector<nn_unit *> src;
	for( size_t i = 0; i < src_name.size(); src.push_back( Subnet.Network.get_nnunit( src_name[i] )), ++i );
	vector<nn_unit *> trg;
	for( size_t i = 0; i < trg_name.size(); trg.push_back( Subnet.Network.get_nnunit( trg_name[i] )), ++i );
	IOMap.clear();
	add_mpiout( src );
	add_mpiin( trg );
	return false;
}

void mpi_subnet::add_mpiout( vector<nn_unit *> src )
{
	OProc.clear();
	for( size_t i = 0; i < src.size(); ++i ){ // number of sources = number of mpi_out elements
		mpi_out oproc;
		OProc.push_back( oproc );
		OProc[i].init();
		OProc[i].reg_unit();
		OProc[i].connect( src[i], CNNConnect() );
		mpi_map iomap;
		iomap.Name = src[i]->get_name(); iomap.ID = i; iomap.TypeNN = -1;
		IOMap.push_back( iomap );
	}
}

void mpi_subnet::add_mpiin( vector<nn_unit *> trg )
{
	IProc.clear();
	for( size_t i = 0; i < trg.size(); ++i ){
		for( size_t j = 0; j < trg[i]->connect_type().Connect.size(); j++ ){
			mpi_in iproc;
			IProc.push_back( iproc );
			size_t iproc_id = IProc.size()-1;
			IProc[iproc_id].init( trg[i] );
			IProc[iproc_id].reg_unit();
			CConnect weight = trg[i]->connect_type().Connect[j];
			weight.Connect = 1.;
			trg[i]->add_connect( &IProc[iproc_id], weight );
			mpi_map iomap;
			iomap.Name = trg[i]->get_name(); iomap.ID = iproc_id; iomap.TypeNN = trg[i]->connect_type().Connect[j].Type; 
			IOMap.push_back( iomap );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// class mpi_simulate
mpi_simulate::mpi_simulate( void )
	: NumSteps( 0 ), CurrStep( 0 ),
	Seed( -1 ), SimStep( 0.1), SimTime( 1000.0 )
{
}

mpi_simulate::~mpi_simulate( void )
{
}

bool mpi_simulate::load_mod( const char *mpi_model )
{
	bool load_ok = false;
	string str;
	ifstream file( mpi_model );
	while( file >> str ){
		if( str == "Generated" ){
			getline( file, str );
		}
		else if( str == "<common>" ){
			load_ok = get_param( file );
		}
		else if( str == "<mpi subnets>" ){
			load_ok = get_subnet( file );
		}
		else if( str == "<ipc connect>" ){ // Potential bug: <ipc connect> tab must be the last one in the file. 
			load_ok = get_ipconnect( file );
		}
		else{
			unknown_string( file, str );
		}
	}
	if( load_ok ){
		CStat::Reset( Seed );
		for( size_t i = 0; i < Subnets.size(); ++i ){
			Subnets[i].Subnet.SimData.SimStep = SimStep;
			Subnets[i].Subnet.SimData.SimTime = SimTime;
			Subnets[i].init( CStat::seed() );
		}
		ModelName = mpi_model;
		Promt = "The model "; Promt += mpi_model; Promt += " is running. Please wait...";
		cout << "Loading the model " << mpi_model << ". Completed        " << endl;
		return true;
	}
	return false;
}

void mpi_simulate::start_sim( void )
{
	init_mod();
	cout << "Start simulation" << endl;
	long t0 = long( time( NULL ));
	long slice = NumSteps/100; long percent = 0;
	for( ; CurrStep < NumSteps; next_step(), ++CurrStep )
		if( CurrStep%slice == 0 ){
			cout << Promt << std::setw( 3 ) << percent << "%\r";
			++percent;
		}
	shut_mod();
	cout << endl << "The simulation is complete. Total duration is " << long( time( NULL ))-t0 << " sec" << endl;
}

void mpi_simulate::save_dat( const char *filename, double prec )
{
	for( size_t i = 0; i < Subnets.size(); ++i ){
		string subnet_name = filename; subnet_name += ".";
		subnet_name += Subnets[i].Subnet.SimData.ModelName;
		Subnets[i].Subnet.save_dat( subnet_name.c_str(), prec );
	}
}

bool mpi_simulate::get_param( istream &file )
{
	string str;
	while( file >> str){
		if( str == "Seed" ){		//parse line which is similar to: Seed = 1234
			file >> str >> Seed;
		}
		else if( str == "Step" ){
			file >> str >> SimStep;
		}
		else if( str == "Time" ){
			file >> str >> SimTime;
		}
		else if( str == "</common>"){
			return true;
		}
	}
	message( "Missing tag </common>!", "Warning!" );
	return false;
}

bool mpi_simulate::get_subnet( istream &file )
{
	Subnets.clear();
	string str;
	while( file >> str){
		if( str == "subnet" ){		//parse line which is similar to: sublet = filename
			file >> str;
			getline( file, str );
			Subnets.push_back( mpi_subnet());
			Subnets[ Subnets.size()-1].load_mod( str.c_str());
		}
		else if( str == "</mpi subnets>"){
			return true;
		}
	}
	message( "Missing tag </mpi subnets>!", "Warning!" );
	return false;
}

bool mpi_simulate::get_ipconnect( istream &file )
{
	IPCMap.clear();
	string str;
	while( file >> str ){
		if( str == "<source"){
			file >> ws;
			getline( file, str, '>' );	// str <- source name
			get_source( str, file );
		}
		else if( str == "</ipc connect>" )
			return true;
		else
			unknown_string( file, str );
	}
	message( "Missing tag </ipc connect>!", "Warning!" );
	return false;
}

bool mpi_simulate::get_source( const string &name, istream &file )
{
	ipc_link from = getl_from( name );
	if( from.SubnetID > 0 && from.UnitID > 0 ){
		string str;
		while( file >> str ){
			if( str == "Proc"){
				int proc = 0;
				file >> str >> proc;
				from.Proc = ( proc != 0 );
			}
			else if( str == "<target " ){
				ipc_connect connect;
				connect.From = from;
				file >> ws;
				getline( file, str, '>' );	// str <- target name
				get_target( connect, str, file );
			}
			else if( str == "</source>" )
				return true;
			else
				unknown_string( file, str );
		}
		message( "Missing tag </source>!", "Warning!" );
	} 
	return false;
}

bool mpi_simulate::get_target( ipc_connect connect, const string &name, istream &file )
{
	double weight = 0.0;
	int sid = -1;
	int type = -1;
	int proc = 0;
	string str;
	while( file >> str ){
		if( str == "<connect>"){
			get_connect( weight, type, sid, proc, file );
			if( sid != -1 && type != -1 ){	// connection exists
				ipc_link to = getl_to( name, type ); 
				to.Proc = ( proc != 0 );
				to.SynID = sid;
				connect.To = to;
				connect.Weight = weight;
				IPCMap.push_back( connect );
			}
		}
		else if( str == "</target>" ){
			return true;
		}
		else
			unknown_string( file, str );
	}
	message( "Missing tag </target>!", "Warning!" );
	return false;
}

bool mpi_simulate::get_connect( double &weight, int &type, int &sid, int &proc, istream &file )
{
	string str;
	while( file >> str ){
		if( str == "Type"){
			file >> str >> type;
		}
		else if( str == "Proc"){
			file >> str >> proc;
		}
		else if( str == "Ex"){
			file >> str >> weight;
			if( weight > 0.0 ) sid = _id_ExSyn;
		}
		else if( str == "Ex2"){
			file >> str >> weight;
			if( weight > 0.0 ) sid = _id_ExSyn+1;
		}
		else if( str == "InhI"){
			file >> str >> weight;
			if( weight > 0.0 ) sid = _id_InhASyn;
		}
		else if( str == "InhII"){
			file >> str >> weight;
			if( weight > 0.0 ) sid = _id_InhBSyn;
		}
		else if( str == "</connect>" ){
			return true;
		}
		else
			unknown_string( file, str );
	}
	message( "Missing tag </connect>!", "Warning!" );
	return false;
}

ipc_link mpi_simulate::getl_from( const string &name )
{
	ipc_link link;
	for( size_t nid = 0; nid < Subnets.size(); ++nid ){
		int uid = Subnets[nid].idout( name );
		if( uid != -1 ){
			link.SubnetID = nid;
			link.UnitID = uid;
			break;
		}
	}
	return link;
}

ipc_link mpi_simulate::getl_to( const string &name, int type )
{
	ipc_link link;
	for( size_t nid = 0; nid < Subnets.size(); ++nid ){
		int uid = Subnets[nid].idin( name, type );
		if( uid != -1 ){
			link.SubnetID = nid;
			link.UnitID = uid;
			break;
		}
	}
	return link;
}

void mpi_simulate::init_mod( void )
{
	CurrStep = 0;
	for( size_t i = 0; i < Subnets.size(); ++i ){
		Subnets[i].Subnet.init_mod();
	}
}

void mpi_simulate::shut_mod( void )
{
	for( size_t i = 0; i < Subnets.size(); ++i ){
		Subnets[i].Subnet.shut_mod();
	}
}

void mpi_simulate::next_step( void )
{
	for( size_t i = 0; i < IPCMap.size(); ++i ){
		double signal = get_from( i );
		put_to( i, signal );
	}
	for( size_t i = 0; i < Subnets.size(); ++i ){
		Subnets[i].Subnet.run_slice( 1 );
	}
}

double mpi_simulate::get_from( size_t id )
{
	double signal = 0;
	Subnets[IPCMap[id].From.SubnetID].out( IPCMap[id].From.UnitID, signal, IPCMap[id].From.Proc );
	return signal;
}

void mpi_simulate::put_to( size_t id, double signal )
{
	signal *= IPCMap[id].Weight;
	Subnets[IPCMap[id].To.SubnetID].in( IPCMap[id].To.UnitID, signal, IPCMap[id].To.SynID, IPCMap[id].To.Proc );
}

#endif // __MPI__
