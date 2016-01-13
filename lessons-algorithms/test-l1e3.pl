#! /usr/bin/perl -w

use 5.018;

use strict;
use utf8;
no  warnings qw/utf8/;
use experimental qw/smartmatch/;

use Data::Dumper;
use IPC::Open2;
use Term::ANSIColor qw(:constants);

use constant REQS_LIMIT => 1;

my ($fhread, $fhwrite, $pid, $indexs);

# main
{
    for (my $i=0;$i<REQS_LIMIT;++$i) {
        my $test=generate_test ();
#        post_test ($test);
#        check_test ($test);
#        finalize ();
    }
}



sub generate_test {
    my $elts_number_o=int(rand(10)); # ordered, [0..n-1]
    my $elts_number_r=int(rand(10)); # rand, [0..m-1]
    my $req_string_o.=$elts_number_o.qq{\n};
    my $req_string_r.=$elts_number_r.qq{\n};

    my $o=[];
    my $r=[];
    $indexs=[];

    for (my $i=0;$i<($elts_number_o);++$i) {
        my $rand=(int(rand(10))+1);
        push @$o,$rand;
    }

    @$o = sort {$a <=> $b} @$o;

    for (my $i=0;$i<($elts_number_r);++$i) {
        my $rand=(int(rand(10))+1);
        push @$r,$rand;
    }

    $req_string_o.=join(q{ },@$o);
    $req_string_r.=join(q{ },@$r);

    my $req_string=$req_string_o.qq{\n}.$req_string_r;

    if ( !@$o || !@$r ) {
        return $req_string;
    }

    my $max_ri=0;
    my $min_ri=0;
    my $max_rv=( @$r )? $r->[0]:0;
    my $min_rv=$max_rv;

    for (my $i=1; $i < $elts_number_r; ++$i) {
        if ( $max_rv < $r->[$i] ) {
            $max_rv=$r->[$i];
            $max_ri=$i;
        }

        if ( $min_rv > $r->[$i] ) {
            $min_rv=$r->[$i];
            $min_ri=$i;
        }
    }

    my $left_oi=0;
    my $right_oi=0;
    my $diff_rmin=abs( $max_rv - (( @$o )? $o->[0]:0 ) ),
    my $diff_lmin=abs( $min_rv - (( @$o )? $o->[0]:0 ) );

    for (my $i=1; $i<$elts_number_o; ++$i) {

        my $diff_rcurr=abs( $o->[$i] - $max_rv );

        if ( $diff_rmin > $diff_rcurr ) {
            $diff_rmin=$diff_rcurr;
            $right_oi=$i;
        }

        my $diff_lcurr=abs( $o->[$i] - $min_rv );

        if ( $diff_lmin > $diff_lcurr ) {
            $diff_lmin=$diff_lcurr;
            $left_oi=$i;
        }
    }

    if ( $left_oi == $right_oi ) {
        @$indexs= ( $left_oi ) x $elts_number_r ;
        return $req_string;
    }

    my $count=$right_oi - $left_oi + 1;
    my @slice=@$o[$left_oi ... $right_oi];

    for ( my $i=0; $i<$elts_number_r; ++$i ) {
        my $index=binary_search_index($r->[$i],$count,\@slice);
        push @$indexs,( $index + $left_oi );
    }

#    say qq{=================\n}.Dumper( $o );
#    say qq{+++++++++++++++++\n}.Dumper( $r );
#    say qq{*****************\n}.Dumper( \@slice );

    return $req_string;
}


sub binary_search_index {
    my ($to_find, $count, $where_find)=@_;
    my $first=0;
    my $last=$count;
    my $mid=0;
    while ( $first < $last ) {
        $mid=int( ( $last + $first ) / 2 );
        if ( $to_find <= $where_find->[$mid] ) {
            $last=$mid;
        }
        else {
            $first=$mid+1;
        }
    }

    if ( $first == $count ) {
        --$first;
    }

    my $diff_first=abs( $where_find->[$first] - $to_find );
    my $diff_mid=abs( $where_find->[$mid] - $to_find );

    return ( $diff_first <= $diff_mid ) ? $first : $mid;
}


sub post_test {
    my($test,$fh)=@_;
    $pid=open2($fhread, $fhwrite,'/home/maxim/app/my/blogspot-examples/sandbox-build/l1e3');
    syswrite $fhwrite,$test,length($test);
    close $fhwrite;
}


sub check_test {
    my($test)=@_;
    my $r=<$fhread>;
    close $fhread;
    unless (defined $r) {
        say RED qq{failed : \n}.$test;
        return;
    }

    unless ( $r=~ /^[0-9 ]+$/ ) {
        say RED qq{failed response : \n}.$r;
        return;
    }

    my $ri=[split( / /, $r )];

    unless ( @$ri ~~ @$indexs ) {
        say RED qq{failed : \n[ }.join(q{ },@$ri).q{ ]};
    }

    else {
        say GREEN qq{pass : \n[ }.join(q{ },@$ri).q{ ]};
    }
}


sub finalize {
    waitpid($pid,0);
}
