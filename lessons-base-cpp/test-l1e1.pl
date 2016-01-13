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

my @words = qw/ оДин дВа трИ чЕтрые пЯть шестЬ Семь вОсемь деВять нОль /;
my @punctuation = qw/ . ? ! !! !!! /;

my ($fhread, $fhwrite, $pid, $result);

# main
{
    for ( my $i=0; $i<REQS_LIMIT; ++$i ) {
        my $test=generate_test();
        say $test;
        say $result;
#        post_test( $test );
#        check_test( $test );
#        finalize ();
    }
}



sub generate_test {

    my $word_to_match_index=rand( 10 );
    my $sentence_number=rand( 10**1 );

    my $req_str=lc( $words[$word_to_match_index] );

    for(my $si=0;$si<$sentence_number;++$si) {
        $req_str.=q{ };
        my $word_in_sentence_number=rand( 10**1 );
        for( my $wi=0; $wi<$word_in_sentence_number; ++$wi ) {
            $req_str.=$words[rand( 10 )].q{ } x rand( 10 );
        }
        chop( $req_str );
        $req_str.= $punctuation[rand( 5 )];
    }

    my $word_to_match_count = () =
            $req_str=~/ $words[$word_to_match_index](?:[ .?!]+)/g;

    my $sentence_count = () =
            $req_str=~/\.|\?|!+/g;

    #say $sentence_count;
    #say $word_to_match_count;

    $result=sprintf "%.1f", $word_to_match_count/$sentence_count;
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
