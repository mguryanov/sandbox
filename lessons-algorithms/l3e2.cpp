#include <cassert>
#include <iostream>
#include <deque>
#include <vector>
#include <memory>

using namespace std;
static const unsigned int num_limit=1000000;

struct dyn_deque_s {
    dyn_deque_s( deque<pair<int,int>>& points )
        :
        buffer ( points )
    {
        size_t tmp_size=buffer.size();
        for ( ssize_t i=( tmp_size / 2 )-1; i>=0; --i ) {
            _sift_down( i, tmp_size-1 );
        }
    }

    ~dyn_deque_s() {}

    void build() {
        for ( size_t i=buffer.size()-1; i>0; ) {
            swap( buffer[0], buffer[i] );
            _sift_down( 0, --i );
        }
    }


private:

    void _sift_down( size_t from, size_t to )
    {
        size_t left=from*2+1;
        size_t right=from*2+2;
        size_t largest=from;

        if ( left <= to &&
             ( buffer[from].first < buffer[left].first ||
               ( buffer[from].first == buffer[left].first &&
                 buffer[from].second < buffer[left].second ))) {
            largest=left;
        }

        if ( right <= to &&
             ( buffer[largest].first < buffer[right].first ||
               ( buffer[largest].first == buffer[right].first &&
                 buffer[largest].second < buffer[right].second ))) {
            largest=right;
        }

        if ( largest != from ) {
            swap( buffer[largest], buffer[from] );
            _sift_down( largest, to );
        }
    }

    size_t buffer_size;
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
    deque<pair<int,int>> points;

    get_size_from_stdin( &size );
    if( !get_points_from_stdin( points ))
        return 0;

    dyn_deque_t curve( points );
    curve.build();

    deque<pair<int,int>>::const_iterator iter=points.cbegin();

    for( ; iter<points.cend(); ++iter ) {
        cout << (*iter).first << " " << (*iter).second << endl;
    }

    return 0;
}
