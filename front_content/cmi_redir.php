<?php
/**
 *
 * $Header:  $
 *
 * Ed Hill
 *
 *   Simple "name"-based redirection for the CMI web pages.
 *
 *   This file belongs in the mitgcm.org DocumentRoot and it uses all
 *   the readable files in the directory DocumentRoot/redir_mappings
 *   to build the list.
 *
 */


$code = array();

$location = "./redir_mappings";
$all = opendir($location);
while ($file = readdir($all)) {
	if (!is_dir($location.'/'.$file)) {
		if (is_readable($location.'/'.$file)) {
			$handle = fopen($location.'/'.$file,"r");
			while ($line = fgets($handle, 4095)) {
				// list ($code, $url) = $mapinfo;
				// print $line;
				$t1 = strtok($line," \n\t");
				$t2 = strtok(" \n\t");
				if ($t1 && $t2) {
					$code[$t1] = $t2;
				}
			}
			fclose($handle);
		}
	}
}
closedir($all);

$arr=explode('/',$REQUEST_URI);
$url=$code[ $arr[2] ];

if ( strlen($url) > 4 ) {
	header ("Location: $url");
} else {
	print "<html><body>";
	print "Sorry, \"" . $arr[2] . "\" is an unknown redirection code.<br><br>";
        print "Please contact <a href=\"mailto:eh3@mit.edu\">eh3@mit.edu</a> if you ";
        print "need help with this site.";
	print "</body></html>";
}

?>
