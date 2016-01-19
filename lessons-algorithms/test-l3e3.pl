#! /usr/bin/perl -w

use 5.018;

use strict;
use utf8;
no  warnings qw/utf8/;
use experimental qw/smartmatch/;

use Data::Dumper;
use IPC::Open2;
use Term::ANSIColor qw(:constants);

use constant REQS_LIMIT => 10;

my ($fhread, $fhwrite, $pid, $result);

# main
{
    for ( my $i=0; $i<REQS_LIMIT; ++$i ) {
        my $test=generate_test();
        say qq{ test :\n}.$test.qq{\n};
        say qq{ result :\n}.Dumper($result).qq{\n};
#        post_test( $test );
#        check_test( $test );
#        finalize ();
    }
}



sub generate_test {
    my $segment_number=int(rand(10**2));
    my $segments=[];

    for (my $i=0;$i<($segment_number);++$i) {
        my $init_point=int( rand( 10**2 ));
        my $segment_length=int( rand( 10**2 ))+1;
        push @$segments,[$init_point,$segment_length];
    }

    #$point_number=19;
    #$list = [[2,36],[84,46],[4,63],[42,15],[31,43],[20,57],[21,68],
    #         [25,14],[24,89],[9,52],[33,62],[82,43],[32,54],[78,21],[67,0],
    #         [81,82],[31,22],[59,76],[92,10]];

    my $req_str=$segment_number.qq{\n}.
                join( qq{\n}, map { $_->[0].q{ }.$_->[1] } @$segments );

    my $tmp=[ sort { $a->[0] <=> $b->[0] }
              sort { $a->[1] <=> $b->[1] } @$list ];

    for( my $i1=0, $i2=1; $i2<$segment_number-1; ++$i2 )
    {
        # summ all length
        $result+=$segments->[$i1]->[1];
        my $y=( $segments->[$i1]->[0] + $segments->[$i1]->[1] );
        # greater and equal guaranteed by sort => not check
        if( $segments->[$i1]->[0] < $segments->[$i2]->[0] ) {
            if( $y =< $segments->[$i2]->[0] ) {
                ++$i1;
                continue;
            }
            my $white=$segments->[i+1]->[0] - $segments->[i]->[0];
            my $next_y=$segments->[i+1]->[0] + $segments->[i+1]->[1];
            if( $y < $next_y ) {
                $result-=$y-$segments->[i+1]->[0];
            } else {
                $result-=$segments->[i+1]->[1];
            }
        }

        $result-=$segments->[$i1]->[1];
        $segments->[$i2]->[0]=$y;
        $segments->[$i2]->[1]-=$y-$segments->[$i2]->[0];
        ++$i1;

        ++$lli if( $segments->[$lli]->[1] =< 0 );
    }

    return $req_str;
}



sub post_test {
    my($test,$fh)=@_;
    $pid=open2(
        $fhread, $fhwrite,
        q{/home/maxim/app/my/blogspot-examples/sandbox-build/algorithms/l3e2}
    );
    syswrite $fhwrite,$test,length($test);
    close $fhwrite;
}


sub check_test {
    my($test)=@_;
    my $r=[];
    while( <$fhread> ) {
        push @$r,[ split( q{ }, $_ ) ];
    }
    close $fhread;
    unless (defined $r) {
        say RED qq{failed : \n}.
                q{ must be : }.
                join( qq{\n}, map { $_->[0].q{ }.$_->[1] } @$result ).qq{\n}.
                q{ test : }.$test;
        return;
    }

    # smart match using ~~
    unless ( $r ~~ $result ) {
        say RED qq{failed response : \n}.
                join( qq{\n}, map { $_->[0].q{ }.$_->[1] } @$r ).qq{\n}.
                q{ must be : }.
                join( qq{\n}, map { $_->[0].q{ }.$_->[1] } @$result ).qq{\n}.
                q{ test : }.$test;
        return;
    }

    say GREEN qq{test pass\n};
}


sub finalize {
    waitpid($pid,0);
}
