/* F90 program symbol/call tree lister */
%{
int Lno; char *currentFile;
%}

%union {
   int      LineNo;
   char    *symbolName;
}

%token <LineNo> OTHER    
%token <symbolName> NAME
%token <LineNo> SUBROUTINE
%token <LineNo> EXTERNAL
%token <LineNo> CALL
%token <LineNo> NAMELIST
%token <LineNo> FSLASH
%token <LineNo> CPP_IFDEF
%token <LineNo> CPP_IFNDEF
%token <LineNo> CPP_DEFINE
%token <LineNo> CPP_UNDEF 
%token <LineNo> CPP_ELIF  
%token <LineNo> CPP_IF
%type  <LineNo> call

%%

input:  /* empty */
      | input NAME                {noteVariable($2,Lno,currentFile);}
      | input SUBROUTINE NAME     {noteProcDef($3,Lno,currentFile); }
      | input call NAME           {noteProcCall($3,Lno,currentFile);}
      | input EXTERNAL   NAME     {noteExtDef($3,Lno,currentFile);  }
      | input NAMELIST FSLASH 
              NAME FSLASH         {noteInNameList($4,Lno,currentFile); }
      | input CPP_IFDEF           {noteInIfdef();                   }
      | input CPP_IFNDEF          {noteInIfdef();                   }
      | input CPP_DEFINE          {noteInIfdef();                   }
      | input CPP_UNDEF           {noteInIfdef();                   }
      | input CPP_ELIF            {noteInIfdef();                   }
      | input CPP_IF              {noteInIfdef();                   }
      | input FSLASH               
      | input OTHER                
      | input error                 { yyerrok; }
;
call: CALL                          { $$ = $1; }
;
%%
