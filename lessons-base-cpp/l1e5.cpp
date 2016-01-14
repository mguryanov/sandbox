#include <iostream>
#include <vector>

using namespace std;


void
get_matrix_size_from_stdin( pair<size_t,size_t>& s )
{
    cin >> s.first;
    cin >> s.second;
}



void
get_matrix_from_stdin( vector<int>& v,
                       size_t size)
{

    for( size_t i=0, l=size; i<l; ++i ) {
        cin >> v[i];
    }
}



int main()
{
    pair<size_t,size_t> matrix_size;
    vector<int> matrix;

    get_matrix_size_from_stdin( matrix_size );

    size_t size=matrix_size.first*matrix_size.second;
    matrix.reserve( size );

    get_matrix_from_stdin( matrix, size );

    for( size_t c=0; c<matrix_size.second; ++c ) {
        for( size_t r=0; r<matrix_size.first; ++r ) {
            cout << matrix[matrix_size.second*r+c] << " ";
        }
    }

    return 0;
}
