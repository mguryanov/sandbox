#! /usr/bin/perl -w

use 5.010;

use strict;
use utf8;
no  warnings 'utf8';

use Data::Dumper;
use IPC::Open2;
use Term::ANSIColor qw(:constants);

use constant REQS_LIMIT => 20;

my ($fhread, $fhwrite, $pid, $max_i, $max_j);

# main
{
	for (my $i=0;$i<REQS_LIMIT;++$i) {
		my $test=generate_test ();
		post_test ($test);
		check_test ($test);
		finalize ();
	}
}



sub generate_test {
	my $elts_number=int(rand(1000))+1;
	my $req_string.=$elts_number.qq{\n};

	my $v1=[];
	my $v2=[];

	for (my $i=0;$i<($elts_number);++$i) {
		my $rand=(int(rand(100000))+1);
		push @$v1,$rand;
	#say Dumper($v1);
		$rand=(int(rand(100000))+1);
		push @$v2,$rand;
	#say Dumper($v2);
	}

	$req_string.=join(q{ },@$v1).qq{\n};
	$req_string.=join(q{ },@$v2);

    	$max_i=0,$max_j=0;

    	my $max_sum=$v1->[0]+$v2->[0];

    	for (my $i=0,my $j=1; $i < $elts_number; ++$i, $j=$i ) {
        	for ( ; $j < $elts_number; ++$j ) {
	            	my $curr_sum=$v1->[$i]+$v2->[$j];
			if ( $max_sum < $curr_sum ) {
			        $max_sum = $curr_sum;
			        $max_i=$i;
			        $max_j=$j;
			}
        	}
    	}

	return $req_string;
}


sub post_test {
	my($test,$fh)=@_;
	$pid=open2($fhread, $fhwrite,'/home/maxim/app/my/blogspot-examples/sandbox-build/l1e2-2');
	syswrite $fhwrite,$test,length($test);
	close $fhwrite;
}



sub check_test {
	my($test)=@_;
	my $r=<$fhread>;
	close $fhread;
	unless (defined $r) {
		say RED q{failed : }.$test;
		return;
	}

	$r=~ /^(\d+) (\d+)$/;

	if ($1 > $2) {
		say RED q{failed : }.$1.q{:}.$2;
	}

	elsif ($1 != $max_i || $2 != $max_j ) {
		say RED q{failed : }.$1.q{[}.$max_i.q{]}.q{:}.$2.q{[}.$max_j.q{]}.qq{\n}.$test;
	}
	
	else {
		say GREEN q{pass : }.$1.q{:}.$2;
	}

}



sub finalize {
	waitpid($pid,0);
}



=head
=cut



