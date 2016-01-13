#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

using namespace std;

typedef enum {  PUSH_FRONT = 1,
                POP_FRONT = 2,
                PUSH_BACK = 3,
                POP_BACK = 4
     } opcode_t;

static const unsigned int num_limit=1000000;

struct dyn_deque_s {
    dyn_deque_s( size_t size )
        :
        buffer_size ( size+1 ),
        buffer ( unique_ptr<int[]>( new int[buffer_size] ) ),
        head ( 0 ),
        tail ( 0 )
    {}

    ~dyn_deque_s() {}

    bool is_empty() const { return head==tail; }

    void push_front( int v ) {
        assert( (head+buffer_size-1 ) % buffer_size != tail );
        head=( head+buffer_size-1 ) % buffer_size;
        buffer[head]=v;
    }


    void push_back( int v ) {
        assert( ( tail+1 ) % buffer_size != head );
        buffer[tail]=v;
        tail=( tail+1 ) % buffer_size;
    }


    int pop_front( ) {
        if ( head==tail ) {
            return -1;
        }
        int result=buffer[head];
        head=( head+1 ) % buffer_size;
        return result;
    }


    int pop_back( ) {
        if ( head==tail ) {
            return -1;
        }
        tail=( tail+buffer_size-1 ) % buffer_size;
        return buffer[tail];
    }


private:
    size_t buffer_size;
    unique_ptr<int[]> buffer;
    int real_size;
    int head;
    int tail;
};


typedef struct dyn_deque_s dyn_deque_t;

bool
get_size_from_stdin( size_t* s )
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
get_action_queue_from_stdin(
        vector<pair<opcode_t,int>>& action_queue )
{
    int n=0;
    int opcode=-1;
    int init=0;
    int sign=1;

    string line;
    getline (cin,line);

    if ( cin.fail() ) {
        return false;
    }

    string illegal_characters("\\,|<>:#$%{}()[]\'\"^!?+*");
    size_t pos = line.find_first_of(illegal_characters,0);
    if( pos!=string::npos ) {
        return false;
    }

    string::iterator it=line.begin();

    for ( ; it!=line.end(); ++it ) {

        if ( *it == ' ' && !init )
            continue;

        if ( *it == ' ' ) {
            opcode=( n*sign );
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

        if ( n > num_limit ) {
            return false;
        }
    }

    if ( init ) {
        action_queue.push_back(
                    pair<opcode_t,int>(
                        static_cast<opcode_t>( opcode ), n*sign
                    )
                );
    }

    return true;
}



int main()
{
    size_t deque_size;
    vector<pair<opcode_t,int>> action_queue;

    if ( !get_size_from_stdin( &deque_size ) || deque_size==0 )
        return 0;

    for ( int i=0; i < deque_size; ++i )
        if ( !get_action_queue_from_stdin( action_queue ) ) {
        return 0;
    }

    dyn_deque_t ddeque( deque_size );

    for ( auto& pair : action_queue ) {

        int value=-1;

        switch ( pair.first ) {
        case PUSH_FRONT :
            ddeque.push_front( pair.second );
            break;
        case POP_FRONT :
            value=ddeque.pop_front( );
            break;
        case PUSH_BACK :
            ddeque.push_back( pair.second );
            break;
        case POP_BACK :
            value=ddeque.pop_back();
        default:
            break;
        }

        if ( !( pair.first%2 ) && value != pair.second ) {
            cout << "NO" << endl;
            return 0;
        }

    }

    cout << "YES" << endl;

    return 0;
}
