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
my @delimiters=( q{,}, q{.}, q{;}, q{!}, q{"},
                 qq{\[}, q{=}, q{%}, q{#},q{A},
                 q{Z}, q{L}, q{@}, q{*}, q{V},
                 q{1}, q{0}, q{ }, qq{\n}, qq{\t}
            );
my @words=qw/ a b c d e f h this test for astarta m /;


# main
{
    for ( my $i=0; $i<REQS_LIMIT; ++$i ) {
        my $test=generate_test();
        post_test( $test );
        check_test( $test );
        finalize ();
    }
}



sub generate_test {
    my $words_in_test_text_counter=0;
    my $repeat=100;#int( rand( 10 ))+1;

    my $list=[ map { ({q{w_index}=>$_, q{w_count}=>0}) }
                    0..scalar( @words )-1];

    for (my $i=0;$i<( $repeat );++$i) {
        my $word_index=int( rand( scalar( @words )));
        my $word_count=int( rand( 10 ));
        $list->[$word_index]->{q{w_index}}=$word_index;
        $list->[$word_index]->{q{w_count}}+=$word_count;
        $words_in_test_text_counter+=$word_count;
    }

    my $req_str=q{};
    @$list=grep { $_->{q{w_count}} > 0 } @$list;

    @$list=
        sort{ $words[$a->{q{w_index}}] cmp $words[$b->{q{w_index}}]} @$list;

    $result=q{};
    for my $v ( @$list ) {
        $result.=$words[$v->{q{w_index}}].
                 q{ - }.
                 $v->{q{w_count}}.
                 qq{\n};
    }

    for (my $i=0;$i<( $words_in_test_text_counter );++$i) {
        my $delim_index=int( rand( scalar( @delimiters ) ));
        my $remain_index=int( rand( scalar( @$list )));
        $req_str.=  $delimiters[$delim_index].
                    $words[$list->[$remain_index]->{q{w_index}}];
        --$list->[$remain_index]->{q{w_count}};
        @$list=grep { $_->{q{w_count}} > 0 } @$list;
    }

    return $req_str;
}



sub post_test {
    my($test,$fh)=@_;
    $pid=open2($fhread, $fhwrite,$ARGV[0]);
    syswrite $fhwrite,$test,length($test);
    close $fhwrite;
}


sub check_test {
    my($test)=@_;
    my $r=q{};

    while( <$fhread> ) {
        $r.=$_;
    }
    close $fhread;

    unless (defined $r) {
        say RED qq{failed : \n}.
                qq{ must be :\n}.$result.qq{\n}.
                q{ test : }.$test;
        return;
    }

    unless ( $r =~ /^$result/s ) {
        say RED qq{failed response : \n}.$r.qq{\n}.
                qq{ must be :\n}.$result.qq{\n}.
                q{ test : }.$test;
        return;
    }

    say GREEN qq{test pass};
}


sub finalize {
    waitpid($pid,0);
}
