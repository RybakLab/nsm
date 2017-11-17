#ifndef __PARSER_H_
#define __PARSER_H_

#include <algorithm>
#include <iterator>
#include <locale>
#include <map>
#include <set>
#include <string>
#include <vector>

using std::basic_string;
using std::ctype;
using std::isgraph;
using std::locale;
using std::make_pair;
using std::multimap;
using std::pair;
using std::reverse_iterator;
using std::set;
using std::vector;
using std::use_facet;


#ifdef __LINUX__
    const char *locale_name = "en_US";
#else
    const char *locale_name = "english";
#endif // LINUX

template <class _CharType>
class basic_parser{
      protected:
typedef     basic_string<_CharType> _Str;
typedef     _Str _Val;
typedef     _Str _Key;
typedef     multimap<_Key,_Val> _Map;
typedef     typename _Str::size_type _Str_pos;
typedef     typename vector<_Key>::size_type _Key_pos;
typedef     set<pair<_Key_pos, _Str_pos> > _Set_keys;
      public:
typedef     typename _Map::size_type size_type;
typedef     typename _Map::const_iterator const_iterator;
typedef     reverse_iterator<const_iterator> const_reverse_iterator;
      protected:
            class _less{
                  public:
                        bool operator()( const pair<_Key_pos, _Str_pos> &x, const pair<_Key_pos, _Str_pos> &y) const
                        {
                         return ( x.second < y.second );
                        }
                  };

      public:
explicit    basic_parser( const _CharType *keys, _CharType delim, bool case_sensitive = false, const char *loc = locale_name )
                        : _loc( loc )
            {
             _case_sensitive = case_sensitive;
             extract_keys( _Key( keys ), delim );
            };
explicit    basic_parser( const _Key &keys, _CharType delim, bool case_sensitive = false, const char *loc = locale_name )
                        : _loc( loc )
            {
             _case_sensitive = case_sensitive;
             extract_keys( keys, delim );
            };
            basic_parser( const basic_parser &x )
                        : _valmap( x._valmap ), _case_sensitive( x._case_sensitive ), _loc( x._loc )
            {
            };
           ~basic_parser( void )
            {
            };

      public:
            basic_parser &operator = ( const basic_parser &x )
            {
             _valmap = x._valmap;
             _case_sensitive = x._case_sensitive;
             _loc = x._loc;
             return *this;
            };

      public:
            const_iterator parse( const _CharType *line )
            {
             return parse( _Str( line ));
            };
            const_iterator parse( const _Str &line )
            {
             typedef typename _Set_keys::const_iterator it;
             _Set_keys all_keys;
             pre_parse( line, all_keys );
             for( it pos = all_keys.begin(), next_pos = pos; pos != all_keys.end(); ++pos, next_pos = pos ){
                  _Str_pos off = pos->second+_keys[pos->first].size();
                  _Str_pos size = ( ++next_pos != all_keys.end() )? next_pos->second-off:_Str::npos;
                  _Val val = line.substr( off, size );
                  add_val( _keys[pos->first], val );
                  }
             return begin();
            };

            const_iterator begin( void ) const
            {
             return _valmap.begin();
            };
            const_iterator end( void ) const
            {
             return _valmap.end();
            };
            const_reverse_iterator rbegin( void ) const
            {
             return _valmap.rbegin();
            };
            const_reverse_iterator rend( void ) const
            {
             return _valmap.rend();
            };

            const_iterator count( _Key &key ) const
            {
             return _valmap.count( key );
            };
            const_iterator find( _Key &key ) const
            {
             return _valmap.find( key );
            };
            pair<const_iterator, const_iterator> search( _Key &key ) const
            {
             return _valmap.equal_range( key );
            };

            bool empty( void ) const
            {
             return _valmap.empty();
            };
            size_type size( void ) const
            {
             return _valmap.size();
            };

      protected:
            void add_key( const _Key &x )
            {
             _Str key( x );
             remove_space( key );
             if( !key.empty() ){
                 if( std::find( _keys.begin(), _keys.end(), key ) == _keys.end() )
                     _keys.push_back( key );
                 }
            };
            void add_val( const _Key &key, const _Val &val )
            {
             _Val v( val );
             if( !_case_sensitive )
                 tolower( v );
             remove_space( v );
             _valmap.insert( make_pair( key, v ));
            };
            void extract_keys( const _Key &keys, _CharType delim )
            {
             typedef typename _Key::size_type size_type;
             for( size_type pos = 0, next_pos = 0; next_pos < _Key::npos; ++pos ){
                  next_pos = keys.find( delim, pos );
                  _Key key = keys.substr( pos, next_pos-pos );
                  add_key( key );
                  pos = next_pos;
                  }
            };
            template <class _InputIterator>
            pair<_InputIterator, _InputIterator> find_space( _Str &x, _InputIterator start, _InputIterator stop )
            {
	         _InputIterator pos = start, beg_pos = start, end_pos = start;
             for( pos = start, beg_pos = pos, end_pos = pos; pos != stop; ++pos ){
                  if( !isgraph( _CharType( *pos ), _loc ))
                      end_pos = pos+1;
                  else
                      break;
                  }
             return make_pair( beg_pos, end_pos );
            }
            void pre_parse_key( const _Str &line, _Set_keys &all_keys, _Key_pos key_pos )
            {
             for( _Str_pos pos = 0; pos < _Str::npos; ){
                  pos = line.find( _keys[key_pos], pos );
                  if( pos != _Str::npos ){
                      all_keys.insert( make_pair( key_pos, pos ));
                      pos += _keys[key_pos].size();
                      }
                  }
            };
            void pre_parse( const _Str &line, _Set_keys &all_keys )
            {
             for( _Key_pos key_pos = 0; key_pos < _keys.size(); ++key_pos ){
                  _Key k = _keys[key_pos];
                  pre_parse_key( line, all_keys, key_pos );
                  }
            };
            void remove_space( _Str &x )
            {
	     typedef typename _Str::iterator it;
	     typedef typename _Str::reverse_iterator rit;
             pair<it, it> beg_pos = find_space( x, x.begin(), x.end() );
             x.erase( beg_pos.first, beg_pos.second );
             pair<rit, rit> end_pos = find_space<rit> ( x, x.rbegin(), x.rend() );
             x.erase( end_pos.second.base(), end_pos.first.base() );
            };
            void tolower( _Str &x )
            {
             use_facet<ctype<_CharType> >( _loc ).tolower( &x[0], &x[x.size()] );
            };
      protected:
            bool _case_sensitive;
            locale _loc;
            vector<_Key> _keys;
            _Map _valmap;
      };

typedef basic_parser<char> parser;
typedef basic_parser<wchar_t> wparser;

#endif // __PARSER_H_
