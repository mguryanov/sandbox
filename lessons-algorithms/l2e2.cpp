#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

using namespace std;

static const unsigned int size_limit=1000000;
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
        reverse( nullptr ),
        dpos( 0 ),
        rpos( 0 ),
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

    bool is_empty() const
    {
        return nelts==0;
    }


    void push_back( T1 v )
    {
        if ( tail ) {
            tail->next=new node_s<T1>( tail, nullptr );
            assert( tail->next );
            tail->next->value=v;
            tail=tail->next;
        }

        else {
            tail=new node_s<T1>( nullptr, nullptr );
            assert( tail );
            tail->value=v;
            head=tail;
        }

        ++nelts;
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


    bool reverse_next ()
    {
        if ( tail == nullptr || nelts == 0 || reverse == head )
            return false;

        if ( reverse == nullptr ) { /* init iterator */
            rpos=nelts-1;
            reverse=tail;
        }

        else {
            --rpos;
            reverse=reverse->prev;
        }

        return true;
    }

    unsigned int get_pos ()
    {
        return dpos;
    }


    unsigned int get_revers_pos ()
    {
        return rpos;
    }


    const T1 get ( )
    {
        assert( direct );
        return direct->value;
    }


    const T1 reverse_get ( )
    {
        assert( reverse );
        return reverse->value;
    }


    bool get ( T1* v )
    {
        if( direct == nullptr ) {
            return false;
        }

        *v=direct->value;
        return true;
    }


    bool reverse_get ( T1* v )
    {
        if( reverse == nullptr ) {
            return false;
        }

        *v=reverse->value;
        return true;
    }


    void iter_reset ()
    {
        dpos=0;
        direct=nullptr;
    }


    void reverse_iter_reset ()
    {
        rpos=(nelts) ? nelts-1 : 0;
        reverse=nullptr;
    }


    bool delete_node_slice( unsigned int from, unsigned int len )
    {
        if ( from > nelts ) /* from [0..nelts-1] */
            return false;

        int index=0;
        node_s<T1>* curr=head;

        while ( index != from && curr ) {
            curr=curr->next;
            ++index;
        }

        bool state=true;

        while ( state && len && curr )
        {
            state=delete_node( &curr );
            --len;

            if ( dpos >= from ) {
                --dpos;
            }

            if ( rpos > from ) {
                --rpos;
            }
        }

        return state;
    }


private:

    node_s<T1>* head;
    node_s<T1>* tail;
    node_s<T1>* direct;
    node_s<T1>* reverse;
    unsigned int dpos;
    unsigned int rpos;
    unsigned int nelts;


    bool delete_node( node_s<T1>** node )
    {
        assert ( *node );

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

        if ( reverse == *node ) {
            reverse=next;
        }

        delete *node;
        *node=next;

        if ( nelts )
            --nelts;

        return true;
    }
};



bool
get_brackets_from_stdin( list_s<char>& list )
{
    string line;
    getline (cin,line);

    if ( cin.fail() ) {
        return false;
    }

    if ( line.size() > size_limit ) {
        return false;
    }

    string illegal_characters(" -\\,|<>:#$%\'\"^!?+*");
    size_t pos = line.find_first_of(illegal_characters,0);
    if( pos!=string::npos ) {
        return false;
    }

    string::iterator it=line.begin();

    for ( ; it!=line.end(); ++it ) {
        list.push_back( *it );
    }

    return true;
}



int main()
{
    list_s<char> brackets_list;

    if ( !get_brackets_from_stdin( brackets_list ) )
        return 0;

    string result;
    while ( brackets_list.next() ) {
        result.push_back( brackets_list.get() );
    }

    brackets_list.iter_reset ();

    char prev, curr;


    while ( brackets_list.next() )
    {
        prev=curr;
        curr=brackets_list.get();

        if ( ( prev == '(' && ( curr == '}' || curr == ']' )) ||
             ( prev == '{' && ( curr == ')' || curr == ']' )) ||
             ( prev == '[' && ( curr == '}' || curr == ')' )) )
        {
            cout << "IMPOSSIBLE";
            return 0;
        }

        if (( prev == '(' && curr == ')') ||
            ( prev == '{' && curr == '}') ||
            ( prev == '[' && curr == ']'))
        {
            unsigned int ii=brackets_list.get_pos()-1;
            brackets_list.delete_node_slice( ii, 2 );
            curr=brackets_list.get();
        }
    }

    brackets_list.iter_reset ();

    while ( brackets_list.next() && brackets_list.get( &curr ))
    {
        switch ( curr ) {
        case ')' :
            result.insert (result.begin(),'(');
            break;
        case '}' :
            result.insert (result.begin(),'{');
            break;
        case ']' :
            result.insert (result.begin(),'[');
            break;
        }
    }

    while ( brackets_list.reverse_next() && brackets_list.reverse_get( &curr ))
    {
        switch ( curr ) {
        case '(' :
            result.push_back( ')' );
            break;
        case '{' :
            result.push_back( '}' );
            break;
        case '[' :
            result.push_back( ']' );
            break;
        }
    }

    cout << result.c_str();
    return 0;
}

