#include <cassert>
#include <iostream>
#include <deque>
#include <vector>
#include <memory>

using namespace std;
static const unsigned int num_limit=1000000;

struct dyn_deque_s {
    dyn_deque_s( vector<pair<int,bool>>& p,
                 size_t s )
        :
          points_num( s ),
          xpoints ( p )
    {}

    ~dyn_deque_s() {}

    size_t unique_values_number() {

        if( xpoints.size() <= 1 )
            return 0;

        if( xpoints.size() == 2 ) {
            return xpoints[1].first - xpoints[0].first;
        }

        size_t result=0;
        ssize_t e=1;

        for( int i=1; i<xpoints.size(); ++i )
        {
            if( !xpoints[i].second ) {

                if( !xpoints[i-1].second && e==1 ) {
                    result+=xpoints[i].first-xpoints[i-1].first;
                }

                else if( xpoints[i-1].second && e==1 ) {
                    result+=xpoints[i].first-xpoints[i-1].first;
                }

                ++e;
            }

            else {
                if( xpoints[i-1].second && e<=1) {
                    result+=xpoints[i].first-xpoints[i-1].first;
                }

                else if( !xpoints[i-1].second && e==1 ) {
                    result+=xpoints[i].first-xpoints[i-1].first;
                }

                --e;
            }
        }

        return result;
    }


    void sort() {
        __sort_by_merge( 0, xpoints.size() );
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

        pair<int,bool>* tmp = new pair<int,bool>[len];
        __merge( index, l_len, index+l_len, r_len, tmp );

        while( --len >= 0 ) {
            xpoints[index+len]=tmp[len];
        }

        delete[] tmp;
    }


    int
    __merge( int li, int ll, int ri, int rl, pair<int,bool>* tmp )
    {
        int i=0,j=0;
        for( ; i<ll && j<rl ; ) {

            if( xpoints[li+i] <= xpoints[ri+j] ) {
                tmp[i+j] = xpoints[li+i];
                ++i;
            }

            else {
                tmp[i+j] = xpoints[ri+j];
                ++j;
            }

        }

        if( i == ll ) {
            for( ; j<rl ; ++j ) {
                tmp[i+j] = xpoints[ri+j];
            }
        }

        else {
            for( ; i<ll ; ++i ) {
                tmp[i+j] = xpoints[li+i];
            }
        }
    }


    size_t points_num;
    vector<pair<int,bool>>& xpoints;
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
get_points_from_stdin( vector<pair<int,bool>>& points,
                       size_t n)
{
    int x=0,y=0;

    while( n-- ) {
        cin >> x;
        cin >> y;
        points.push_back( pair<int,bool>( x, false ) );
        points.push_back( pair<int,bool>( y, true ) );
    }

    return true;
}



int main()
{
    size_t size;
    vector<pair<int,bool>> xpoints;

    get_size_from_stdin( &size );
    if( !get_points_from_stdin( xpoints, size ))
        return 0;

    dyn_deque_t plot( xpoints, size );
    plot.sort();
    cout << plot.unique_values_number() << endl;

    return 0;
}
