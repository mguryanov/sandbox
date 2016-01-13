#include <cassert>
#include <iostream>
#include <deque>
#include <vector>
#include <memory>

using namespace std;
static const unsigned int num_limit=1000000;

bool
get_vector_from_stdin( vector<int>& v )
{
    int n=0;
    int init=0;
    int sign=1;

    string line;
    getline( cin, line );

    if ( cin.fail() ) {
        return false;
    }

    string illegal_characters( "\\,|<>:#$%{}()[]\'\"^!?+*" );
    size_t pos = line.find_first_of(illegal_characters,0);
    if( pos!=string::npos ) {
        return false;
    }

    string::iterator it=line.begin();

    for ( ; it!=line.end(); ++it ) {

        if ( *it == ' ' && !init )
            continue;

        if ( *it == ' ' ) {
            v.push_back( n*sign );
            n=0;
            init=0;
            sign=1;
            continue;
        }

        if ( *it == '-' && !init ) {
            sign=-1;
            continue;
        }

        init=1;
        n*=10;
        n+=(*it-'0');

        if ( n > 100000 ) {
            return false;
        }
    }

    if ( init ) {
        v.push_back( n*sign );
    }

    return true;
}



int main()
{
    vector<int> data;

    if ( !get_vector_from_stdin( data ))
        return 0;

    /* inserted sort */
    for( size_t i=1; i < data.size(); ++i ) {
        int tmp=data[i];
        size_t j=i;
        for( ; j>0 && data[j-1]>tmp; --j ) {
            data[j]=data[j-1];
        }
        data[j]=tmp;
    }

    for( size_t i=0; i < data.size(); ) {
        cout << data[i];
        if( ++i < data.size())
            cout << " ";
    }

    return 0;
}
