#include <cassert>
#include <iostream>
#include <deque>
#include <vector>
#include <memory>

using namespace std;
static const unsigned int num_limit=1000000;

struct dyn_deque_s {
    dyn_deque_s( vector<int>& numbers,
                 size_t num)
        :
        data( numbers ),
        ksize( num )
    {
        size_t tmp_size=data.size();
        for( int x=0; x < tmp_size ; x+=ksize ) {
            slices_points.push_back( pair<int,int>( x, 0 ) );
            for ( ssize_t i=( ksize / 2 ) - 1; i>=0; --i ) {
                __sift_down( i, ksize - 1, x );
            }
        }

    }

    void dump()
    {
        for( auto v : data ) {
            cout << v << ":";
        }
    }

    void sorted_dump()
    {
        size_t tmp_size=data.size();

        for( ; tmp_size; --tmp_size )
        {
            size_t minimum=-1;
            int x=0, y=0, it=0, last_it=0;

            for( ; it < slices_points.size(); ++it )
            {
                if( slices_points[it].second == ksize )
                    continue;

                x=slices_points[it].first;
                y=slices_points[it].second;

                if( minimum > data[x] ) {
                    minimum = data[x];
                    last_it=it;
                }
            }

            x=slices_points[last_it].first;
            y=slices_points[last_it].second;
            slices_points[last_it].second+=1;

            cout << data[x] << " ";

            data[x]=0;
            swap( data[x], data[x+ksize-(y+1)] );

            __sift_down( 0, ksize-(y+2), x );
        }
    }

    ~dyn_deque_s() {}


private:

    void __sift_down( size_t from, size_t to, int x )
    {
        size_t left=from*2+1+x;
        size_t right=from*2+2+x;
        to+=x;from+=x;
        size_t minimum=from;

        if ( left <= to &&
             ( data[from] > data[left] ||
               ( data[from] == data[left] &&
                 data[from] > data[left] ))) {
            minimum=left;
        }

        if ( right <= to &&
             ( data[minimum] > data[right] ||
               ( data[minimum] == data[right] &&
                 data[minimum] > data[right] ))) {
            minimum=right;
        }

        if ( minimum != from ) {
            swap( data[minimum], data[from] );
            __sift_down( minimum - x, to - x, x );
        }
    }

    size_t ksize;
    vector<int>& data;
    vector<pair<int,int>> slices_points;
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
get_points_from_stdin( vector<int>& numbers,
                       size_t size)
{
    int x=0;
    for( ; size; --size ) {
        cin >> x;
        numbers.push_back( x );
    }

    return true;
}





int main()
{
    size_t size;
    size_t kvalue;
    vector<int> numbers;

    get_size_from_stdin( &size );

    if( !get_points_from_stdin( numbers, size ))
        return 0;

    get_size_from_stdin( &kvalue );

    dyn_deque_t vec( numbers, kvalue );
    vec.sorted_dump();

    return 0;
}
