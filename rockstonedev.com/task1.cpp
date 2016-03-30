#include <cstring>
#include <list>
#include <array>
#include <iostream>



using namespace std;



enum {
    unused = 0x0,
    user_used,
    self_used
};


static char tic_tac[3] = { ' ', 'U', 'C' };


static const u_int8_t weight_center = 4;
static const u_int8_t weight_corner = 3;
static const u_int8_t weight_cross = 2;


typedef struct {
    u_int8_t id;
    u_int8_t w; /* init weight */
    u_int8_t triplets_number;
    u_int8_t triplets[8];
} static_cell_data_t;


static const static_cell_data_t cells[9] = {
    {0,weight_corner,3,{1,2,3,6,4,8}},
    {1,weight_cross,2,{0,2,4,7}},
    {2,weight_corner,3,{0,1,5,8,4,6}},
    {3,weight_cross,2,{0,6,4,5}},
    {4,weight_center,4,{0,8,1,7,2,6,3,5}},
    {5,weight_cross,2,{2,8,3,4}},
    {6,weight_corner,3,{0,3,2,4,7,8}},
    {7,weight_cross,2,{1,4,6,8}},
    {8,weight_corner,3,{0,4,2,5,6,7}}
};


typedef struct volatile_cell_s volatile_cell_t;
typedef pair<u_int8_t,volatile_cell_t*> playborad_cell_t;
typedef array<playborad_cell_t,9> volatile_playborad_t;
typedef struct playboard_s playboard_t;

static bool reverse_compare_by_weight ( const volatile_cell_t*, const volatile_cell_t*);
static void clear();




struct volatile_cell_s
{
    volatile_cell_s ( u_int8_t i )
        :
          _cell( cells[i] )
    {}

    u_int8_t get_cell_id() const {
        return _cell.id;
    }

    u_int8_t weight() const {
        return _cell.w;
    }

    const u_int8_t* triplets() const {
        return _cell.triplets;
    }

    u_int8_t triplets_number() const {
        return _cell.triplets_number;
    }

private:
    const static_cell_data_t& _cell;
};




struct playboard_s
{
    playboard_s () :
        _playboard{ playborad_cell_t( unused, nullptr ) }
    {}



    ~playboard_s ()
    {
        for( auto v : _playboard ) {
            if( v.second )
                delete v.second;
        }
    }


    bool is_filled() const {
        return _active_cells.empty();
    }



    u_int8_t cell_holder( const u_int8_t id ) const {
        return _playboard[id].first;
    }



    u_int8_t counter_strike( const u_int8_t id ) {
        volatile_cell_t* user_hit_cell  = _playboard[id].second;
        const u_int8_t* triplets        = user_hit_cell->triplets();
        const u_int8_t  triplets_number = user_hit_cell->triplets_number();

        u_int8_t cell=0;
        for( u_int8_t i=0; i<triplets_number*2; )
        {
            if( _playboard[triplets[i]].first == self_used ||
                _playboard[triplets[i+1]].first == self_used ) {
                i+=2; continue;
            }

            if( _playboard[triplets[i]].first == user_used ) {
                cell=triplets[i+1];
                break;
            }

            ++i;

            if( _playboard[triplets[i]].first == user_used ) {
                cell=triplets[i-1];
                break;
            }

            ++i;
        }

        if( !cell ) {
            cell = _active_cells.front()->get_cell_id();
            _active_cells.pop_front();
        }

        else {
            _active_cells.remove( _playboard[cell].second );
        }

        _playboard[cell].first = self_used;
        return cell;
    }



    void cell_append( const u_int8_t id ) {
        _playboard[id].second = new volatile_cell_t( id );
        _active_cells.push_back( _playboard[id].second );
    }



    void cell_sort( ) {
        _active_cells.sort( reverse_compare_by_weight );
    }



    void cell_detach( const u_int8_t id, const u_int8_t holder ) {
        _playboard[id].first = holder;
        _active_cells.remove( _playboard[id].second );
    }



    bool winner_check( const u_int8_t id ) const {
        const u_int8_t  holder          = _playboard[id].first;
        const u_int8_t* triplets        = _playboard[id].second->triplets();
        const u_int8_t  triplets_number = _playboard[id].second->triplets_number();

        for( u_int8_t i=0; i<triplets_number*2; )
        {
            if( _playboard[triplets[i]].first    != holder ||
                _playboard[triplets[i+1]].first  != holder ) {
                i+=2; continue;
            }

            return true;
        }

        return false;
    }



    void render( ) const {

        clear();

        uint index=0;
        for( uint r=0; r<5; ++r ) {

            if( r % 2 ) {
                for( uint c=0; c<5; ++c ) {
                    if( c % 2 )
                        cout << "\u256C";
                    else
                        cout << "\u2550";
                }

                cout << endl;
                continue;
            }

            for( uint c=0; c<5; ++c ) {
                if( c % 2 ) {
                    cout << "\u2551";
                }

                else {
                     cout << tic_tac[_playboard[index].first];
                    ++index;
                }
            }

            cout << endl;
        }
    }

private:
     volatile_playborad_t   _playboard;
     list<volatile_cell_t*> _active_cells;

};



static bool
reverse_compare_by_weight ( const volatile_cell_t* first,
                            const volatile_cell_t* second)
{
    return ( first->weight() >= second->weight() );
}



static void
clear()
{
    cout << "\x1B[2J\x1B[H";
}



static bool
get_data_from_stdin( u_int* cell )
{
    char x,y,comma;

    cin >> x;
    cin >> comma;
    cin >> y;

    cin.clear();
    fflush(stdin);

    if( !isdigit(x) || !isdigit(y) || comma != ',' )
        return false;

    u_int dx = ( x - '0' );
    u_int dy = ( y - '0' );

    if( dx > 2 || dy > 2 )
        return false;

    *cell = (dx + dy * 3);
    return true;

}



int main( int argc, char** argv )
{
    if( argc > 1 &&
        ( strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0 )) {
        cout << "Using :\n";
        cout << "\t-h, --help\t: this help manual\n";
        cout << "\t1,2\t\t: mean coordinates of cell place\n\n";
        return 0;
    }

    playboard_t playboard;

    try {

        for( u_int8_t i=0; i<9; ++i )
            playboard.cell_append( i );

        playboard.cell_sort();
    }

    catch(...) {
        cout << "Internal error!!!" << endl;
        return 1;
    }

    const char* msg=nullptr;

    for(;;) {

        playboard.render();
        cout << "Insert cell position:" << endl;

        u_int cell=0;
        if( !::get_data_from_stdin( &cell ))
            continue;

        u_int8_t holder = playboard.cell_holder( cell );
        if( holder == user_used || holder == self_used )
            continue;

        playboard.cell_detach( cell, user_used );

        if( playboard.winner_check( cell )) {
            msg = "You are winner!!! Congratulation!";
            break;
        }

        /* ai */
        if( playboard.is_filled() ) {
            msg =  "No winer now!!! Have a luck next time!";
            break;
        }

        u_int8_t strike = playboard.counter_strike( cell );

        if( playboard.winner_check( strike )) {
            msg = "You are looser!!!";
            break;
        }
    }

    if( msg ) {
        playboard.render();
        cout << msg << endl;
    }

    return 0;
}
