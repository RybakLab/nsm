#ifndef _CMATRIX_H_
#define _CMATRIX_H_

#include "sptypes.h"

template <class T>
class CMatrix{
typedef	typename vector<T>::size_type size_type;
	public:
		CMatrix( void ) : NumRow( 0 ), NumCol( 0 ){};
		CMatrix( const CMatrix<T> &matrix ) : NumRow( matrix.NumRow ), NumCol( matrix.NumCol ), Matrix( matrix.Matrix ){};
		CMatrix( size_t row, size_t col, T element = T() ) : NumRow( row ), NumCol( col ), Matrix( row*col, element){};
		~CMatrix( void ){};
	public:
		T &operator()( size_t row, size_t col ){ return Matrix[row*NumCol+col]; };
		CMatrix<T> &operator =( const CMatrix<T> &matrix )
		{
			NumRow = matrix.NumRow; NumCol = matrix.NumCol; Matrix = matrix.Matrix;
			return *this;
		};
	public:
		size_t size( void ) const { return Matrix.size(); };
		size_t GetNumRow( void ) const{ return NumRow; };
		size_t GetNumCol( void ) const{ return NumCol;};
		vector<T> &GetMatrix( void ){ return Matrix;};
		vector<T> &GetCol( size_t index )
		{
			Str.clear();
			if( index < NumCol ){
				Str.resize( NumRow );
				for( size_t row = 0; row < NumRow; row++ ){
					size_t offset = row*NumCol+index;
					Str[row] = Matrix[offset];
				}
			}
			return Str;
		};
		vector<T> &GetRow( size_t index )
		{
			Str.clear();
			if( index < NumRow ){
				Str.resize( NumCol );
				for( size_t col = 0; col < NumCol; col++ ){
					size_t offset = index*NumCol+col;
					Str[col] = Matrix[offset];
				}
			}
			return Str;
		};
		size_type CreateMatrix( size_t row, size_t col, T element = T() )
		{
			NumRow = row;
			NumCol = col;
			Matrix.resize( row*col, element );
			return Matrix.size();
		};
		size_type CreateMatrix( const CMatrix &matrix )
		{
			NumRow = matrix.NumRow;
			NumCol = matrix.NumCol;
			Matrix = matrix.Matrix;
			return Matrix.size();
		};
		size_t AddRow( void )
		{
			NumRow++;
			Matrix.resize( NumRow*NumCol, T());
			return NumRow;
		};
		size_t AddCol( void ) // improve performance!!!!!!!
		{
			size_t n_col = NumCol+1;
			Matrix.resize( NumRow*n_col, T());
			for( size_t row = NumRow; row > 1; row-- ){
				size_t off_old = ( row-1 )*NumCol;
				size_t off_new = ( row-1 )*n_col;
				copy_backward( Matrix.begin()+off_old, Matrix.begin()+off_old+NumCol, Matrix.begin()+off_new+NumCol );
			}
			for( size_t row = 0; row < NumRow; row++ ){
				size_t offset = row*n_col+NumCol;
				Matrix[offset] = T();
			}
			NumCol++;
			return NumCol;
		};
		bool SetRow( const vector<T> &row, size_t index )
		{
			if( index < NumRow ){
				for( size_t col = 0; col < NumCol; col++ ){
					size_t offset = index*NumCol+col;
					if( col < row.size() )
						Matrix[offset] = row[col];
					else
						Matrix[offset] = T();
				}
				return true;
			}
			return false;
		};
		bool SetCol( const vector<T> &col, size_t index )
		{
			if( index < NumCol ){
				for( size_t row = 0; row < NumRow; row++ ){
					size_t offset = row*NumCol+index;
					if( row < col.size() )
						Matrix[offset] = col[row];
					else
						Matrix[offset] = T();
				}
				return true;
			}
			return true;
		};
		size_t InsertRow( const vector<T> &row, size_t index )
		{
			if( index < NumRow ){
				for( size_t col = 0; col < NumCol; col++ ){
					size_t offset = index*NumCol+col;
					if( col < row.size() )
						Matrix.insert( Matrix.begin()+offset, row[col] );
					else
						Matrix.insert( Matrix.begin()+offset, T() );
				}
				NumRow++;
			}
			return NumRow;
		};
		size_t InsertCol( const vector<T> &col, size_t index )
		{
			if( index < NumCol ){
				for( size_t row = 0; row < NumRow; row++ ){
					size_t offset = row*( NumCol+1 )+index;
					if( row < col.size() ){
						Matrix.insert( Matrix.begin()+offset, col[row] );
					}
					else{
						Matrix.insert( Matrix.begin()+offset, T() );
					}
				}
				NumCol++;
			}
			return NumCol;
		};
		bool SwapRow( size_t row1, size_t row2 )
		{
			if( row1 < NumRow && row2 < NumRow ){
				for( size_t col = 0; col < NumCol; col++ ){
					size_t offset1 = row1*NumCol+col;
					size_t offset2 = row2*NumCol+col;
					swap( Matrix[offset1], Matrix[offset2] );
				}
				return true;
			}
			return false;
		}
		bool SwapCol( size_t col1, size_t col2 )
		{
			if( col1 < NumCol && col2 < NumCol ){
				for( size_t row = 0; row < NumRow; row++ ){
					size_t offset1 = row*NumCol+col1;
					size_t offset2 = row*NumCol+col2;
					swap( Matrix[offset1], Matrix[offset2] );
				}
				return true;
			}
			return false;
		}
		size_t DelRow( size_t index )
		{
			if( index < NumRow ){
				size_t offset_first = ( index )*NumCol;
				size_t offset_last = ( index+1 )*NumCol;
				Matrix.erase( Matrix.begin()+offset_first, Matrix.begin()+offset_last);
				NumRow--;
			}
			return NumRow;
		};
		size_t DelCol( size_t index )
		{
			if( index < NumCol ){
				size_t offset;
				for( int row = NumRow-1; row >= 0; row-- ){
					offset = row*NumCol+index;
					Matrix.erase( Matrix.begin()+offset );
				}
				NumCol--;
			}
			return NumCol;
		};
		void ClearMatrix( void )
		{
			NumRow = NumCol = 0;
			Str.clear();
			Matrix.clear();
		}
	private:
		size_t NumRow;
		size_t NumCol;
		vector<T> Matrix;        // format: row1[NumCol], row2[NumCol], ...
		vector<T> Str;
};

#endif // _CMATRIX_H_
