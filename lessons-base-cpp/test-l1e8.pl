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

my @first_names=qw/ Иван Петр Сидор /;
my @second_names=qw/ Иванов Петров Сидоров /;
my @father_names=qw/ Иванович Петрович Сидорович /;

my @born_places=qw/ Москва Питер Челябинск Тула Самара /;
my @faq_names=qw/ ПС-5 Мехмат Биологический Химический Юрфак /;

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

    my $req_str=q{};
    my $born_years=[];
    my $students_count=int( rand( 10 )) + 1;

    while( --$students_count )
    {
        my $f_name_index=int( rand( 2 ));
        my $s_name_index=int( rand( 2 ));
        my $fr_name_index=int( rand( 2 ));
        my $b_place_index=int( rand( 4 ));
        my $faq_name_index=int( rand( 4 ));
        my $tel_number=time()%10**6;
        my $course_num=int( rand( 5 )) + 1;

        my (undef,undef,undef,$mday,$mon,$year,undef,undef,undef)
                = localtime( time - 31536000 * ( 20 + int( rand( 5 ))));

        $year+=1900;
        ++$mon;

        push @$born_years,$year;
        my $bdata=sprintf( "%02d.%02d.%d", $mday, $mon, $year );

        # Иванов, Иван, Иванович, 01.01.1998, Москва, 128500, РК-6, 4.

        $req_str.=  $second_names[$s_name_index].q{, }.
                    $first_names[$f_name_index].q{, }.
                    $father_names[$s_name_index].q{, }.
                    $bdata.q{, }.
                    $born_places[$b_place_index].q{, }.
                    $tel_number.q{, }.
                    $faq_names[$faq_name_index].q{, }.
                    $course_num.q{. };
    }

    # РК-6, 3, 1994
    my $test_faq_index=int( rand( 4 ));
    my $test_course_num=int( rand( 5 ) + 1 );
    my $test_byear=( sort {$a <=> $b} @$born_years )[0] + int( rand( 10 ) );

    $req_str.=  $faq_names[$test_faq_index].q{, }.
                $test_course_num.q{, }.
                $test_byear;

    return $req_str;
}



sub post_test {
    my($test,$fh)=@_;
    $pid=open2(
            $fhread, $fhwrite,
            '/home/maxim/app/my/blogspot-examples/sandbox-build/base/l1e8'
        );
    binmode $fhwrite,':utf8';
    syswrite $fhwrite,$test,length($test);
    close $fhwrite;
}


sub check_test {
    my($test)=@_;
    binmode $fhread,':utf8';
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
                q{ must be : }.$result.q{ : }.
                q{ test : }.$test;
        return;
    }

    say GREEN qq{test pass\n};
}


sub finalize {
    waitpid($pid,0);
}
