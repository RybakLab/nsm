///////////////////////////////////////////////////////////////////////////////
// mmanager.h
//-----------------------------------------------------------------------------
#ifndef __MEMORY_MAN
#define __MEMORY_MAN

#include <map>
#include <new>
#include <string>

const unsigned char __align = 32;

///////////////////////////////////////////////////////////////////////////////
// allocator
//-----------------------------------------------------------------------------
class allocator{
typedef  	unsigned char byte;
typedef  	struct __fat_chunk{
			unsigned short FirstBlk;	//
			byte *Data;					//
			struct __fat_chunk *Next;	//
		} chunk;
	public: //--- constructors/destructor
		allocator( unsigned short blk_size, unsigned short capacity, unsigned char align = __align );
		~allocator( void );
	public: //--- class methods
		void *alloc( unsigned short size = 1 );
		void free_all( void );
	private:
		void *alloc_chunk( unsigned short size = 1 );
		bool insert_chunk( void );
	protected: //--- data
		chunk *Root;		//
		byte *AvaliableBlk;	//
		unsigned char Align;	//
		unsigned short BlkSize;	//
		unsigned short Capacity;//
};

using	std::map;
using	std::string;
///////////////////////////////////////////////////////////////////////////////
// mmanager
//-----------------------------------------------------------------------------
class mmanager{
typedef  	map<string, allocator *> alist;
typedef  	map<string, allocator *>::const_iterator ai;
	public: //--- constructors/destructor
		mmanager( unsigned short capacity );
		~mmanager( void );
	public: //--- class methods
		bool reg_type( const char *btype, unsigned short bsize, short capacity = -1, unsigned char align = 32 );
		void *alloc( const char *btype, unsigned short size = 1 );
		void free_all( void );
	private: //--- data
		alist Allocators;		//
		unsigned short Capacity;//
};

#endif // __MEMORY_MAN
