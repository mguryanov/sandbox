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

my ($fhread, $fhwrite, $pid, $result);

# main
{
    for ( my $i=0; $i<REQS_LIMIT; ++$i ) {
        my $test=generate_test();
#        say $test;
#        say $result;
        post_test( $test );
        check_test( $test );
        finalize ();
    }
}



sub generate_test {
    my $elts_number=int(rand(10**2))+1;
    my $list=[];

    for (my $i=0;$i<($elts_number);++$i) {
        my $value=int( rand( 10**3 ) )+1;
        push @$list, $value;
    }

#    $elts_number=8;
#    @$list = qw/ 21 22 5 39 35 1 16 7 /;

    my $size=scalar( @$list );
    my $req_str=join(q{ },@$list);

    $result=join( q{ }, sort { $a <=> $b } @$list );
    return $req_str;
}



sub post_test {
    my($test,$fh)=@_;
    $pid=open2($fhread, $fhwrite,'/home/maxim/app/my/blogspot-examples/sandbox-build/l3e1');
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

    unless ( $r =~ /^$result$/ ) {
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
