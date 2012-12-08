/************************************************************************
*																		*
*	 Pandora Products - Support routines for OS X -> LINUX link			*
*		Copyright 2002 J.Schimpf Pandora Products						*
*																		*
*			proj2make OS X pbxproj -> MAKE file converter				*
*																		*
*		Module:cutil.c													*
*																		*
*	Version	   Date		Person  	Description							*
*-----------------------------------------------------------------------*
*		0.1		27-Jul-2002 	J.Schimpf		Initial Version			*
*		0.2		31-Jul-2002		J.Schimpf		Add in version display	*
*		0.3		 2-Aug-2002 	J.Schimpf		Change to use proj file	*
*												as directory and will	*
*												the project.pbxproj 	*
*												inside					*
*		0.4		15-Mar-2003		J.Schimpf		Convert to work with	*
*												new project builder 	*
*												files					*
*		0.5		15-Mar-2003		J.Schimpf		Add user input for		*
*												compiler name			*
*		0.6		16-Mar-2003 	J.Schimpf		Change to extract 		*
*												frameworks				*
*		0.7		16-Mar-2003		J.Schimpf		Add program ID line on	*
*												make files				*
*		0.8		21-Jun-2003		J.Schimpf		Change in hash to		*
*												to unsigned as per Marc	*
*												Boucher					*
*		0.9		 2-Mar-2004		J.Schimpf		Convert to X code ver   *
*		0.91	23-Apr-2004		J.Schimpf		Change to handle very   *
*												simple projects			*
*		1.00	 8-Jun-2005		J.Schimpf		Convert to work with	*
*												XCode2.1				*
*		1.10	28-Jul-2005		J.Schimpf		Change makefile output	*
*												(see CMaker.c) to make	*
*												Solaris happy			*
*		1.2		31-Jul-2005		J.Schimpf		Add LINKER & CC options	*
*												commmand items			*
*		1.3		12-Mar-2006		J.Schimpf		Add framework flag		*
*		1.3A	05-May-2006		F.Wagner		Add generation of		*
*												"clean" and "install"	*
*												section.				*
*												Add BIN_DIR option		*
*		1.4		12-Oct-2006		J.Schimpf		Specify target in multi	*
*												target projects			*
*		1.5		13-Oct-2006		J.Schimpf		Add object file lcn		*
*																		*
*	DESCRIPTION:														*
*		This will take a pbxproj file extracted from a ProjectBuilder	*
*	project and turn it into a simple make file	which can be used in 	*
*	Linux																*
*	SYNTAX:	proj2make -i <name>.xcodeproj [-o <makefile name>] [-v]		*
*				-i <name>.xcodeproj	- Project file NO DEFAULT			*
*				[-o <makefile>]		- Output makefile DEF = makefile	*
*				[-cc compiler]		- User compiler selection			*
*										DEF = /usr/bin/cc				*
*				[-cflags <options>]	- CC options						*
*				[-ldflags <options]- Link options						*
*				[-bin_dir target_dir]- target dir for install			*
*				[-no_framwork]		- Supress frameworks in makefile	*
*				[-t Target]			- Specify build target for project	*
*									  with multiple targets				*
*				[-obj OBJ location	- Store .o files here				*
*									  DEF = ./							*
*				[-v]				- Show version and HELP				*
*				[-debug]			- Show debug output					*
*																		*
************************************************************************/

/******* INCLUDE FILES ******/
#include <stdio.h>
#include <stdio.h>
#include "base.h"
#include "cutil.h"
#include "CpbxLexFile.h"
//#include "CPBPROJ.h"
#include "CMaker.h"

#include "helper.h"

//for stat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/******* Local Definitions ******/

#define DEF_OUTPUT_FILE	"Makefile"
#define DEF_COMPILER	"ios-clang"
#define DEF_CCOPT		""
#define DEF_LINKOPT		""
#define DEF_BINDIR		""
#define DEF_OBJDIR		"."

/******* Local Functions ******/

static void dump_pgmdir(void);

//*****************************************
int main(int argc,char **argv)
{
	FILE *out;
	char *ptr;
	CpbxLexFile *in;
	char msg[256];
	//CPbxProj *proj;
    CMaker *make;
	char *compiler;			// REF:JS15032003
	char *cflags;			// REF:JS31072005
	char *lnk_opt;			// REF:JS31072005
	char *bin_dir;
    char inproj[256];
	char pgmid[256];		// REF:JS16032003
	bool debug = false;
	bool framework_f = true;// REF:JS12032006
	char *target;			// REF:JS12102006 
	char *obj;				// REF:JS13102006

	// (1) Parse the command line and open the input and output files
	//     Failure here will cause an early abort 

	if( flag_srch(argc,argv,"-v",FALSE,&ptr))		// HELP
	{
		dump_pgmdir();
        error_exit("***");
	}

	if( flag_srch(argc,argv,"-debug",FALSE,&ptr))	// DEBUG
	{
		debug = true;
	}

	if( flag_srch(argc,argv,"-no_framework",FALSE,&ptr))// FRAMEWORKS ?
	{
		framework_f = false;
	}

	if( !flag_srch(argc,argv,"-i",TRUE,&ptr))		// Input file
	{
		ptr = NULL;
	}

	if( !flag_srch(argc,argv,"-cc",TRUE,&compiler))	// Compiler
	{
		compiler = DEF_COMPILER;
	}

	if( !flag_srch(argc,argv,"-cflags",TRUE,&cflags))	// Compiler options
	{
		cflags = DEF_CCOPT;
	}

	if( !flag_srch(argc,argv,"-ldflags",TRUE,&lnk_opt))	// Link options
	{
		lnk_opt = DEF_LINKOPT;
	}
	
	if( !flag_srch(argc,argv,"-bin_dir",TRUE,&bin_dir))	// target directory for 'install'
	{
		bin_dir = DEF_BINDIR;
	}

	if( !flag_srch(argc,argv,"-t",TRUE,&target))		// target object
	{
		target = NULL;		// Set for NO target
	}

	if( !flag_srch(argc,argv,"-obj",TRUE,&obj))		// target object
	{
		obj = DEF_OBJDIR;	// Set for DEFAULT OBJ directory
	}
	
	// (0) Check for trailing slash on bin_dir
	if(*bin_dir != '\0') {
		if( bin_dir[strlen(bin_dir) - 1] != '/' ) {
			bin_dir = strcat( bin_dir, "/" );
		}
	}
	   
	// (1a) Try to open the input file
	// NOTE: The input file is the project.pbxproj
	// INSIDE the project (which is a directory)
	
	in = new CpbxLexFile;
	in->init();
    if( ptr != NULL )
    {
        sprintf(inproj,"%s/project.pbxproj",ptr);
    }
    else
    {
        //cjacker
//        strcpy(inproj,"none");		// Should have been strcat...
/*        string xcodeproj = find_dir_end_with(".xcodeproj");
        if(!xcodeproj.empty())
            sprintf(inproj,"%s/project.pbxproj",xcodeproj.c_str()); 
        else*/
            strcpy(inproj,"none");		// Should have been strcat...
        
    }   
	if( !in->fopen(inproj,"r") )
	{
		dump_pgmdir();
		sprintf(msg, "*** CANNOT OPEN [%s] ****",inproj);
		error_exit(msg);
	}

	// (1b) Try to create the output file
	
	if( !flag_srch(argc,argv,"-o",TRUE,&ptr))		// Output file
	{
		ptr = DEF_OUTPUT_FILE;
	}
	
	out = fopen( ptr,"w");
	if( out == NULL )
	{
		dump_pgmdir();
		sprintf(msg,"*** CANNOT CREATE [%s] ****",ptr);
		error_exit(msg);
	}
	
	make = new CMaker( in, out,debug,framework_f,target,obj );
	sprintf(pgmid,"Xcode -> Makefile Ver: %s %s",__DATE__,__TIME__);
	if( make->parse() )
	{
		make->build(pgmid,compiler,cflags,lnk_opt, bin_dir);
	}
	else
	{
		printf("** Parse failed **\n");
	}
	delete make;
		
	// (n) All done, close all objects and files
	
	//delete proj;
	delete in;
	fclose(out);

	
	return( 0 );
}

/***********************************************************************
*
* void dump_pgmdir( void  )	- Show pgm version and directions
*
*	INPUT:	NONE
*
*	OUTPUT:	NONE
*		Print out program directions to stdout
*
***********************************************************************/

void dump_pgmdir( void )
{
	printf("proj2make : iOS Xcode Project -> Makefile converter\n\n");
	printf("\tSyntax proj2make -i <Project.xcodeproj> [-o <fname>] [-cc <c compiler>] [-v]\n");
	printf("\t\t-i <fname>\tInput Project (xcodeproj) DEF NONE\n");
	printf("\t\t-o <fname>\tOutput File (Makefile)   DEF Makefile\n");
	printf("\t\t-cc <compiler>\tCompiler used DEF ios-clang\n");
	printf("\t\t-cflags <compiler optons>\tCompiler options DEF NONE\n");
	printf("\t\t-bin_dir <install directory>\tTarget directory for\n");
	printf("\t\t\t\t\t\t\"make install\" DEF NONE\n");	
	printf("\t\t-obj <Object directory>\tObject file directory DEF = \".\"\n");
	printf("\t\t-ldflags <link options>\tLink stage options DEF NONE\n");
	printf("\t\t-no_framework\tSupress -framework lines DEF Show frameworks\n");
	printf("\t\t-t <target>\tSpecify target name in multi-target Project files\n");
	printf("\t\t-debug\t\tTurn on Debug output\n");
	printf("\t\t-v\t\tShow this help\n");
	printf("\n");
}
