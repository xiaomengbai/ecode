#!/usr/bin/perl

use strict; 
use 5.010;
use warnings;

my $size = $ARGV[0];
my $filename = "TMP";

say "generating file $filename in size $size Bytes...";

open TMP_FILE, ">$filename";

for (1..$size) {
    my $char = int rand 255;
    print TMP_FILE chr($char);
}
