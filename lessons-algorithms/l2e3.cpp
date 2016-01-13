#include <cassert>
#include <iostream>
#include <deque>
#include <vector>
#include <memory>

using namespace std;
static const unsigned int num_limit=1000000;

struct dyn_deque_s {
    dyn_deque_s( size_t size,
                 size_t bound )
        :
        buffer_size ( size ),
        value_bound ( bound )
    {}

    ~dyn_deque_s() {}

    bool is_empty() const {
        return buffer.empty();
    }

    void push_front( int v ) {
        buffer.push_front( v );
    }


    void push_back( int v ) {
        buffer.push_back( v );
    }


    int pop_front( ) {
        int tmp=buffer.at( 0 );
        buffer.pop_front();
        return tmp;
    }


    int pop_back( ) {
        int tmp=buffer.at( buffer.size() - 1 );
        buffer.pop_back();
        return tmp;
    }


    void build() {
        for ( ssize_t i=( buffer.size() / 2 ) - 1; i>=0; --i ) {
            _sift_down( i );
        }
    }



    void next_bound()
    {
        assert( !buffer.empty() );

        vector<int> vtmp;
        ssize_t bound=value_bound;
        size_t size=buffer.size();

        for( bound-=buffer[0]; bound >= 0 && size;
             bound-=buffer[0] )
        {
            int max=_get_maximum( --size );
            if( max != 1 ) {
                vtmp.push_back( max / 2 );
            }
        }

        for ( auto v : vtmp ) {
            buffer.push_back( v );
        }

        build();
    }


    void dump_to_cout() {
        cout << buffer_size << " : ";
        for( auto v : buffer ) {
            cout << v << " ";
        }
        cout << ": " << value_bound;
    }



private:

    int _get_maximum( size_t last )
    {
        assert( !buffer.empty() );
        int tmp=buffer[0];
        buffer[0]=buffer[last];
        buffer.pop_back();
        if ( last ) {
            _sift_down( 0, last-1 );
        }
        return tmp;
    }

    void _sift_down( size_t from, size_t to )
    {
        size_t left=from*2+1;
        size_t right=from*2+2;
        size_t largest=from;

        if ( left <= to && buffer[from] < buffer[left]) {
            largest=left;
        }

        if ( right <= to && buffer[largest] < buffer[right]) {
            largest=right;
        }

        if ( largest != from ) {
            swap( buffer[largest], buffer[from] );
            _sift_down( largest );
        }
    }

    void _sift_down( size_t from )
    {
        _sift_down( from, buffer.size()-1 );
    }

    size_t buffer_size;
    size_t value_bound;
    deque<int> buffer;
};


typedef struct dyn_deque_s dyn_deque_t;

bool
get_number_from_stdin( size_t* s )
{
    int n=0;
    char c;

    while(cin.get(c)) {
        if (c == '\n' ) {
            *s = n;
            break;
        }

        if ( n == 0 && (c == '-' || c == ' ' )) {
            continue;
        }

        n*=10;
        n+=(c-'0');

        if ( n > num_limit ) {
            return false;
        }
    }

    if (cin.eof())
        *s = n;

    return true;
}



bool
get_basket_from_stdin(
        unique_ptr<dyn_deque_t>& bwa )
{
    int n=0;
    int init=0;
    size_t vnelts=0;
    size_t vbound=0;

    if ( !get_number_from_stdin( &vnelts ))
         return false;

    string line;
    getline( cin, line );

    if ( cin.fail() ) {
        return false;
    }

    string illegal_characters( "-\\,|<>:#$%{}()[]\'\"^!?+*" );
    size_t pos = line.find_first_of( illegal_characters, 0 );
    if( pos!=string::npos ) {
        return false;
    }

    if ( !get_number_from_stdin( &vbound ))
         return false;

    bwa.reset(
        new dyn_deque_t( vnelts, vbound )
    );

    string::iterator it=line.begin();

    for ( ; it!=line.end(); ++it ) {

        if ( *it == ' ' && !init )
            continue;

        if ( *it == ' ' ) {
            bwa->push_back( n );
            n=0;
            init=0;
            continue;
        }

        init=1;
        n*=10;
        n+=(*it-'0');

        if ( n > num_limit ) {
            return false;
        }
    }

    if ( init ) {
        bwa->push_back( n );
    }

    bwa->build();
    return true;
}



int main()
{
    unique_ptr<dyn_deque_t> bascket_with_apples;

    if ( !get_basket_from_stdin( bascket_with_apples ))
        return 0;

    size_t count=0;

    while( !bascket_with_apples->is_empty()) {
//        bascket_with_apples->dump_to_cout();
        bascket_with_apples->next_bound();
        ++count;
    }

    cout << count;
    return 0;
}
