#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

template<typename T1> struct list_s;


typedef struct meta_s {
    const char* word;
    unsigned int matched;

    meta_s () :
        word( nullptr ),
        matched( 0 )
    {}

    ~meta_s ()
    {
        if( word )
            delete[] word;
    }

} meta_t;


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
    T1 value;
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
    insert_from_begin( const char* word )
    {
        if ( tail == nullptr ){
            tail=new node_s<T1>( nullptr, nullptr );
            tail->value.word=word;
            tail->value.matched=1;
            head=tail;
            ++nelts;
            return true;
        }

        node_s<T1>* next=head;

        while( next ) {
            node_s<T1>* tmp=next->next;

            int result=strcmp( next->value.word, word);

            if ( result < 0 ) {
                next=tmp;
                continue;
            }

            if( result == 0 ) {
                ++(next->value.matched);
                delete[] word;
            }

            else if( result > 0 ) {
                tmp=next->prev;
                node_s<T1>* nn=new node_s<T1>( tmp, next );
                nn->value.word=word;
                nn->value.matched=1;
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
        tail->next->value.word=word;
        tail->next->value.matched=1;
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

        *v=&direct->value;
        return true;
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
get_unique_sorted_words_list_from_file( list_s<meta_t>& list )
{
    FILE *fd=stdin;
    if( fd == nullptr ) {
        return false;
    }

    char c;
    char* word=nullptr;

    unsigned int length=0;
    unsigned int limit=30;

    while(( c=fgetc( fd )) != EOF ) {

        if ( c > 'z' || c < 'a' )
        {
            if( !word )
                continue;

            if( !list.insert_from_begin( word ))
                goto failed;

            length=0;
            word=nullptr;
            continue;
        }

        if ( ++length > limit )
            goto failed;

        if( word == nullptr )
            /*
             * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4296.pdf
             *
             * zero-initialization (as varinat of value-initialization)
             * according
             *
             * ISO/IEC 14882:2014
             *  5.3.4 (17.2) new-expression
             *  8.5 (6.4) zero-initialize
             */
            word=new char[31]();

        word[length-1]=c;
    }

    if( length ) {
        if( !list.insert_from_begin( word ))
            goto failed;
    }

    fclose( fd );
    return true;

failed:
    fclose( fd );
    delete[] word;
    return false;
}



int main()
{
    list_s<meta_t> list;

    if ( !get_unique_sorted_words_list_from_file( list ) )
        exit( 0 );

    meta_t* curr;
    while ( list.next() && list.get( &curr )) {
        printf("%s - %d\n", curr->word, curr->matched );
    }

    return 0;
}
