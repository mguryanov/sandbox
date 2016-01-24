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

use constant REQS_LIMIT => 100;

my ($fhread, $fhwrite, $pid, $result);

# main
{
    for ( my $i=0; $i<REQS_LIMIT; ++$i ) {
        my $test=generate_test();
#        say qq{ test :\n}.$test.qq{\n};
#        say qq{ result :\n}.$result.qq{\n};
        usleep( 100000 );
        post_test( $test );
        check_test( $test );
        finalize ();
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

#    $segment_number=10;
#    $segments = [[27,20],[47,16],[51,31],[59,3],[67,15],
#                 [69,88],[73,22],[79,9],[83,45],[85,49]];

#    $segment_number=3;
#    $segments = [[1,3],[7,1],[2,3]];

#    $segment_number=3;
#    $segments = [[1,43],[13,5],[27,100]];

    my $req_str=$segment_number.qq{\n}.
                join( qq{\n}, map { $_->[0].q{ }.( $_->[0] + $_->[1] ) }
                @$segments );

=head

    This algorithm is very hard to understand.
    More simply for undarstanding algorithm consist several step:
    1- sort by first point all ranges
    2- summ lengths of 2 merging segment
    3- merge all segments by pair, and sub from sum-length nonunique points

=cut

    my $tmp=[ sort { $a->[0] <=> $b->[0] }
              sort { $a->[1] <=> $b->[1] } @$segments ];

    #say Dumper( $tmp );

    unless( $segment_number ) {
        $result=0;
        return $req_str;
    }

    $result=$tmp->[0]->[1];
    my $length=0;

    for( my $i1=0, my $i2=1; $i2<$segment_number; )
    {
        my $y1=( $tmp->[$i1]->[0] + $tmp->[$i1]->[1] );
        my $y2=$tmp->[$i2]->[0] + $tmp->[$i2]->[1];
        if( $tmp->[$i1]->[0] >= $y2 ) {
            ++$i2;$length=0;
        }
        # greater and equal guaranteed by sort => not check
        elsif( $y1 <= $tmp->[$i2]->[0] ) {
            $length=$tmp->[$i2]->[1];
            $i1=$i2++;
        }
        elsif( $y1 < $y2 ) {
            if( $tmp->[$i1]->[0] < $tmp->[$i2]->[0] ) {
                $result-=$y1-$tmp->[$i2]->[0];
            } else {
                $result-=$tmp->[$i1]->[1];
            }
            $tmp->[$i2]->[1]=$y2-$y1;
            $tmp->[$i2]->[0]=$y1;
            $length=$tmp->[$i2]->[1];
            $i1=$i2++;
        } else {
            if( $tmp->[$i1]->[0] < $tmp->[$i2]->[0] ) {
                $result-=$tmp->[$i2]->[1];
            } else {
                $result-=$y2-$tmp->[$i1]->[0];
            }
            $tmp->[$i1]->[1]=$y1-$y2;
            $tmp->[$i1]->[0]=$y2;
            ++$i2;$length=0;
        }
        $result+=$length;
    }

    return $req_str;
}



sub post_test {
    my($test,$fh)=@_;
    $pid=open2(
        $fhread, $fhwrite,
#        q{/home/maxim/app/my/blogspot-examples/sandbox-build/algorithms/l3e3}
        q{/home/plushka/qtprojects/sandbox-build/algorithms/l3e3-1}

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
                q{ must be : }.
                join( qq{\n}, map { $_->[0].q{ }.$_->[1] } @$result ).qq{\n}.
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
