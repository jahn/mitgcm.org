#=======================================================================
#  Module for using Template-Toolkit for generating output
#    $Id: TemplateHTMLTemplate.pm,v 1.1 2001/12/08 21:49:14 whmoseley Exp $
#
#  This module probably does not automatically support all the features
#  of the swish.cgi script (such as selecting index files).  See
#  the TemplateToolkit.pm module for examples.
#
#=======================================================================
package TemplateHTMLTemplate;
use strict;

use HTML::Template;
use HTML::FillInForm;
use CGI ();

use vars '$Template';

use TemplateDefault;  # ugly hack


sub show_template {
    my ( $class, $template_params, $results ) = @_;

    my $cgi = $results->CGI;


    my $template = HTML::Template->new( %{$template_params->{options}} );

    my $params = {
        TITLE           => ($results->config('title') || 'Search Page'),
        QUERY_SIMPLE    => CGI::escapeHTML( $results->{query_simple} ),
        MESSAGE         => CGI::escapeHTML( $results->errstr ),
        QUERY_HREF      => $results->{query_href},
        MY_URL          => $cgi->script_name,

        HITS            => $results->navigation('hits'),
        FROM            => $results->navigation('from'),
        TO              => $results->navigation('to'),
        SHOWING         => $results->navigation('showing'),

        PAGES           => $results->navigation('pages'),
        NEXT            => $results->navigation('next'),
        NEXT_COUNT      => $results->navigation('next_count'),
        PREV            => $results->navigation('prev'),
        PREV_COUNT      => $results->navigation('prev_count'),
        

        RUN_TIME        => $results->header('run time') ||  'unknown',
        SEARCH_TIME     => $results->header('search time') ||  'unknown',
        MOD_PERL        => $ENV{MOD_PERL},
    };

    $params->{FILES} =  $results->results if $results->results;



    my $MapNames  = $results->config('name_labels') || {};
    my $Sorts     = $results->config('sorts');
    my $MetaNames = $results->config('metanames');


    # set a default
    if ( $MetaNames && !$cgi->param('metanames') ) {
        $cgi->param('metaname', $MetaNames->[0] );
    }

    $params->{SORTS} = [ map { { NAME => $_, LABEL => ($MapNames->{$_} || $_) } } @$Sorts ] if $Sorts;
    $params->{METANAMES} = [ map { { NAME => $_, LABEL => ($MapNames->{$_} || $_) } } @$MetaNames ] if $MetaNames;


    $template->param( $params );
    my $page = $template->output;


    my $fif = new HTML::FillInForm;

    print $cgi->header,
          '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">',
          $fif->fill(
            scalarref => \$page,
            fobject   => $cgi,
          );
}
   
1;

