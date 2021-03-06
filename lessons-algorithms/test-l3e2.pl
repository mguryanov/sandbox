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
#        say qq{ test :\n}.$test.qq{\n};
#        say qq{ result :\n}.Dumper($result).qq{\n};
        post_test( $test );
        check_test( $test );
        finalize ();
    }
}



sub generate_test {
    my $point_number=int(rand(10**2))+1;
    my $list=[];

    for (my $i=0;$i<($point_number);++$i) {
        my $x=int( rand( 10**2 ));
        my $y=int( rand( 10**2 ));
        push @$list,[$x,$y];
    }

    #$point_number=19;
    #$list = [[2,36],[84,46],[4,63],[42,15],[31,43],[20,57],[21,68],
    #         [25,14],[24,89],[9,52],[33,62],[82,43],[32,54],[78,21],[67,0],
    #         [81,82],[31,22],[59,76],[92,10]];


    my $req_str=scalar( @$list ).qq{\n};
    $req_str.=join( qq{\n}, map { $_->[0].q{ }.$_->[1] } @$list );
    $result=[ sort { $a->[0] <=> $b->[0] }
              sort { $a->[1] <=> $b->[1] } @$list ];
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
