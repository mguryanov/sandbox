#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <list>
#include <array>
#include <memory>
#include <iostream>

using namespace std;


enum {
    unused = 0x0,
    user_used,
    self_used
};


static char tic_tac[3] = { ' ', 'U', 'C' };


static constexpr u_int8_t weight_center = 4;
static const u_int8_t weight_corner = 3;
static const u_int8_t weight_cross = 2;


typedef struct {
    u_int8_t id;
    u_int8_t w;     /* init weight  : chance to win */
    u_int8_t ls;    /* life score   : law of energy conservation */
    u_int8_t sr[8]; /* successors */
} static_cell_data_t;


static constexpr static_cell_data_t cells[9] = {
    {0,weight_corner,6,{1,2,4,6,3,8}},
    {1,weight_cross,4,{2,0,4,7}},
    {2,weight_corner,6,{5,8,4,6,1,0}},
    {3,weight_cross,4,{0,6,4,5}},
    {4,weight_center,0,{0}},
    {5,weight_cross,4,{8,2,4,3}},
    {6,weight_corner,6,{3,0,4,2,7,8}},
    {7,weight_cross,4,{6,8,4,1}},
    {8,weight_corner,6,{7,6,4,0,5,2}}
};


template<typename T1> struct volatile_cell_s;

template<>
struct volatile_cell_s<u_int8_t> {

    volatile_cell_s ( u_int8_t i )
        :
            life_score( cells[i].w ),
            cell( cells[i] )
    {}

    u_int8_t life_score;
    const static_cell_data_t& cell;

};



bool
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



static bool
reverse_compare_by_weight (volatile_cell_s<u_int8_t>* first,
                           volatile_cell_s<u_int8_t>* second)
{
    return ( first->cell.w >= second->cell.w );
}



static void
clear() {
    cout << "\x1B[2J\x1B[H";
}



typedef pair<u_int8_t,volatile_cell_s<u_int8_t>*> playborad_cell_t;

static void
render_playboard ( array<playborad_cell_t,9>& playboard )
{
    clear ();

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
                 cout << char(tic_tac[playboard[index].first]);
                ++index;
            }
        }
        cout << endl;
    }
}


static bool
winner_check ( u_int8_t id,
               array<playborad_cell_t,9>& playboard )
{
    /* hardcore */
    for( uint i=0,score_x=0; i < 9; ++i ) {

        if( i % 3 == 0 )
            score_x=0;

        if( playboard[i].first != playboard[id].first ) {
            continue;
        }

        if( ++score_x == 3)
            return true;
    }

    for( uint i=0,y=0,z=0,score_y=0; i < 9; ++i, ++y ) {

        if( i % 3 == 0 ){
            score_y=0;
            y=0;
            ++z;
        }

        if( playboard[3*y+z].first != playboard[id].first ) {
            continue;
        }

        if( ++score_y == 3)
            return true;
    }

    for( uint i=0,x=0,y=0,score=0; i < 3; ++i, ++x, ++y ) {

        if( playboard[3*x+y].first != playboard[id].first ) {
            continue;
        }

        if( ++score == 3)
            return true;
    }

    for( uint i=0,x=0,score=0; i < 3; ++i, ++x ) {

        if( playboard[2*x+2].first != playboard[id].first ) {
            continue;
        }

        if( ++score == 3)
            return true;
    }

    return false;
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

    array<playborad_cell_t,9> playboard {
        playborad_cell_t(unused,nullptr)
    };

    list<volatile_cell_s<u_int8_t>*> active_cells;

    try {
        for( u_int8_t i=0; i<9; ++i ) {
            playboard[i].second = new volatile_cell_s<u_int8_t>( i );
            active_cells.push_back( playboard[i].second );
        }
    }

    catch(...) {
        cout << "Internal error!!!" << endl;
    }

    const char* msg=nullptr;

    for(;;) {

        render_playboard( playboard );

        active_cells.sort( reverse_compare_by_weight );
        cout << "Insert cell position:" << endl;

        u_int cell=0;
        if( !get_data_from_stdin( &cell )) {
            cout << "Please insert correct values!" << endl;
            continue;
        }

        if( playboard[cell].first == user_used ||
            playboard[cell].first == self_used )
        {
            cout << "This cell ["
                 << cell % 3 << "," << cell / 3
                 << "] already used!!!" << endl;
            continue;
        }

        playboard[cell].first = user_used;
        volatile_cell_s<u_int8_t>* user_hit_cell = playboard[cell].second;

        for( u_int8_t i=0; i<user_hit_cell->cell.ls; ++i ) {
            ++playboard[user_hit_cell->cell.sr[i]].second->life_score;
        }

        active_cells.remove( user_hit_cell );

        if( winner_check( cell, playboard )) {
            msg = "You are winner!!! Congratulation!";
            break;
        }

        active_cells.sort( reverse_compare_by_weight );

        /* ai */
        if( active_cells.empty()) {
            msg =  "No winer now!!! Have a luck next time!";
            break;
        }

        volatile_cell_s<u_int8_t>* ai_hit_cell = active_cells.front();
        active_cells.pop_front();

        playboard[ai_hit_cell->cell.id].first = self_used;

        if( winner_check( ai_hit_cell->cell.id, playboard )) {
            msg = "AI are winner!!!";
            break;
        }
    }

    if( msg ) {
        render_playboard( playboard );
        cout << msg << endl;
    }

    for( auto v : playboard ) {
        if( v.second )
            delete v.second;
    }

    return 0;
}
