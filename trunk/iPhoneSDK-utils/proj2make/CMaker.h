/************************************************************************
*																		*
* 					Jim Schimpf - Pandora Products 						*
*					Copyright 2002 Jim Schimpf							*
*  																		*
*		MAC MAKEFILE BUILDER FROM PROJECT BUILDER STRUCTURES			*
*																		*
*		Module:CMaker.h													*
*																		*
*	Version		Date	Person  	Description							*
*-----------------------------------------------------------------------*
*	0.1		30-Jul-2002	J.Schimpf	Initial Version						*
*	0.2		15-Mar-2003 J.Schimpf	Change to allow user defined		*
*									compilers							*
*	0.3		16-Mar-2003 J.Schimpf	Add code to put FRAMEWORKS into 	*
*									linker								*
*	0.4		16-Mar-2003 J.Schimpf	Add creator ID line to make file	*
*	0.5		27-Sep-2003 J.Schimpf	Add internal debug print			*
*	0.6		28-Sep-2003 J.Schimpf	Add find file node routine			*
*   0.7		23-Apr-2004 J.Schimpf   Add code to get path and file name  *
*									from raw path.						*
*	0.8		31-Jul-2005 J.Schimpf	Add compile and link options to		*
*									make file generation				*
*	0.9		10-Oct-2006	J.Schimpf	A support for DYLIB creation		*
*	1.0		12-Oct-2006	J.Schimpf	Add code to allow built to a target	*
*									name.								*
*	1.1		13-Oct-2006	J.Schimpf	Allow obj file lcn spec				*
*																		*
*	DESCRIPTION:													    *
*		This class takes as input a open write file (makefile) and		*
*	a PBXPROJ structure and uses this to build a make file for the proj	*
*																		*
*	Operations:															*
*			1) Find targets object										*
*			2) Get child and find that									*
*			3) Find PRODUCT_NAME from here and save						*
*			4) Find files and extract file list							*
*			5) Find includes and extract file list						*
*			6) Build simple makefile									*
*																		*
*																		*
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "CPBPROJ.h"
#include "CTarget.h"

#pragma once

// File list structs

typedef struct cmaker_kvp
{
	int state;						// 0 - Looking for key
									// 1 - Looking for =
									// 2 - Looking for value
									// 3 - Complete
	char key[MAX_TOK];
	char value[MAX_TOK];
} KEY_VALUE;

typedef enum {
				SOURCE_FILE,
				INCLUDE_FILE,
				EXE_FILE,
				LIB_FILE,
				ARCHIVE_FILE,
				DYLIB_FILE,			// REF:JS10102006 DYLIB creation
				UNK_TYPE
			} BFILE_TYPE;

typedef struct cmaker_line
{
	int state;						// 0 - Looking for {
									// 1 - Gathering KVP's
									// 2 - } seen
									// 3 - Complete
									
	bool file_name;					// We have a name
	bool file_type;					// We have a type
	BFILE_TYPE	type;				// Type of file
	
	char name[MAX_TOK];
	char path[MAX_TOK];
} LINE_VALUE;

typedef struct cmaker_dummy
{
    struct cmaker_dummy	*next;
    char *path;
    char *name;
	
	// REF:JS12062006 Add code here to hold file 
	// type
	
	bool file_type;
	BFILE_TYPE type;
} FILE_LIST;


/************************************************************
*		OBJECT DEFS
************************************************************/

class CMaker {

	public:
            CMaker(	CpbxLexFile *p,
					FILE *out, 
					bool debug_f,
					bool framework_f,
					char *target_name,
					char *obj_name );// Start it up
			bool parse();								// Parse the input file
			bool build( char *pgmid,
						char *compiler,
						char *cflags,
						char *ldflags, 
						char *bin_dir);
            ~CMaker(void);								// Tidy up
            
	private:

	
		void clrline();									// Init line
		void parseline(int type,char *token);			// Init line parser
		void gatherline();								// Look for file info
		void storeline();
	
		void clrkvp();									// Init kvp
		bool getkvp(int type,char *token);				// Return KVP's from list

		bool build_make(char *pgmid,char *compiler,char *cflags,char *ldflags, char *bin_dir);
		void linker_line( char *lnk_opts );				// REF:JS16032002 
		void build_print();						// Print file and include list
        
        void trim_files( FILE_LIST *list );
        void trim_file( FILE_LIST *list );
        FILE_LIST *trim_paths( FILE_LIST *list );
		
		char *getfname( char *path );
		char *getfpath( char *path );
		
		// *** Local vars
    
		char comp[256];			// Compiler name
        CpbxLexFile *base;			// Base of Descriptor struct
        FILE *make;				// Make file output pointer
		
		FILE_LIST *pgmid;		// EXE file data
        FILE_LIST *cfiles;		// Internal list of C files
        FILE_LIST *includes;	// Internal list of .h files
        FILE_LIST *libs;		// Internal list of libraries
		
		// These store the path and name while we are in work
		
		bool debug;				// Internal debug flag
		bool proj_ok;			// If FALSE abort generation
		bool frameworks;		// If TRUE generate frameworks

		// KVP state & data
		KEY_VALUE	kvp;		// Current KVP
		/// Line state & data
		LINE_VALUE	line;
		// File data
		FILE_LIST	file;		// File element state
		char namestore[256];
		char pathstore[256];
		
		// REF:JS10102006 Change here to help support dylibs
		// These are the extra link options needed to support them
		
		bool dylib_flag;	// Mark as TRUE if we are build a dylib
		
		// REF:JS12102006 Add target name to specify a build target
		
		char *target;
		
		// REF:JS13102006 Add lcn for OBJ files
		
		char *obj;
		
		// REF:JS14102006 Used to Target builds

		CTarget *target_files;
};
