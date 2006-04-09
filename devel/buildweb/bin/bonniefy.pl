#!/usr/bin/perl -w

#VARIABLES
$DEBUG=1;
$BUILDALLFILES=0;

#DIRECTORIES (ABSOLUTE PATHS)
$targetdir     = "/u/httpd/html/devel/sealion/";	# Web site location
$homedir       = "/u/httpd/html/devel/buildweb/";	# This code dir
$latexroot   = $homedir."latex/";          # LaTeX pages
$vdbroot     = $homedir."vdb/";            # VDB pages
$templatedir = $homedir."templates/";      # Conversion templates
$pfilesdir   = $homedir."program_files/";  # Program/config files

# PROCESS COMMAND LINE ARGUMENTS
foreach $_ (@ARGV) {
 if (/--targetdir=(.*)/) {$targetdir=endslash($1);
                          print "Setting targetdir = $targetdir\n";};
 if (/--homedir=(.*)/) {
     $homedir=endslash($1);
     #eh3 subdirectories of homedir
     $latexroot   = $homedir."latex/";            # LaTeX pages
     $vdbroot     = $homedir."vdb/";	          # VDB pages
     $templatedir = $homedir."templates/";	  # Conversion templates
     $pfilesdir   = $homedir."program_files/";    # Program/config files
     print "Setting homedir = $homedir\n";
 };
 if (/--latexroot=(.*)/) {$latexroot=endslash($1);
                          print "Setting latexroot = $latexroot\n";};
 if (/--vdbroot=(.*)/) {$vdbroot=endslash($1);
                          print "Setting vdbroot = $vdbroot\n";};
 if (/--templatedir=(.*)/) {$templatedir=endslash($1);
                          print "Setting templatedir = $templatedir\n";};
 if (/--pfilesdir=(.*)/) {$pfilesdir=endslash($1);
                          print "Setting pfilesdir = $pfilesdir\n";};
}


#DIRECTORIS WITHIN WEB SITE
$webbase = "../";
$latek2htmldir = "online_documents/";
$coderefdir = "code_reference/";
$vdbtargetdir = "code_reference/vdb";

#File names
$toc2url          = "< ".$pfilesdir."toc2url.txt";
$toc2pattern      = "< ".$pfilesdir."toc2pattern.txt";
$template2url     = "< ".$pfilesdir."template2url.txt";
$template2pattern = "< ".$pfilesdir."template2pattern.txt";


#FILES TO COPY AND FORMAT FROM PAOC SERVER
#These files are brought over via FTP
$frontpage = "home_page/frontpage.html";
$subspage = "code_reference/subroutine.html";
$paramspage = "code_reference/parameters.html";
$sourcepage = "code_reference/sourcefiles.html";
$varspage = "code_reference/variabledictionary.html";
@paocfiles = ($frontpage, $subspage, $paramspage, $sourcepage, $varspage);

#GLOB WEB FILES TO BE FORMATTED
#directory containing all the latek generated html
@htmlfiles = ($targetdir . $latek2htmldir . "manual.html");
@htmlfiles = (@htmlfiles, glob($targetdir . $latek2htmldir . "node*.html"));
if ($BUILDALLFILES) {
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "*.htm"));
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "code/*.htm"));
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "code/*.html"));
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "code/diags/inc/*.html"));
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "code/diags/src/*.html"));
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "code/eesupp/inc/*.html"));
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "code/eesupp/src/*.html"));
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "code/model/inc/*.html"));
  @htmlfiles = (@htmlfiles, glob($targetdir . $coderefdir. "code/model/src/*.html"));
}


#TEMPLATE TOC PREPROCESSING
#read in template formatting and mapping files
open(TOC2URL,$toc2url) || die("can't open $toc2url");
@toc2urllist = parsepatternfile(<TOC2URL>);

open(TOC2PATTERN,$toc2pattern) || die("can't open $toc2pattern");
@toc2patternlist = parsepatternfile(<TOC2PATTERN>);

#TEMPLATE SUBSTITUTION FILES
open(TEMPLATE2URL,$template2url) || die("can't open $template2url");
@template2urllist = parsetemplatefile(<TEMPLATE2URL>);

open(TEMPLATE2PATTERN,$template2pattern) || die("can't open $template2pattern");
@template2patternlist = parsetemplatefile(<TEMPLATE2PATTERN>);

#FIND TEMPLATE FILES
#Find all template files from template2url.txt and template2pattern.txt.
#The template files are even entries in @template2patternlist and
#@template2urllist. This is the list of templates that are actually used.
for ($i=0;$i<$#template2patternlist; $i=$i+2){
  @templatefiles = (@templatefiles,$template2patternlist[$i]);
}
for ($i=0;$i<$#template2urllist; $i=$i+2){
  @templatefiles = (@templatefiles,$template2urllist[$i]);
}

#FIND SINGLE FILE NAMES FROM REGEXP
#Find html file that corresponds to each REGEXP for TOC entries
#loop through patterns
for ($i=1;$i <= $#toc2patternlist; $i=$i+2){

  $pattern = $toc2patternlist[$i];
  $notfound = 1;
  $j=0; 

  while (($j <= $#htmlfiles) && ($notfound)) {
    open(THISHTML,$htmlfiles[$j]) || die("can't open $htmlfiles[$j]");

    while (<THISHTML>)  {
      if (/$pattern/){
        #$htmlfiles[$j] =~ /.*\/(.*\.html)/;
        #print "$1\n";
        $htmlfiles[$j] =~ /(node\d+\.html)/;
        $toc2patternlist[$i] = "$webbase" . "$latek2htmldir" . $1;
        $notfound = 0;
        last;
      } # check for pattern match
    } # loop through lines in html file

    $j++;
    close THISHTML;

  } #loop through htmlfiles

  #ANY INCORRECT REGEXP WILL NOT HAVE BEEN REPLACED BY FILE NAMES.
  #CHECK FOR EXTRA SPACES BETWEEN NUMBER AND TITLE AND AT END OF TITLE
  if ($DEBUG) {
    print "$toc2patternlist[$i-1], $toc2patternlist[$i]\n";
  }

} #loop through REGEXP

@tocreplace = (@toc2patternlist, @toc2urllist);

#PREFORMAT FRONTPAGE AND OTHER HTML FILES FROM PAOC SERVER

for ($thispaocfile=0; $thispaocfile<$#paocfiles; $thispaocfile++) {
  if ($DEBUG) {print "Preformatting paocfile $paocfiles[$thispaocfile]\n";}
  open(THISHANDLE,"$targetdir"."$paocfiles[$thispaocfile]") || die "couldn't open " . "$targetdir"."$paocfiles[$thispaocfile]";
  @thisfile = <THISHANDLE>;
  for ($thisreplace=0; $thisreplace<$#tocreplace; $thisreplace=$thisreplace+2) {
    foreach (@thisfile) {
      s/$tocreplace[$thisreplace]/$tocreplace[$thisreplace+1]/;
    }  
  }
  close THISHANDLE;
  open(THISHANDLE, "> " . "$targetdir"."$paocfiles[$thispaocfile]") || die "couldn't open " . "$targetdir"."$paocfiles[$thispaocfile]";
  print THISHANDLE @thisfile;
  close THISHANDLE;
}

#LOOP THROUGH TEMPLATES
### added by AJA: create a flag to indicate whether file has been associated
#                 with a template (1 = no and 0 = yes)
      $thisfile=0;
      while ($thisfile <= $#htmlfiles) {
       $htmlfileassociated[$thisfile]=1;
       $thisfile++;
      } # loop through file
### end addition by AJA
for ($thistemplate=0; $thistemplate <= $#templatefiles; $thistemplate++) {
  if ($DEBUG) {print "Preformatting template $templatefiles[$thistemplate]\n";}
  open(THISHANDLE, $templatedir . $templatefiles[$thistemplate]) || 
    die "couldn't open " . $templatedir . $templatefiles[$thistemplate];
  @thisfile = <THISHANDLE>;

  #PREFORMAT TEMPLATES
  for ($thisreplace=0; $thisreplace<$#tocreplace; $thisreplace=$thisreplace+2) {
    foreach (@thisfile) {
      s/$tocreplace[$thisreplace]/$tocreplace[$thisreplace+1]/;
    }
  }

  close THISHANDLE;

  #FIND ALL HTML FILES THAT USE THIS TEMPLATE
  @subfiles = ();

  #... FROM URL
  for ($thisurl=0;$thisurl < $#template2urllist; $thisurl=$thisurl+2){
    if ($template2urllist[$thisurl] eq $templatefiles[$thistemplate]) {
      @subfiles = (@subfiles, glob("$targetdir" . "$template2urllist[$thisurl+1]"));
    }
  }
  #... HTML FILES FROM PATTERN
  for ($thispattern=0;$thispattern < $#template2patternlist; $thispattern=$thispattern+2){
    if ($template2patternlist[$thispattern] eq $templatefiles[$thistemplate]) {

      $pattern = $template2patternlist[$thispattern+1];
      $thisfile=0; 
      while ($thisfile <= $#htmlfiles) {
        open(THISHTML,$htmlfiles[$thisfile]) || die("can't open $htmlfiles[$thisfile]");
        while (<THISHTML>)  {
          if (/$pattern/ && $htmlfileassociated[$thisfile]){
            @subfiles = (@subfiles, $htmlfiles[$thisfile]);
            $htmlfileassociated[$thisfile]=0;
          } # check for pattern match
        } # loop through lines in html file
        $thisfile++;
        close THISHTML;
      } # loop through file

    } # check if pattern for this template
 
  } #for each html file

  if ($DEBUG) {
    print "Template $templatefiles[$thistemplate] is used by these html files :\n";
    #print (join("\n",@subfiles) . "\n");
  }

  @pretitle=();
  @title2content=();
  @postcontent=();
  foreach (@thisfile){
    if ($#title2content == -1) {
      if (/(.*)_TITLE_(.*)/i) {
        @pretitle = (@pretitle, $1);
        @title2content = ("$2");
      } else {
        @pretitle = (@pretitle, "$_");
      }
    } elsif ($#postcontent == -1) {
      if (/(.*)_CONTENT_(.*)/i) {
        @title2content = (@title2content, "$1");
        @postcontent = (@postcontent, "$2");
      } else {
        @title2content = (@title2content, "$_");
      } 
    } else {
      @postcontent = (@postcontent, "$_");
    }
  }

  #OPEN SUBFILES, EXTRACT TITLE AND BODY, PLUG INTO TEMPLATE
  for ($thissubindx=0; $thissubindx<= $#subfiles; $thissubindx++){

    #we're overwriting each file as we go
    $savefile = $subfiles[$thissubindx]; 

    if ($DEBUG) {print "$savefile\n";}
    open (SUBHANDLE, $subfiles[$thissubindx]) || die "Can't open $subfiles[$thissubindx]";
    @thissubfile = <SUBHANDLE>;
    close SUBHANDLE;
    $thisline = 0;
    while ($thisline <= $#thissubfile){
      if ($thissubfile[$thisline] =~ /<TITLE>(.*)<\/TITLE>/i) {
        $thistitle = $1;
        last;
      }
      $thisline++;
    }
    $thistitle = "no title";
    @thiscontent = ();
    $working=1;
    $multiple=0;     #title spans multiple lines
    foreach (@thissubfile){
      if ($thistitle eq "no title") {
        if (/<TITLE>(.*)/i) {
          $thistitle = $1;
          if (/<TITLE>(.*)<\/TITLE>/i) {
            $thistitle = $1;
          } else {
            $thistitle=chomp($thistitle);
            $multiple=1;
          }
        }
      } elsif ($multiple) {
        if (/(.*)<\/TITLE>/i) {
          $thistitle = $thistitle . " " . $1;
          $multiple=0;
        } else {
          $thistitle = $thistitle . chomp($_);
        }
      } elsif ($#thiscontent == -1) {
        if (/.*<BODY.*?>(.*)/i) {
          @thiscontent = ("$1");
        } 
      } elsif ($working){
        if (/(.*)<\/BODY>.*/i) {
          @thiscontent = (@thiscontent, "$1");
          $working=0;
        } else {
          @thiscontent = (@thiscontent, "$_");
        }
      }
    }

    #SAVE THE FORMATTED HTML FILE
    open(THISHANDLE, "> " . $savefile) || die "couldn't open " . $savefile;
    print THISHANDLE @pretitle;
    print THISHANDLE "$thistitle";
    print THISHANDLE @title2content;
    print THISHANDLE @thiscontent;
    print THISHANDLE @postcontent;
    close THISHANDLE;

  } #for each html file

}# for each template


#SUBROUTINES#
#-----------#
sub parsepatternfile {
  my(@filecontents) = @_;
  @patternlist=();
  foreach (@filecontents) {
    #if it starts with a # then skip it
    unless (/^#/) {
      #print $_;
      #find the string that starts and ends with _
      #skip following whitespace and find the remaining string
      if (/(_.+?_)\s*(.*)/) {
        @patternlist = (@patternlist, $1, $2);
      }
    }
  }
  
  return @patternlist;
}

sub parsetemplatefile {
  my(@filecontents) = @_;
  @templatelist=();
  foreach (@filecontents) {
    #if it starts with a # then skip it
    unless (/^#/) {
      #print $_;
      #find the string that starts and ends with _
      #skip following whitespace and find the remaining string
      if (/^(\S+)\s*(.*)/) {
        @templatelist = (@templatelist, $1, $2);
      }
    }
  }
  
  return @templatelist;
}

sub endslash {
  local($thispath) = $_[0];
  if (chop ne "/") { $thispath=$thispath."/"; };
  return $thispath;
}
