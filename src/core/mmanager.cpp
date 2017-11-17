///////////////////////////////////////////////////////////////////////////////
// mmanager.cpp.	
//	The simple memory manager is implemented here. It opimizes the allocation
//	of similar objects in the memory.
//-----------------------------------------------------------------------------
// Sergey Markin <smarkin@drexelmed.edu>
// 2008, Drexel University, College of Medicine
//-----------------------------------------------------------------------------
#include "precompile.h"

#include "mmanager.h"
#include <malloc.h>

template <class T>
T alignment( T x, unsigned char align )
{
	return ( x%align )? ( x/align+T( 1 ))*align: x;
}

///////////////////////////////////////////////////////////////////////////////
// class allocator
//-----------------------------------------------------------------------------
#define BYTE_SIZE	8
#define MORE0( x )	(( x ) > 0 )? ( x ): 1
///////////////////////////////////////////////////////////////////////////////
// constructors/destructor
//-----------------------------------------------------------------------------
allocator::allocator( unsigned short blk_size, unsigned short capacity, unsigned char align ) :
		Root( NULL ), AvaliableBlk( NULL )
{
	align = MORE0( align );
	blk_size = MORE0( blk_size );
	Capacity = MORE0( capacity );

	align = ::alignment( align, BYTE_SIZE );
	Align = align/BYTE_SIZE;
	BlkSize = ::alignment( blk_size, Align );
}

allocator::~allocator( void )
{
	free_all();
}

#undef MORE0
#undef BYTE_SIZE
///////////////////////////////////////////////////////////////////////////////
// methods
//-----------------------------------------------------------------------------
void allocator::free_all( void )
{
	for( chunk *node = Root; node != NULL; ){
		chunk *rm = node;
		node = node->Next;
		::free( rm );
	}
	AvaliableBlk = NULL;
	Root = NULL;
}

void *allocator::alloc( unsigned short size )
{
	if( size > 1 && size <= Capacity ){
		for( chunk *node = Root; node != NULL; node = node->Next ){
			if( node->FirstBlk+size <= Capacity ){
				node->FirstBlk += size;
				void *blk = AvaliableBlk;
				AvaliableBlk += BlkSize*size;
				return blk;
			}
		}
		return alloc_chunk( size );
	}
	else if( size == 1 ){
		if( Root && Root->FirstBlk < Capacity ){
			Root->FirstBlk++;
			void *blk = AvaliableBlk;
			AvaliableBlk += BlkSize;
			return blk;
		}
		return alloc_chunk();
	}
	return NULL;
}

void *allocator::alloc_chunk( unsigned short size )
{
	if( insert_chunk()){
		AvaliableBlk = Root->Data+BlkSize*size;
		Root->FirstBlk += size;
		return Root->Data;
	}
	return NULL;
}

bool allocator::insert_chunk( void )
{
	size_t chunk_size = ::alignment( sizeof( chunk ), Align );
	byte *mem_blk = ( byte *)::malloc( chunk_size+size_t( Capacity )*BlkSize+Align );
	if( mem_blk != NULL ){
		chunk *nroot = ( chunk * )mem_blk;
		nroot->FirstBlk = 0;
		nroot->Data = mem_blk+chunk_size+( Align-size_t( mem_blk )%Align );
		nroot->Next = Root;
		Root = nroot;
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// class mmanager
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// constructors/destructor
//-----------------------------------------------------------------------------
mmanager::mmanager( unsigned short capacity )
		: Capacity( capacity )
{
}

mmanager::~mmanager( void )
{
	for( ai blk = Allocators.begin(); blk != Allocators.end(); ++blk ){
		delete blk->second;
	}
}

///////////////////////////////////////////////////////////////////////////////
// methods
//-----------------------------------------------------------------------------
bool mmanager::reg_type( const char *btype, unsigned short bsize, short capacity, unsigned char align )
{
	capacity = ( capacity == -1 )? Capacity: capacity;
	if( Allocators.find( string( btype )) == Allocators.end() ){
		Allocators[btype] = new allocator( bsize, capacity, align );
		return true;
	}
	return false;
}

void *mmanager::alloc( const char *btype, unsigned short size )
{
	ai blk = Allocators.find( btype );
	if( blk != Allocators.end() ){
		return ( blk->second )->alloc( size );
	}
	return NULL;
}

void mmanager::free_all( void )
{
	for( ai blk = Allocators.begin(); blk != Allocators.end(); ++blk ){
		( blk->second )->free_all();
	}
}
