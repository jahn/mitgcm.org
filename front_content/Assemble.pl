#! /usr/bin/env perl

#  Ed Hill
#  Tue Dec  2 20:17:27 EST 2003

#  Assemble the web pages from XML files.

open(TOP,"<top_list.txt") or die "ERROR: can't open \"top_list.txt\"\n";
while($line = <TOP>)  {
    chomp($line);
    length($line) > 0 and @top_list[$top_list++] = $line;
}

open(TEMPL,"<template.xml") or die "ERROR: can't open \"template.xml\"\n";
$template = join "", <TEMPL>;

mkdir html;

foreach $file (@top_list)  {

    $fname = "<./" . $file . ".xml";
    open(INF,$fname) or die "ERROR: can't open \"$fname\"\n";
    $f_all = join "", <INF>;

    $_ = $f_all;
    /<body>(.*)<\/body>/s and $body = $1;

    $_ = $f_all;
    /<meta *name="add_title" *content="(.*)"/ and $title = $1;

    $_ = $f_all;
    /<meta *name="add_name_0" *content="(.*)"/ and $name = $1;

    $menu = "";
    foreach $f (@top_list)  {
	if ($f ne $file) {
	    $tmp = "<a href=\"./" . $f . ".html\">" . $f . "</a><br />\n";
	} else {
	    $tmp = "<b>" . $name . "</b>\n";
	}
	$menu = join "", $menu, $tmp;
    }

    $_ = $template;
    s/<!--ADDTITLE-->/$title/s;
    s/<!--ADDMENU-->/$menu/s;
    s/<!--ADDCONTENT-->/$body/s;

    $out_name = ">./html/" . $file . ".html";
    open(OUT,$out_name) or die "ERROR: can't open \"$out_name\"\n";
    print OUT $_;

    close(OUT);
    close(INF);
}

