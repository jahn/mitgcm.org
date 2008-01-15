#! /usr/bin/perl

#  Ed Hill
#  Tue Dec  2 20:17:27 EST 2003

#  Assemble the web pages from XML files.

$topdir = ".";
$file_list = $topdir . "/order.txt";
open(ORDER,$file_list) or die "ERROR: can't open \"$file_list\"\n";
$_ = join(" ", <ORDER>);
s/\n//s;
@flist = split(" ", $_);

open(TEMPL,"<template.xml") or die "ERROR: can't open \"template.xml\"\n";
$template = join "", <TEMPL>;

system("rm -rf html");
system("mkdir html");

print "Parsing files ...  ";
foreach $file (@flist) {
    $fname = $topdir . "/" . $file;
    open(INF,$fname) or die "ERROR: can't open \"$fname\"\n";
    $all = join "", <INF>;
    $_ = $all;
    /<meta *name="add_name_0" *content="(.*)"/  and $k0 = $1;
    $_ = $all;
    /<meta *name="add_name_1" *content="(.*)"/ and $k1 = $1;
    $name0{$file} = $k0;
    $name1{$file} = $k1;
    close(INF);
}
print "  done\n";

print "Generating XHTML ...";
foreach $file (@flist) {
    
    $fname = "<" . $topdir . "/" . $file;
    open(INF,$fname) or die "ERROR: can't open \"$fname\"\n";
    $f_all = join "", <INF>;
    close(INF);

    #  Get the body
    $_ = $f_all;
    /<body>(.*)<\/body>/s and $body = $1;

    #  Get the title
    $_ = $f_all;
    /<meta *name="add_title" *content="(.*)"/ and $title = $1;

    #  Create the menu
    $cname = $name0{$file};
    $menu = "";
    foreach $fm (@flist) {
	$ind = "";
	$name = $name0{$fm};

	if ($name eq "Documentation") {
	    $tmp = "<a href=\"http://eddy.csail.mit.edu/r2/latest/\">"
		. "Documentation</a><br />\n";
	    $menu = join "", $menu, $tmp;
	    next;
	}

	if (length($name1{$fm}) > 0) {
	    $ind = "&nbsp;&nbsp;&nbsp;";
	    $name = $name1{$fm};
	}
	if (length($ind) > 0 and ($name0{$fm} ne $cname)) {
	    next;
	}
	if ($fm ne $file) {
	    $_ = $fm;
	    s/.xml/.html/;
	    $tmp = $ind . "<a href=\"./" . $_ . "\">" . $name . "</a><br />\n";
	} else {
	    $tmp = $ind . "<b>" . $name . "</b><br />\n";
	}
	$menu = join "", $menu, $tmp;
    }
    # $tmp = "<a href=\"./htdig\" />Search</a><br />\n";
    # $menu = join "", $menu, $tmp;

    #  Create the output
    $_ = $template;
    s/<!--ADDTITLE-->/$title/s;
    s/<!--ADDMENU-->/$menu/s;
    s/<!--ADDCONTENT-->/$body/s;
    $content = $_;

    $_ = $file;
    s/.xml/.html/;
    $out_name = ">./html/" . $_;
    open(OUT,$out_name) or die "ERROR: can't open \"$out_name\"\n";
    print OUT $content;

    close(OUT);
}
print "  done\n";

