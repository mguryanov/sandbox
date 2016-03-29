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
    self_used,
    user_hold,
    self_hold
};



typedef struct {
    u_int8_t x;
    u_int8_t y;
    u_int8_t z;
    u_int8_t w;
} vec_t;


static constexpr u_int8_t weight_perimetr = 3 + 2 + 3;
static constexpr u_int8_t weight_througth_center_diagonal = 3 + 4 + 3;
static constexpr u_int8_t weight_througth_center_vertical = 2 + 4 + 2 +
                                                /* center priority */ 1;


static vec_t vecs[8] = {
    {1,2,3,weight_perimetr},
    {1,4,7,weight_perimetr},
    {1,5,9,weight_througth_center_diagonal},
    {2,5,8,weight_througth_center_vertical},
    {3,5,7,weight_througth_center_diagonal},
    {3,6,9,weight_perimetr},
    {4,5,6,weight_througth_center_vertical},
    {7,8,9,weight_perimetr}
};



template<typename T1> struct triplet_s;

template<>
struct triplet_s<u_int8_t> {

    triplet_s ( u_int8_t i ) :
        id(i),
        hit_score(0),
        vec(vecs[i-1])
    {}

    u_int8_t id;
    u_int8_t hit_score;
    vec_t& vec;

};



template<typename T1> struct list_s;
template<typename T1>
struct node_s {

    friend struct list_s<T1>;

    node_s( node_s<T1>* parent,
            node_s<T1>* child )
    :
        prev( parent ),
        next( child )
    {}

    ~node_s() {}

private:
    T1* item;
    node_s<T1>* prev;
    node_s<T1>* next;
};



template<typename T1>
struct list_s {

    list_s( )
    :
        head( nullptr ),
        tail( nullptr ),
        direct( nullptr ),
        dpos( 0 ),
        nelts( 0 )
    {}

    ~list_s()
    {
        node_s<T1>* next=head;

        while ( next ) {
            node_s<T1>* tmp=next->next;
            delete_node( &next );
            next=tmp;
        }
    }


    bool
    sorted_insert( T1* item )
    {
        if ( tail == nullptr ){
            tail=new node_s<T1>( nullptr, nullptr );
            tail->item=item;
            head=tail;
            ++nelts;
            return true;
        }

        node_s<T1>* next=head;

        while( next ) {
            node_s<T1>* tmp=next->next;

            if ( next->item->vec.w >= item->vec.w ) {
                next=tmp;
                continue;
            }

            else {
                tmp=next->prev;
                node_s<T1>* nn=new node_s<T1>( tmp, next );
                nn->item=item;
                next->prev=nn;
                if( next == head ) {
                    head=nn;
                } else {
                    tmp->next=nn;
                }
                ++nelts;
            }

            return true;
        }

        tail->next=new node_s<T1>( tail, nullptr );
        tail->next->item=item;
        tail=tail->next;

        ++nelts;
        return true;
    }


    bool next ()
    {
        if ( head == nullptr || nelts == 0 || direct == tail )
            return false;

        if ( direct == nullptr ) { /* init iterator */
            dpos=0;
            direct=head;
        }

        else {
            ++dpos;
            direct=direct->next;
        }

        return true;
    }


    bool get ( T1** v )
    {
        if( direct == nullptr ) {
            return false;
        }

        *v=direct->item;
        return true;
    }


    void remove ()
    {
        if( direct == nullptr ) {
            return;
        }

        if( direct->next ) {
            node_s<T1>* next=direct->next;
            next->prev=direct->prev;
        }

        if( direct->prev ) {
            node_s<T1>* prev=direct->prev;
            prev->next=direct->next;
        }
    }


    void resect ()
    {
        if( direct == nullptr ) {
            return;
        }

        delete_node( &direct );
    }

private:

    node_s<T1>* head;
    node_s<T1>* tail;
    node_s<T1>* direct;
    unsigned int dpos;
    unsigned int nelts;


    bool delete_node( node_s<T1>** node )
    {
        if ( nelts == 0 ) {
            return false;
        }

        node_s<T1>* next=(*node)->next;

        if ( *node == tail ) {
            tail=(*node)->prev;
            if ( tail ) {
                tail->next=nullptr;
            }
        }

        else if ( *node==head ) {
            head=(*node)->next;
        }

        else {
            (*node)->prev->next=next;
            next->prev=(*node)->prev;
        }

        if ( direct == *node )
        {
            if ( next ) {
                direct=next->prev;
            }

            else {
                direct=tail;
            }
        }

        delete *node;
        *node=next;

        if ( nelts )
            --nelts;

        return true;
    }
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




int main( int argc, char** argv )
{
    if( argc > 1 &&
        ( strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0 )) {
        cout << "Using :\n";
        cout << "\t-h, --help\t: this help manual\n";
        cout << "\t1,2\t\t: mean coordinates of cell place\n\n";
        return 0;
    }

    array<u_int8_t,9> playboard { unused };

    list_s<triplet_s<u_int8_t>> unused_triplets;
    list_s<triplet_s<u_int8_t>> user_triplets;
    list_s<triplet_s<u_int8_t>> self_triplets;

    for( u_int8_t i=1; i<9; ++i ) {
        unused_triplets.sorted_insert( new triplet_s<u_int8_t>( i ));
    }

/* for test purposes
 *
    triplet_s<u_int8_t>* triplet;
    while ( unused_triplets.next() && unused_triplets.get( &triplet )) {
        printf("%d: %d, %d - %d - %d , %d\n",
               triplet->id,
               triplet->hit_score,
               triplet->vec.x,
               triplet->vec.y,
               triplet->vec.z,
               triplet->vec.w);
    }
*/

    for(;;) {

        list_s<triplet_s<u_int8_t>>& current_triplets = unused_triplets;

        cout << "Insert cell position:" << endl;

        u_int cell=0;
        if( !get_data_from_stdin( &cell )) {
            cout << "Please insert correct values!" << endl;
            continue;
        }

        if( playboard[cell] == user_used || playboard[cell] == self_used ) {
            cout << "This cell ["
                 << cell % 3 << "," << cell / 3
                 << "] already used!!!" << endl;
            continue;
        }

        else if( playboard[cell] == user_hold ) {
            current_triplets = user_triplets;
        }

        else if( playboard[cell] == self_hold ) {
            current_triplets = self_triplets;
        }

        u_int8_t user_hit_score=0;
        triplet_s<u_int8_t>* triplet;
        while ( current_triplets.next() && current_triplets.get( &triplet )) {
            printf("%d: %d, %d - %d - %d , %d\n",
                   triplet->id,
                   triplet->hit_score,
                   triplet->vec.x,
                   triplet->vec.y,
                   triplet->vec.z,
                   triplet->vec.w);

            if( cell == triplet->vec.x ||
                cell == triplet->vec.y ||
                cell == triplet->vec.z )
            {
                if( playboard[cell] == user_hold ) {
                    user_hit_score = ++triplet->hit_score;
                }

                if( playboard[cell] == unused ) {
                    user_hit_score = ++triplet->hit_score;
                    current_triplets.remove();
                    user_triplets.sorted_insert( triplet );
                    playboard[triplet->vec.x] = user_hold;
                    playboard[triplet->vec.y] = user_hold;
                    playboard[triplet->vec.z] = user_hold;
                }

                else if( playboard[cell] == self_hold ) {
                    current_triplets.resect();
                }

                playboard[cell] = user_used;
                break;
            }
        }

        if( user_hit_score == 3 ) {
            cout << "You are win!! Сongratulations!!!" << endl;
            break;
        }

        else if( user_hit_score == 2) {
            if( playboard[triplet->vec.x] == user_hold ) {
                playboard[triplet->vec.x] = self_used;
            }

            else if( playboard[triplet->vec.y] == user_hold ) {
                playboard[triplet->vec.y] = self_used;
            }

            else if( playboard[triplet->vec.z] == user_hold ) {
                playboard[triplet->vec.z] = self_used;
            }

            else {
                cout << "Something wrong!!!" << endl;
                break;
            }

            current_triplets.resect();
        }

        else {

        }

    }

    return 0;
}
