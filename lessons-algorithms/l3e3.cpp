#include <cassert>
#include <iostream>
#include <deque>
#include <vector>
#include <memory>

using namespace std;
static const unsigned int num_limit=1000000;

struct dyn_deque_s {
    dyn_deque_s( deque<pair<int,int>>& p,
                 size_t s )
        :
          points_num( s ),
          unique_segments_length( 0 ),
          points ( p )
    {
        for( auto v : points ) {
            for( int i=v.first; i<=v.second ; ++i ) {
                values.push_back( i );
            }
        }
    }

    ~dyn_deque_s() {}

    size_t unique_values_number() {

        if( values.size() == 0 )
            return 0;

        int tmp=values[0];
        int cuniq=1;

        bool deleted=false;
        unique_segments_length=0;

        for( int i=1; i<values.size(); ++i )
        {
            if( tmp == values[i] ) {
                if( !deleted ) {
                    --cuniq;
                    deleted=true;
                }
                continue;
            }

            ++cuniq;
            tmp=values[i];
            deleted=false;
        }

        return unique_segments_length;
    }


    void sort() {
        __sort_by_merge( 0, values.size() );

        for( auto v : values ) {
            cout << v << " ";
        }
    }


private:

    void __sort_by_merge( int index, int len )
    {
        if( len <= 1 ) {
            return;
        }

        int l_len = len / 2;
        int r_len = len - l_len;

        __sort_by_merge( index, l_len );
        __sort_by_merge( index+l_len, r_len );

        int* tmp = new int[len];
        __merge( index, l_len, index+l_len, r_len, tmp );

        while( --len >= 0 ) {
            values[index+len]=tmp[len];
        }

        delete[] tmp;
    }


    int
    __merge( int li, int ll, int ri, int rl, int* tmp )
    {
        int i=0,j=0;
        for( ; i<ll && j<rl ; ) {

            if( values[li+i] <= values[ri+j] ) {
                tmp[i+j] = values[li+i];
                ++i;
            }

            else {
                tmp[i+j] = values[ri+j];
                ++j;
            }

        }

        if( i == ll ) {
            for( ; j<rl ; ++j ) {
                tmp[i+j] = values[ri+j];
            }
        }

        else {
            for( ; i<ll ; ++i ) {
                tmp[i+j] = values[li+i];
            }
        }
    }


    size_t points_num;
    size_t unique_segments_length;
    vector<int> values;
    deque<pair<int,int>>& points;
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
    deque<pair<int,int>> points;

    get_size_from_stdin( &size );
    if( !get_points_from_stdin( points ))
        return 0;

    dyn_deque_t plot( points, size );
    plot.sort();
    cout << plot.unique_values_number() << endl;

    return 0;
}
