#!/usr/bin/perl -w

foreach $arg (@ARGV) {
 &spider(0,$arg);
#@qq = &scnfle($arg);
#print @qq;
}

sub scnfle {
 local($topfile) = $_[0];
 local(@listofhrefs) = ();

 open(HF,$topfile) || die "Couldn't open $topfile!\n";

 while (<HF>) {
   if (s/.*href=(["a-zA-Z0-9:\/].*)">.*/$1/) {
     s/"//g;	# strip out quotes
     s/>.*//;	# strip of end
     chop;
     @listofhrefs=(@listofhrefs,$_)
     };
   }
 @listofhrefs;
}

sub spider {
 local($thislevel) = $_[0];
 local($thisfile) = $_[1];
 
 if ($thislevel >=2) {return;}

 print "spider: level $thislevel, $thisfile\n";
 @hrefs=scnfle($thisfile);
#print "spider: $thisfile: @hrefs\n";
 foreach $href (@hrefs) {
   if ($href ne $thisfile) {
      print "$thisfile: -> $href\n";
      spider($thislevel+1,$href);
      }
   }
}
