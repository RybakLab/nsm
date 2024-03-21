#include "precompile.h"

#include "utilities.h"
#include "sptypes.h"
#include <stdio.h>

#ifdef __CONSOLE__

void message( const char *mess, const char *name )
{
 cout << "Warning! " << name << ": "<< mess << endl;
}

#else

void message( const char *mess, const char *name )
{
	::MessageBox( AfxGetMainWnd()->m_hWnd, mess, name, MB_ICONWARNING );
}

#endif // __CONSOLE__

bool remove_char( const char *filename, char ch )
{
 using std::remove;
 FILE *file = fopen( filename, "r+b" );
 if( file ){
     fseek( file, 0, SEEK_END );
     long file_size = ftell( file );
     fseek( file, 0, SEEK_SET );
     char *buffer = new char[file_size+1];
     fread( buffer, file_size, sizeof( char ), file );
     fclose( file );
     char *end_buffer = remove( buffer, buffer+file_size, 0x0D );
     size_t new_size = end_buffer-buffer;
     file = fopen( filename, "w+b" );
     fwrite( buffer, new_size, sizeof( char ), file );
     fclose( file );
     delete[] buffer;
     return true;
     }
 return false;
}

void unknown_string( istream &file, string str )
{
	if( str.length() == 0 ) return;
	if( str[0] == '<' ){ //Unknown tag
		string tag = str;
		tag[0] = '/';
		if( tag[tag.length()-1] == '>' )
			tag.erase(tag.length()-1);
		if( str !="<Comment>" ){
			str = "Unknown tag: " + str + "!";
			message( str.c_str(), "Warning!" );
		}
		while( file.ignore( INT_MAX , '<' )){
			getline( file, str, '>' );
			if( str == tag )
				break;
		}
	}
	else{ // Unknown variable
		str = "Unknown variable: " + str + "!";
		message( str.c_str(), "Warning!" );
		file.ignore(INT_MAX, '\n');
	}

}
