#! /usr/bin/perl -w

use 5.018;

use strict;
use utf8;
no  warnings qw/utf8/;
use experimental qw/smartmatch/;

use Data::Dumper;
use IPC::Open2;
use Term::ANSIColor qw(:constants);

use constant REQS_LIMIT => 100;

my ($fhread, $fhwrite, $pid, $result);

# main
{
    for ( my $i=0; $i<REQS_LIMIT; ++$i ) {
        my $test=generate_test();
        say $test;
        post_test( $test );
        check_test( $test );
        finalize ();
    }
}



sub generate_test {
    my $elts_number=int(rand(10**2))+1;
    my $k_value=int(rand(10**2))+1;
    my $list=[];

    for (my $i=0;$i<($elts_number);++$i) {
        my $apple_value=int( rand( $k_value ) )+1;
        push @$list, $apple_value;
    }

#    $elts_number=8;
#    $k_value=40;
#    @$list = qw/ 21 22 5 39 35 1 16 7 /;

    my $size=scalar( @$list );
    my $req_str=$elts_number.qq{\n}.
                join(q{ },@$list).qq{\n}.
                $k_value;

    # make heap with priority
    # from array of values

    my $heap=[];

    for ( my $i=int( $size / 2 ) - 1; $i>=0; --$i ) {
        sift_down( $list, $i );
    }

    # pop maximum ( 1 or more ) apple-values from heap (<= K-value)
    # modify some apple-value
    # siftdown changed value
    # repeat until apples count not 0

    my $tmp=[];
    my $count=0;
    my $lk_value=$k_value;

    while( @$list )
    {
        $lk_value-=$list->[0];

        unless( $lk_value >= 0 ) {
            $lk_value=$k_value;
            @$list=( @$tmp, @$list );
            @$tmp=();
            $size=scalar( @$list );
            for ( my $i=int( $size / 2 ) - 1; $i>=0; --$i ) {
                sift_down( $list, $i );
            }
#            say q{:1:}.Dumper( $list );
            ++$count;
            next;
        }

#        say q{ + : }.$list->[0];

        if( $list->[0] != 1 ) {
            push @$tmp,int( $list->[0] / 2 );
        }

        my $last=$size-1;
        $list->[0]=$list->[$last];
        pop( @$list );

        unless( @$list ) {
            @$list=@$tmp;
#            say q{:2:}.Dumper( $list );
            $lk_value=$k_value;
            @$tmp=();
            ++$count;
        }

        $size=scalar( @$list );
        sift_down( $list, 0 );
    }

    $result=$count;
    return $req_str;
}



sub post_test {
    my($test,$fh)=@_;
    $pid=open2($fhread, $fhwrite,'/home/maxim/app/my/blogspot-examples/sandbox-build/l2e3');
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


sub sift_down {
    my ($arr,$i)=@_;
    my $left=$i*2+1;
    my $right=$i*2+2;
    my $largest=$i;
    my $size=scalar( @$arr );

    if ( $left < $size && $arr->[$i] < $arr->[$left] ) {
        $largest=$left;
    }

    if ( $right < $size && $arr->[$largest] < $arr->[$right] ) {
        $largest=$right;
    }

    if ( $largest != $i ) {
        ( $arr->[$largest], $arr->[$i] ) =
                        ( $arr->[$i], $arr->[$largest] );
        sift_down( $arr, $largest );
    }
}

=head
    my $row=q{};
    my $pow=0;
    for( my $i=0, my $l=0; $i<$size; ++$i,++$l ) {
        $row.=$list->[$i].q{ };
        unless ( $l % 2**$pow ) {
            say $row;
            ++$pow;
            $row=q{};
            $l=0;
        }
        if ( $i+1 == $size ) {
            say $row;
        }
    }
=cut
