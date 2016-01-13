#! /usr/bin/perl -w

use 5.018;

use strict;
use utf8;
no  warnings qw/utf8/;
use experimental qw/smartmatch/;

use Data::Dumper;
use IPC::Open2;
use Term::ANSIColor qw(:constants);

use constant REQS_LIMIT => 1000;
use constant LEFT_PARENTH => 1;     # (
use constant RIGHT_PARENTH => 2;    # )
use constant LEFT_BRACES => 3;      # {
use constant RIGHT_BRACES => 4;     # }
use constant LEFT_BRACKETS => 5;    # [
use constant RIGHT_BRACKETS => 6;   # ]

my ($fhread, $fhwrite, $pid, $result);

# main
{
    for ( my $i=0; $i<REQS_LIMIT; ++$i ) {
        my $test=generate_test();
#       if ($result!~/^IMPOSSIBLE$/) {
#           say GREEN $result;
#           say $test;
#       }
        post_test( $test );
        check_test( $test );
        finalize ();
    }
}



sub generate_test {
    my $elts_number=int(rand(10**2))+1;
    my $list=[];

    for (my $i=0;$i<($elts_number);++$i) {
        my $brace_code=int( rand( 10 ) )%6+1;

        given ( $brace_code ) {
            when ( LEFT_PARENTH )   { push @$list, q{(}; }
            when ( RIGHT_PARENTH )  { push @$list, q{)}; }
            when ( LEFT_BRACES )    { push @$list, qq{\{}; }
            when ( RIGHT_BRACES )   { push @$list, q{\}}; }
            when ( LEFT_BRACKETS )  { push @$list, q{[}; }
            when ( RIGHT_BRACKETS ) { push @$list, q{]}; }
            default {}
        }
    }

#    @$list=qw/ ) { { ( ) /;

    my ( $prev, $curr ) = ( q{}, q{} );
    my $list_size=scalar( @$list );

    $result=join(q{},map { given ($_) {


        } } @$list);

    foreach my $v ( @$list ) {
        given ( $v ) {
            when ( q{(} ) { $result.=q{\050}; }
            when ( q{)} ) { $result.=q{\051}; }
            when ( q{[} ) { $result.=q{\133}; }
            when ( q{]} ) { $result.=q{\135}; }
            when ( '{' )  { $result.=q{\173}; }
            when ( '}' )  { $result.=q{\175}; }
        };
    }

    my $req_str=join(q{},@$list).qq{\n};

    for ( my $i=0; $i<$list_size; ++$i ) {
        $prev=$curr;
        $curr=$list->[$i];
        my $pair=$prev.$curr;
        if ( $pair eq q{(\}} ||
             $pair eq q{(]}  ||
             $pair eq qq{\{)}||
             $pair eq qq{\{]}||
             $pair eq qq{[)} ||
             $pair eq qq{[\}})
        {
            $result=q{IMPOSSIBLE};
            return $req_str;
        }

        if ( $pair eq q{()} ||
             $pair eq q{[]}  ||
             $pair eq qq{\{\}})
        {
            $i-=2;
            $i=( $i >= 0 ) ? ++$i : 0;
            splice( @$list, $i, 2 );
            --$i if ( $i > 0 );
            $list_size=scalar( @$list );
            $i=( $i >= $list_size ) ? $list_size - 1 : $i;
            $curr=$list->[$i];
        }

#        say Dumper($list);
    }

    foreach my $v ( @$list ) {
        given ( $v ) {
            when ( q{)} )   { $result=q{\050}.$result; }
            when ( q{\}} )  { $result=q{\173}.$result; }
            when ( q{]} )   { $result=q{\133}.$result; }
            default {}
        }
    }

    foreach my $v ( reverse @$list ) {
        given ( $v ) {
            when ( q{(} )   { $result.=q{\051}; }
            when ( qq{\{} ) { $result.=q{\175}; }
            when ( q{[} )   { $result.=q{\135}; }
            default {}
        }
    }

    return $req_str;
}



sub post_test {
    my($test,$fh)=@_;
    $pid=open2($fhread, $fhwrite,'/home/maxim/app/my/blogspot-examples/sandbox-build/l2e2');
    syswrite $fhwrite,$test,length($test);
    close $fhwrite;
}


sub check_test {
    my($test)=@_;
    my $r=<$fhread>;
    close $fhread;
    unless (defined $r) {
        say RED qq{failed : \n}.
                q{ must be : }.$result.qq{\n}.
                q{ test : }.$test   ;
        return;
    }

    unless ( $r =~ /^$result$/ ) {
        say RED qq{failed response : \n}.$r.qq{\n}.
                q{ must be : }.$result.qq{\n}.
                q{ test : }.$test   ;
        return;
    }

    say GREEN qq{test pass\n};
}


sub finalize {
    waitpid($pid,0);
}
