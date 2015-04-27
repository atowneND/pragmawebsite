#!/usr/bin/perl
#
# Creates new dummy web pages

die;
die "pass page name from menu" if scalar(@ARGV == 0);

my $page_name = $ARGV[0];
my $prog_page_name = $page_name;

$prog_page_name = lc $prog_page_name;
$prog_page_name =~ s/ /_/g;

# copy file
`cp about.html $prog_page_name.html`;

`perl -i -pe 's/"#">$page_name/"$prog_page_name.html">$page_name/g' /Users/charlesshinaver/Dropbox/Programming/HTML/pragmaSite/*.html`;
