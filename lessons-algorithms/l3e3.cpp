#include <cassert>
#include <iostream>
#include <deque>
#include <vector>
#include <memory>

using namespace std;
static const unsigned int num_limit=1000000;

struct dyn_deque_s {
    dyn_deque_s( deque<pair<int,int>>& points,
                 size_t size )
        :
          buffer_size( size ),
          unique_segments_length( 0 ),
          buffer ( points )
    {
        tmp.reserve( buffer_size );
    }

    ~dyn_deque_s() {}

    size_t unique_painting() {
        return unique_segments_length;
    }


    void sort_by_merge() {

    }


private:

    void _sort_by_merge( int index, int len )
    {
        if( len <= 1 ) {
            return;
        }

        int l_len = len / 2;
        int r_len = len - l_len;

        _sort_by_merge( index, l_len );
        _sort_by_merge( index+l_len, r_len );
        _merge( index, l_len, index+l_len, r_len );
    }


    void _merge( int li, int ll, int ri, int rl )
    {
        int i=0,j=0;
        for( ; i<ll && j<rl ; ) {
            if( buffer[i].first <= buffer[j].first ) {
                _merge_segments( buffer[i], buffer[j] );
                ++i;
            }
            else {
                _merge_segments( buffer[i], buffer[j] );
                ++j;
            }
        }

        if( i == ll ) {
            for( ; j<rl ; ++j ) {
                _merge_segments( buffer[i] );
            }
        }

        else {
            for( ; i<ll ; ++i ) {
                _merge_segments( buffer[j] );
            }
        }
    }

    void _merge_segments( pair<int,int>& x )
    {
        tmp.push_back( buffer[i] );
    }

    void _merge_segments( pair<int,int>& x, pair<int,int>& y )
    {
        int i=x.first,j=y.first;
        for( ; i<x.second && j=y.second ; ) {
            if( i <= j ) {
                tmp.push_back( i );
                ++i;
            }
            else {
                tmp.push_back( j );
                ++j;
            }

            if(  ) {

            }

        }

    }

    size_t buffer_size;
    size_t unique_segments_length;
    vector<int> tmp;
    deque<pair<int,int>>& buffer;
};


typedef struct dyn_deque_s dyn_deque_t;


bool
get_size_from_stdin( size_t* s )
{
    cin >> *s;

    if ( cin.fail() ) {
        *s=0;
        return false;
    }

    return true;
}



bool
get_points_from_stdin( deque<pair<int,int>>& points )
{
    int x=0,y=0;

    while( !cin.eof() ) {
        cin >> x;
        cin >> y;
        points.push_back( pair<int,int>(x,y) );
    }

    return true;
}



int main()
{
    size_t size;
    deque<pair<int,int>> plot;

    get_size_from_stdin( &size );
    if( !get_points_from_stdin( points ))
        return 0;

    dyn_deque_t plot( points, size );
    deque<pair<int,int>>::const_iterator iter=points.cbegin();

    for( ; iter<points.cend(); ++iter ) {
        cout << (*iter).first << " " << (*iter).second << endl;
    }

    return 0;
}
