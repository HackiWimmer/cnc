#!/bin/perl -w

#-------------------------------------------------------------------------------------------
use lib ".";
use strict;

use Class::Struct;

my $tempDir = "/home/fw591/temp";
my $wxFile  = "./CncControlerGui/wxcrafter.cpp";

my $newFile = $tempDir . "newSizer.txt";
my $addFile = $tempDir . "addSizer.txt";
my %newRegister;

` grep Sizer $wxFile | grep 'new' | grep 'wxFlexGridSizer'  > $newFile`;
` grep Sizer $wxFile | grep '>Add' > $addFile`;

print "New File: ", $newFile, "\n";
print "Add File: ", $addFile, "\n";


struct Sizer => {
    row  => '$',
    col  => '$',
    aaa  => '$',
    bbb  => '$',
};

sub strip 
{
	if ( !@_ ) { die "strip(): No string received\n"; }
	my $str  = shift;
	
	$str =~ s/,//g;
	$str =~ s/;//g;
	$str =~ s/\)//g;
	$str =~ s/ //g;
	$str =~ s/\n//g;
	
	return $str;
}	

open (in, $newFile) || die $!;
	while (<in>) {

		#print $_;
		
		my @tokens1 = split(/ /, $_);
		my $var = $tokens1[5];
		#print "'", $var, "'\n";
		
		my @tokens2 = split(/\(/, $_);
		my @tokens3 = split(/ /, $tokens2[1]);
		#print strip($tokens3[0]), "\n";
		#print strip($tokens3[1]), "\n";
		#print strip($tokens3[2]), "\n";
		#print strip($tokens3[3]), "\n";
		
		my $sizer = Sizer->new(); 
		$sizer->row(strip($tokens3[0])); 
		$sizer->col(strip($tokens3[1])); 
		$sizer->aaa(strip($tokens3[2])); 
		$sizer->bbb(strip($tokens3[3])); 
		
		$newRegister{$var} = $sizer;
		
	
	}
close in;

#print %newRegister;


open (in, $addFile) || die $!;
	while (<in>) {
		#print $_;
		
		my @tokens1 = split(/-/, $_);
		my $var = strip($tokens1[0]);
		
		my @tokens2 = split(/>/, $_);
		my $func = strip($tokens2[1]);
		
		my $sizer2 = $newRegister{$var}; 
		if ( $sizer2 ) {
			
			my $row = $sizer2->row;
			$row -= 1;
			
			my $col = $sizer2->col;
			$col -= 1;
			
   			my @tokens3 = split(/\(/, $_);
   			my $val = strip($tokens3[1]);
						
			if ( index($func, "AddGrowableRow") != -1) {
	   			
	   			if ( $val gt $row ) { 
	   				print "AddGrowableRow: ", $var, "(", $row + 1, ", ", $col + 1, ")\n"; 
	   				print " ", $val, " > ", $row, "\n"; 
	   			}
			}
			
			if ( index($func, "AddGrowableCol") != -1) {
	   			
	   			if ( $val gt $col ) { 
	   				print "AddGrowableCol: ", $var, "(", $row + 1, ", ", $col + 1, ")\n"; 
	   				print " ", $val, " > ", $col, "\n"; 
	   			}
				
			}
		}
		
		#print "! '", $var, "', ", $func, "\n";
	}
close in;
