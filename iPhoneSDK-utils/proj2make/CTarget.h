/************************************************************************
*																		*
* 					Jim Schimpf - Pandora Products 						*
*					Copyright 2006 Jim Schimpf							*
*  																		*
*						Target file list builder						*
*																		*
*		Module:CTarget.h												*
*																		*
*	Version		Date	Person  	Description							*
*-----------------------------------------------------------------------*
*	0.1		14-Oct-2006	J.Schimpf	Initial Version						*
*																		*
*	DESCRIPTION:													    *
*		This class will build the file list for a targeted build. It	*
*	will be used to find the PBXNativeTarget section and get the list	*
*	of source files for the particular target.  This will be used as	*
*	hash list against the full list to select only those for the target	*
*																		*
*																		*
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "CPBPROJ.h"

#pragma once

// File list structs

#define MAX_HASH		256			// Hash table size
typedef struct file_hash
{
	struct file_hash	*next;
	int hash;			// Hash for this file
	char *name;			// Name of file
} FILE_HASH;

#define NEW_FILE_HASH	((FILE_HASH *)calloc(1,sizeof(FILE_HASH)))

/************************************************************
*		OBJECT DEFS
************************************************************/

class CTarget {

	public:
            CTarget(	CpbxLexFile *p,
						bool dylib_flag,				// Are we building a dylib
						char *target_name);				// Start up
            ~CTarget(void);								// Tidy up

			bool parse();								// Find the records
			bool check(char *source);					// Check if in list
            
	private:
	
		bool find_list();								// Find the source file list for
														// target
		bool read_list();								// Read the source file list for 
														// target
	
		bool hash_insert( char *name );					// Put a file into the hash
		FILE_HASH *hash_find( char *name );				// Check for presence
		void hash_wipe();								// Clear the table
		int hash_gen( char *name );
			
		// *** Local vars
    
        CpbxLexFile *fbase;		// File lexer
		bool	no_target;		// If target NULL then don't bother with this work
								// Also check() will always return TRUE
								
		FILE_HASH *hash_table[MAX_HASH];
		
		// REF:JS10102006 Change here to help support dylibs
		// These are the extra link options needed to support them
		
		bool dylib_flag;	// Mark as TRUE if we are build a dylib
		
		// REF:JS12102006 Add target name to specify a build target
		
		char target[256];
		
};
