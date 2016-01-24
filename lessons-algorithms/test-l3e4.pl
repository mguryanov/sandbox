#! /usr/bin/perl -w

use 5.018;

use strict;
use utf8;
no  warnings qw/utf8/;
use experimental qw/smartmatch/;

use Data::Dumper;
use IPC::Open2;
use Term::ANSIColor qw(:constants);
use Time::HiRes qw(usleep nanosleep);

use constant REQS_LIMIT => 1;

my ($fhread, $fhwrite, $pid, $result);

# main
{
    for ( my $i=0; $i<REQS_LIMIT; ++$i ) {
        my $test=generate_test();
        say qq{ test :\n}.$test.qq{\n};
        say qq{ result :\n}.$result.qq{\n};
#        usleep( 100000 );
#        post_test( $test );
#        check_test( $test );
#        finalize ();
    }
}



sub generate_test {
    my $k_value=5;#int( rand( 10**2 ));
    my $elts_number=$k_value * 3;#int( rand( 10 ));
    my $list=[];

    my $max=0;
    my $factor=0;
    my $tmp=[];

    for (my $i=0;$i<$elts_number;++$i) {
        my $v=int( rand( 10 )) + $factor;
        $max=$v if( $v > $max );
        push @$list,$v;
        unless( ( $i + 1 ) % $k_value ) {
            $factor=$max;
        }
    }

#    @$list = map { map { $_ } @$_ } @$result;
    my $req_str=$elts_number.qq{\n}.
                join( qq{ }, @$list ).qq{\n}.
                $k_value;

    $result = join( qq{ }, sort { $a <=> $b } @$list );
    return $req_str;
}



sub post_test {
    my($test,$fh)=@_;
    $pid=open2(
        $fhread, $fhwrite,
#        q{/home/maxim/app/my/blogspot-examples/sandbox-build/algorithms/l3e4}
        q{/home/plushka/qtprojects/sandbox-build/algorithms/l3e4}
    );
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
                q{ test : }.$test;
        return;
    }

    unless ( $r =~ /^$result/ ) {
        say RED qq{failed response : \n}.$r.qq{\n}.
                q{ must be : }.$result.qq{\n}.
                q{ test : }.$test;
        return;
    }

    say GREEN qq{test pass\n};
}


sub finalize {
    waitpid($pid,0);
}
