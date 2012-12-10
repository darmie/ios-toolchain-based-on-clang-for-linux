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
*	0.2		25-Oct-2006	J.Schimpf	Change creator to set no_target		*
*									flag to TRUE on entrance and check	*
*									for passed in target value for NULL	*
*									if so we don't have a defined target*
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
#include <string.h>
#include "CPBPROJ.h"
#include "CTarget.h"

#pragma mark -- CONSTRUCTORS/DESTRUCTORS --

/*******************************************************************************
*
* CTarget( CpbxLexFile *p,char *dylib_flag,char *target_name )
*
*   INPUT:	p	- Lex object hook to project file
*			dflag	- TRUE if dylib (strip name see below)
*			target_name	- Name of target, NULL for no target
*
*   OUTPUT: Create object
*
*******************************************************************************/

CTarget::CTarget( CpbxLexFile *p,bool dflag,char *target_name )
{	
	int i,n;
	
	// (1) Clear hash table
	
	for( i=0; i<MAX_HASH; i++ )
	{
		hash_table[i] = NULL;
	}
	
	// Lock in locals
	
	fbase = p;
	dylib_flag = dflag;
	no_target = true;			// We have no specified target
								// Set here and check below
	
	// Now if target is NULL set the flag no_target and bail
	
	if( target_name != NULL )
	{
		strncpy(target,target_name,256);
		no_target = false;		// We are running
		
		if( dylib_flag )
		{
			// This implies the input name if of the form lib<NAME>.dylib
			// We want to strip off the lib and end the name at the .
			
			n = strlen(target);
			for( i=3; i<n; i++ )
			{
				target[i-3] = target[i];		// Move up over the lib
			}
			target[i-3] = '\0';		// close it
			
			n = strlen(target);
			for( i=0; i<n; i++ )
			{
				switch( target[i] )
				{
					case '.':		// End it at the period
									target[i] = '\0';
									break;
					default:		target[i] = target[i];	// do nothing
									break;
				}
			}
		}
	}
}

/*******************************************************************************
*
* ~CTarget(  )
*
*   INPUT:	NONE
*
*   OUTPUT: Tidy up
*
*******************************************************************************/

CTarget::~CTarget(  )
{
	// Only do this if we had a target
	
	if( no_target == false )
	{
		this->hash_wipe();
	}
}

#pragma mark -- PUBLIC API --

/*******************************************************************************
*
*	bool parse()
*
*   INPUT:	NONE
*
*   OUTPUT: Find the file list for this target
*
*******************************************************************************/

bool CTarget::parse()
{
	bool rtnval = false;		// Set for failure

	if( !no_target )		// If we have a target then do parse
	{
		if( find_list() )
		{
			if( read_list() )
			{
				rtnval = true;		// Success
			}
		}
	}
	else
	{
		rtnval = true;				// Success, no target
	}

	return( rtnval );
}

/*******************************************************************************
*
*	bool check(char *source)
*
*   INPUT:	source	- Check this source file
*
*   OUTPUT: TRUE if source file is part of set, FALSE if not
*
*******************************************************************************/

bool CTarget::check(char *source)
{
	bool rtnval = true;		// Set for success
	FILE_HASH *fh;
	
	if( !no_target )		// If we have a target then do the check
	{
		fh = this->hash_find(source);
		if( fh == NULL )
		{
			rtnval = false;	// NOT FOUND
		}
	}
	
	return( rtnval );
}

#pragma mark -- PRIVATE API --

/*******************************************************************************
*
* bool find_list( )
*
*   INPUT:	NONE
*
*   OUTPUT: TRUE if PBXNativeTarget section found & target name acquired
*
*******************************************************************************/

bool CTarget::find_list( )
{
	bool rtnval = false;		// Set for failure
	char token[MAX_TOK+1];	
	int state = 0;
	bool run = true;
	int type;
	char build_num[MAX_TOK+1];

	// (1) Search for PBXFileReference section
	
	while( run )
	{
		type = fbase->get_token(token);
		if( type == TOK_BUF_EOF )
			break;
			
		switch( state )
		{
			// Look for PBXNativeTarget 
			case 0:			if( strcmp(token,"PBXNativeTarget") == 0 )
								state = 1;
							else
								state = 0;		// Recycle
							break;
							
			// Now look for target # then name
			case 1:		if( strlen(token) > 10 )
							state = 2;
						break;
					
			// Now find /* then name and try to match
			case 2:		if( strcmp(token,"/") == 0 )
							state = 3;
						else
							state = 1;
						break;
						
			case 3:		if( strcmp(token,"*") == 0 )
							state = 4;
						else
							state = 1;
						break;
						
			case 4:		if( strcmp(token,target) == 0 )
							state = 5;		// Target found
						else
							state = 15;		// Skip to end of section
						break;
						
			// Skip to end of this section and when hit start searching on next section
			case 15:	if( strcmp(token,"}") == 0 )
							state = 1;		// Look for next section
						break;
							
			// Look for buildPhases
			case 5:			if( strcmp(token,"buildPhases") == 0 )
								state = 6;
							break;
							
			// Look for =
			case 6:			if( strcmp(token,"=") == 0 )
								state = 7;
							else
								state = 0;
							break;

			// Look for (
			case 7:			if( strcmp(token,"(") == 0 )
								state = 8;
							else
								state = 1;
							break;
				
			// Get this build # thing
			case 8:			if( strlen(token) > 10 )
							{
								strcpy(build_num,token);
								state = 9;			// Advance to next
							}
							break;
							
			case 9:			if( strcmp(token,"/") == 0 )
								state = 10;
							else
								state = 1;
							break;
							
			case 10:			if( strcmp(token,"*") == 0 )
								state = 11;
							else
								state = 1;
							break;
							
			case 11:		if( strcmp(token,"Sources") == 0 )
							{
								// Found it, so exit this loop 
								
								run = false;
								state = -1;		// Show we have found it !!!
							}
							else
							{
								state = 8;		// Go back for build #
							}
							break;
							
			default:		printf("Unk Tok in find source list # [%s]\n",token);
							break;
							
		}
	}
	
	// (2) Now search for the source code list
	// First loop through and find this build_num tag
	// Then find files = ( and the next stuff is done in the
	// list read program
	
	if( state < 0 )
	{
		state = 0;
		run = true;
		while( run )
		{
			type = fbase->get_token(token);
			if( type == TOK_BUF_EOF )
				break;
				
			switch( state )
			{
				// Look for build_num target 
				case 0:			if( strcmp(token,build_num) == 0 )
									state = 1;
								else
									state = 0;		// Recycle
								break;
								
				// Look for files
				case 1:			if( strcmp(token,"files") == 0 )
									state = 2;
								else
									state = 1;
								break;
						
				// Look for =
				case 2:			if( strcmp(token,"=") == 0 )
									state = 3;
								else
									state = 1;
								break;
								
				case 3:			if( strcmp(token,"(") == 0 )
								{
									state = -1;
									run = false;		// FOUND !!!
									rtnval = true;
								}
								else
									state = 1;
								break;
			}
		}
	}
	
	return( rtnval );
}
	
/*******************************************************************************
*
* bool read_list( )
*
*   INPUT:	NONE
*			We are at the start of the file list for this target
*
*   OUTPUT: TRUE if hash table is built, FALSE if not
*
*******************************************************************************/

bool CTarget::read_list( )
{
	bool rtnval = false;		// Set for failure
	char token[MAX_TOK+1];	
	int state = 0;
	bool run = true;
	int type;

	// (1) Search for files in the list
	
	while( run )
	{
		type = fbase->get_token(token);
		if( type == TOK_BUF_EOF )
			break;
			
		if( strcmp(token,")") == 0 )
		{
			run = false;
		}
		else
		{
			// Read in the list data as
			// <######.....> /* <File Name> */
			//      ^        ^       ^      ^
			//      |        |       |      +-> Pitch
			//      |        |       +-> Put in list
			//      |        +-> Pitch
			//      +-> Pitch
			//
			switch( state )
			{
				// Look for <REF #> 
				// Get this build # thing
				case 0:			if( strlen(token) > 10 )
								{
									state = 2;			// Advance to next
								}
								break;
				
								// Skip over /*				
				case 2:			if( strcmp(token,"/") == 0 )
									state = 3;
								else
									state = 0;
								break;
								
				case 3:			if( strcmp(token,"*") == 0 )
									state = 4;
								else
									state = 0;
								break;
								
								// We have the file name, put this into the
								// list and go back to 0
				case 4:			
								if( this->hash_insert(token) )
								{
									rtnval = true;		// We have put something in table
									state = 0;
								}
								else
								{
									run = false;
									rtnval = false;		// FAILURE
								}
								break;
								
				default:		printf("Unk Tok in find source list # [%s]\n",token);
								break;
								
			}
		}
	}

return( rtnval );
}
	

/*******************************************************************************
*
* bool hash_insert( char *name )
*
*   INPUT:	name	- String name to insert
*			If name = name.x  store only name
*
*   OUTPUT: TRUE if done, FALSE if not
*
*******************************************************************************/

bool CTarget::hash_insert( char *name )
{
	FILE_HASH *fh;
	FILE_HASH *base;
	bool rtnval = false;			// Set for fail
	int n = strlen(name);
	int i;
	
	// (0) Strip the .x from the name if any
	
	for( i=0; i<n; i++)
	{
		if( name[i] == '.' )
		{
			name[i] = '\0';
			break;
		}
	}
	
	// (1) Build the store element and fill it out
	
	fh = NEW_FILE_HASH;
	if( fh != NULL )
	{
		// Build the name store
		
		n += 1;		// Make room for NUL
		fh->name = (char *)calloc(1,n);
		if( fh->name != NULL )
		{
			strcpy(fh->name,name);
			
			// Build the hash for the name
			
			fh->hash = this->hash_gen(name);
			
			// (2) Insert this into the hash list
			
			base = hash_table[fh->hash];
			
			fh->next = base;
			hash_table[fh->hash] = fh;
			rtnval = true;				// SUCCESS !
		}
		else
		{
			// FAILURE, delete the struct
			
			free( fh );
		}
	}
	
	return( rtnval );
}

/*******************************************************************************
*
* void hash_wipe(  )
*
*   INPUT:	NONE
*
*   OUTPUT: NONE
*			Remove the hash table
*
*******************************************************************************/

void CTarget::hash_wipe(  )
{
	FILE_HASH *fh;
	FILE_HASH *base;
	int i;
	
	// (1) Walk through the table and find all chains
	
	for( i=0; i<MAX_HASH; i++ )
	{
		if( hash_table[i] != NULL )
		{
			base = hash_table[i];
			
			// (2) Now delete the elements in this list
			
			while( base != NULL )
			{
				fh = base->next;		// Get next
				
				// Free element
				
				if( base->name != NULL )
					free( base->name );
				free( base );
				
				base = fh;
			}
		}
	}
}

/*******************************************************************************
*
* FILE_HASH *hash_find( char *name )
*
*   INPUT:	name	- String name to find
*
*   OUTPUT: Return pointer to FILE_HASH or NULL if not found
*
*******************************************************************************/

FILE_HASH *CTarget::hash_find( char *name )
{
	int hash;
	FILE_HASH *fh;
	
	// (1) Generate the HASH and see if we have a hit
	
	hash = this->hash_gen( name );
	fh = hash_table[hash];
	
	// (2) Now look for a match in the list
	
	while( fh != NULL )
	{
		if( strcmp(name,fh->name) == 0 )
			break;
			
		fh = fh->next;
	}
	
	// (3) At this point either we have found it, fh <> NULL or
	// not fh == NULL so just return the result and let the upper 
	// level decide
	
	return( fh );
}

/*******************************************************************************
*
* int hash_gen( char *name )
*
*   INPUT:	name	- String name to hash
*
*   OUTPUT: Return HASH value of name or 0 if name length = 0
*
*  Modified version of the FNV Hash with lazy mod conversions
*	(http://www.isthe.com/chongo/tech/comp/fnv/#FNV-1)
*
*******************************************************************************/

int CTarget::hash_gen( char *name )
{
	unsigned long hash = (unsigned long)(2166136261);	// 32 bit Starting OFFSET
	int i,n;
	int hval = 0;
	
	// (1) Get length of input string
	
	n = strlen( name );
	if( n > 0 )
	{
		for( i=0; i<n; i++ )
		{
			hash = hash ^ (0xff & name[i]);
			hash *= 16777619;				// 32 bit Hash constant
		}
		
		hval =  (hash % MAX_HASH);
	}
		
		
	return( hval );
}
