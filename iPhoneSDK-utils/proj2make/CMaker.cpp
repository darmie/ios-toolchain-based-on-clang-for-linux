/************************************************************************
*																		*
* 					Jim Schimpf - Pandora Products 						*
*					Copyright 2002-2003 Jim Schimpf						*
*  																		*
*		MAC MAKEFILE BUILDER FROM PROJECT BUILDER STRUCTURES			*
*																		*
*		Module:CMaker.h													*
*																		*
*	Version		Date	Person  	Description							*
*-----------------------------------------------------------------------*
*	0.1		30-Jul-2002	J.Schimpf	Initial Version						*
*	0.2		31-Jul-2002 J.Schimpf	Got makefile generation to work		*
*	0.3		31-Jul-2002 J.Schimpf	Change trim_paths to remove dup		*
*									path names							*
*	0.4		 4-Aug-2002 J.Schimpf	Add macros to make file				*
*	0.5		21-Aug-2002 J.Schimpf	REF:JS21082002 the link Macro had	*
*									a lower case i and not I as used	*
*									the references						*
*	0.6		15-Mar-2003	J.Schimpf	REF:JS15032003 Change to look for	*
*									isd not name for first item in		*
*									search.  New PB files don't have	*
*									name. (This also still works on		*
*									older files)						*
*	0.7		15-Mar-2003	J.Schimpf	REF:JS15032002 Change to allow user	*
*									defined compiler names				*
*	0.8		16-Mar-2003 J.Schimpf	Add code to put FRAMEWORKS into 	*
*									linker								*
*	0.9		16-Mar-2003 J.Schimpf	Add creator ID line to make file	*
*	1.0		27-Sep-2003	J.Schimpf	Format changes in MAKE file			*
*   1.1		 6-Mar-2004 J.Schimpf   More format changes					*
*   1.2		23-Apr-2004 J.Schimpf   Change to handle case when no path  *
*									key with file name (use ./)			*
*	1.3		 9-Jun-2005	J.Schimpf	Convert to XCode2.1 new fmt			*
*	1.4		28-Jul-2005 J.Schimpf	Remove blnk line between LINK		*
*									list and link command				*
*	1.5		28-Jul-2005	J.Schimpf	Remove extra \ on LDFLAGS line	*
*	1.6		31-Jul-2005 J.Schimpf	Add compile and link options to		*
*									make file generation				*
*	1.7		22-Aug-2005 J.Schimpf	Change linker_line() to work when	*
*									there are frameworks and when they	*
*									are not.  1.6 failed when			*
*									frameworks were present				*
*	1.8		12-Mar-2006	J.Schimpf	Add code in here to classify archive*
*									files and add to link list			*
*	1.9		12-Mar-2006 J.Schimpf	Add flag to enable/disable framework*
*									store in make file					*
*	2.0		22-Mar-2006 J.Schimpf	Check for good ending conditions	*
*									and fail if not in parse()			*
*	2.1		22-Mar-2006	J.Schimpf	Change output LINK line to put out	*
*									libraries with extensions stripped	*
*									and -L to say where to look			*
*	2.2		 5-May-2006	F.Wagner	Add generation of					*
*									"clean" and "install" sections.		*
*	2.3		10-Oct-2006	J.Schimpf	A support for DYLIB creation		*
*	2.4		12-Oct-2006	J.Schimpf	Add code to allow built to a target	*
*									name.								*
*	2.5		16-Oct-2006	J.Schimpf	Changes to only build target files	*
*									not all								*
*	2.6		17-Oct-2006	J.Schimpf	Only put out Apple link commands if	*
*									no other links cmds are input on	*
*									dylib								*
*	2.7		 3-Nov-2006	J.Schimpf	Change to remove extra /n between	*
*									obj list and main C compile			*
*	2.8		 7-Dec-2006	J.Schimpf	Change parse() to check for non		*
*									null target before looking at		*
*									string								*
*																		*
*	DESCRIPTION:													    *
*		This class takes as input a open write file (makefile) and		*
*	a PBXPROJ structure and uses this to build a make file for the proj	*
*			(1) Scan to find PBXFileReference section					*
*				(a) Read {....} section and pull out KVP's				*
*				(b) Store .c files & paths in FILES						*
*				(c) Store .h files & paths in INCLUDES					*
*				(d) Find "compiled.mach-o.executable" type as target	*
*					or "compiled.mach-o.dylib" for DYLIB target			*
*				(e) Reject all others									*
*				When PBXFileReference seen exit							*
*																		*
*	Operations:															*
*																		*
*																		*
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "CPBPROJ.h"
#include "CMaker.h"


#include <string>
using namespace std;

#include "helper.h"

#pragma mark -- CREATE/DELETE --
/***********************************************************************
					*** CREATE/DELETE CMAKER ***
***********************************************************************/
/***********************************************************************
*
*  CMaker()	- Init for makefile creation
*
*	INPUT:	p	- PBXPROJ class with parsed file
*			fp	- Open to blank makefile
*			debug_f	- Debug flag 
*			framework_f	- Framework flag (true to put in make file, false if not)
*			target_name	- Target name, NULL => any target
*			obj_name	- Object file name
*
*	OUTPUT:	NONE
*			Init and store paramters for run 
*
***********************************************************************/

CMaker::CMaker(CpbxLexFile *p,
				FILE *out,
				bool debug_f,
				bool framework_f,
				char *target_name,
				char *obj_name)
{
    base = p;
    make = out;
    
    cfiles = NULL;
    includes = NULL;
	this->clrkvp();
	this->clrline();
	
	debug = debug_f;
	
	proj_ok = true;			// Set as nominally ok
	frameworks = framework_f;	// Set internal flag
	
	dylib_flag = false;		// Set for NO dylib
	
	target = target_name;	// Add Target name or NULL for no target
	
	obj = obj_name;		// Set .o file directory
	
}

/***********************************************************************
*
*  ~CMaker()	- Destructor for class
*
*	INPUT:	NONE 
*
*	OUTPUT:	NONE
*			Destroy locally created structures
*
***********************************************************************/

CMaker::~CMaker(void)
{
    FILE_LIST *fl;
    
    // (1) See if there is stuff in the file lists
    // And delete it
    
    if( cfiles != NULL )
    {
        while( cfiles != NULL )
        {
            fl = cfiles->next;
            if( cfiles->name )
                free( cfiles->name );
            if( cfiles->path )
                free( cfiles->path );
            free( cfiles );
            cfiles = fl;
        }
    }

   if( includes != NULL )
    {
        while( includes != NULL )
        {
            fl = includes->next;
            if( includes->name )
                free( includes->name );
            if( includes->path )
                free( includes->path );
            free( includes );
            includes = fl;
        }
    } 
	
	// Remove the target file object
	
	delete target_files;
}

#pragma mark -- PUBLIC --
/***********************************************************************
*
*  bool parse(void)	- Read the input file and parse it
*
*	INPUT:	NONE 
*
*	OUTPUT:	TRUE if parse ok FALSE if not
*
*	22-Mar-2006 Check for good ending conditions and return FALSE if not
*
***********************************************************************/

bool CMaker::parse()
{
	char token[MAX_TOK+1];
	int type;
	int state  = 0;
	bool run = true;
	bool rtnval = false;		// Assume failure
	
	// (1) Search for PBXFileReferencee section
	
	while( run )
	{
		type = base->get_token(token);
		if( type == TOK_BUF_EOF )
			break;
			
		switch( state )
		{
			//****** STARTUP ACTIONS
			// Look for Start
			case 0:			if( strcmp(token,"Begin") == 0 )
								state = 1;
							break;
					
			// Look for PBXFileReference 
			case 1:			if( strcmp(token,"PBXFileReference") == 0 )
								state = 2;
							else
								state = 0;		// Recycle
							break;
							
			// Look for section
			case 2:			if( strcmp(token,"section") == 0 )
								state = 3;
							else
								state = 0;
							break;
			
			// **** RUN ACTIONS - NOTE: Parse but look for section end
			// In section Look for ending stuff
			case 3:			if( strcmp(token,"End") == 0 )
								state = 4;
							// Also parse this anyway...
							this->parseline(type,token);
							break;
				
			// In section Look for ending stuff
			case 4:			if( strcmp(token,"PBXFileReference") == 0 )
								state = 5;
							else
								state = 4;
							// Also parse this anyway...
							this->parseline(type,token);
							break;
							
			// In section Look for ending stuff
			case 5:			if( strcmp(token,"section") == 0 )
							{
								run = false;		// DONE with parse
								printf("\n-- Parse Done --\n");
                                printf(RED "\nNOTE:\n");
                                printf(RED "1.Info.plist may contains some macros\n");
                                printf(RED "  You need edit it by yourself\n");
                                printf(RED "2.Application use xib or storyboard can be compiled\n");
                                printf(RED "  But will not works in devices\n");
                                printf(RED "3.Resources in some dir may not be included\n");
                                printf(RED "  Please check it.\n");
                                printf(RED "\nAnd\n");
                                printf(RED "The Makefile may not work:-)\n\n");
                                printf(NONE);
							}
							else
								state = 4;
							// Also parse this anyway
							this->parseline(type,token);
							break;
		}
	}
	
	// REF:JS22032006
	// Look for good ending conditions
	
	if( state == 5 )
	{
		// OK we finished stuff in the file
		// Now see if we have the parts
		
		if( pgmid != NULL )
		{
			if( cfiles != NULL )
			{
				rtnval = true;			// We got all we need for makefile
				
				// Build a target file thing & parse here, if necessary
				
				// REF:JS10112006 Change here to look for dylib as target and
				// set the dylib flag ALWAYS
				// REF:JS07122006 Check for existance of target before checking
				// for dylib 
				
				if( target != NULL && strstr(target,".dylib") != NULL )
					dylib_flag = true;
					
				target_files = new CTarget( base,dylib_flag,target );
				if( !target_files->parse() )
				{
					printf("*** ERROR No C/C++ target list found\n");
					pgmid = NULL;
					rtnval = false;
				}
			}
			else
			{
				printf("*** ERROR no C/C++ files found\n");
				rtnval = false;
			}
		}
		else
		{
			printf("*** ERROR no executiable target found\n");
			rtnval = false;
		}
	}

	return( rtnval );
}

/***********************************************************************
*
*  bool build(char *compiler)	- Build the make file
*
*	INPUT:	pgmid		- Creator program string
*			compiler	- Name of compiler used 
*			cflags		- Compiler options
*			ldflags	- Link options
*
*	OUTPUT:	TRUE if run ok FALSE if not
*
***********************************************************************/

bool CMaker::build(char *pgmid,char *compiler,char *cflags,char *ldflags, char *bin_dir)
{
	
	if( debug )
		this->build_print();
    
    // (5) We have all the parts build the make file
    
	if( proj_ok )
	{
		if( this->build_make(pgmid,compiler,cflags,ldflags, bin_dir) )
			return( true );
	}
    else
	{
		printf("** BAD PROJECT FILE FIX AND TRY AGAIN\n");
        return( false );
	}
	
	return( false );		// Fix warning
}

#pragma mark -- PRIVATE --
/***********************************************************************
					*** PRIVATE FUNCTIONS ***
***********************************************************************/
#pragma mark -- PARSE --
/***********************************************************************
*
*   void clrline()	- Init LINE
*
*	INPUT:	NONE
*
*	OUTPUT:	NONE
*				LINE struct readyed for next search
*
***********************************************************************/

void CMaker::clrline()
{
	line.state = 0;
	line.name[0] = '\0';
	line.path[0] = '\0';
	line.file_name = false;
	line.file_type = false;
}
/***********************************************************************
*
*   void parseline(int type,char *token)	- Handle { KVP's set }
*
*	INPUT:	type	- Type of token
*			value	- Value of token
*
*	OUTPUT:	NONE
*				Trigger internal states between { & }
*
***********************************************************************/

void CMaker::parseline(int type,char *token)
{
	switch( line.state )
	{
		case 0:				// Look for leading {
							if( type == P_TOK_OBJ_START )
							{
								line.state = 1;
								this->clrkvp();		// Init for KVP's
							}
							break;
								
		case 1:				if( type != P_TOK_OBJ_END )
							{
								if( getkvp(type,token ) )
								{
									this->gatherline();
									clrkvp();
								}
							}
							else
							{
								this->storeline();		// Determine if we should store
								
								// EOL hit, go back to start
								this->clrline();
							}
							break;
	}
}

/***********************************************************************
*
*   void gatherline()	- Put current kvp info in line
*
*	INPUT:	NONE
*				Use current line struct
*
*	OUTPUT:	NONE
*				Update current line struct from current kvp
*	12-Mar-2006 Add code in here to classify archives 
*	10-Oct-2006	J.Schimpf	A support for DYLIB creation
*
***********************************************************************/

void CMaker::gatherline()
{
	char *keys[] = {
						"path",
						"name",
						"lastKnownFileType",
						"explicitFileType",
						"sourceTree",
					};
	int klen = (sizeof(keys)/sizeof(char *));
					
	char *values[] = {
						"sourcecode.c.c",
						"sourcecode.cpp.cpp",
						"sourcecode.c.h",
						"wrapper.application",
						"wrapper.framework",
						"archive",				// REF:JS12032006
						"archive.ar",
						"compiled.mach-o.dylib",	// REF:JS10102006 Add dynamic lib
                        "sourcecode.c.objc",  // REF:JS07092008 Add obj c
						"compiled.mach-o.executable"
					};
					
	int vlen = (sizeof(values)/sizeof(char *));

	char *sources[] = {
						"SOURCE_ROOT",
						"<absolute_no>",
						"<group_no>"
					};
	int slen = (sizeof(sources)/sizeof(char *));
	
	int i,j;
	
	// Look into the following keys
	//	path	- Store value into PATH
	//	name	- Store value into name
	//	lastKnownFileType	- Set .h or .c flag
	
	for( i=0; i<klen; i++ )
	{
		if( strcmp(kvp.key,keys[i]) == 0 )
			break;
	}
	
	// (2) Handle the various actions 
	
	switch( i )
	{
		case 0:			// Path, just save it
						strcpy(line.path,kvp.value);
						break;
						
		case 1:			// Name save and mark that we have it
						strcpy(line.name,kvp.value);
						line.file_name = true;
						break;
						
		case 2:
		case 3:
						// File type, determine what type we have
						for(j=0; j<vlen; j++ )
						{
							if( strcmp(kvp.value,values[j]) == 0 )
								break;
						}
						switch( j )
						{
							case 0:			
							case 1:			// Source files just mark that we found it
                            case 8:
											line.file_type = true;
											line.type = SOURCE_FILE;
											break;
							
							case 2:			// Mark that we found source file and
											// that it's an include file
											line.file_type = true;
											line.type = INCLUDE_FILE;
											break;
											
							case 3:			// EXE file, mark and save
                            case 9:
											line.type = EXE_FILE;
											line.file_type = true;
											break;
											
											// Mark that we found a framework
							case 4:			line.file_type = true;
											line.type = LIB_FILE;
											break;
											
											// REF:JS12032006 Add in archive type handler
							case 5:			
                                            line.file_type = true;
											line.type = ARCHIVE_FILE;
											break;

											// REF:JS12032006 Add in archive.ar handler
							case 6:			line.file_type = true;
											line.type = ARCHIVE_FILE;
											break;

							case 7:			// DYLIB file, mark and save
											line.type = DYLIB_FILE;
											line.file_type = true;
											dylib_flag = true;		// Mark doing a dylib
											break;
						
							default:		line.file_type = true;
											line.type = UNK_TYPE;
											break;
						}
						break;
						
			case 4:		if( line.type != UNK_TYPE )
						{
							// File reference type, determine what type we have
							for(j=0; j<slen; j++ )
							{
								if( strcmp(kvp.value,sources[j]) == 0 )
									break;
							}
							switch( j )
							{
								case 0:			// SOURCE Root Reference OK
												break;
								
								case 1:			// ABSOLUTE REFERENCE FAILURE
												printf("** Fix this file and run again\n");
												printf("\t** ABSOLUTE FILE REF [%s]\n",line.name);
												printf("\t** PATH [%s]\n",line.path);
												proj_ok = false;			// Bad project
												break;
												
								case 2:			// GROUP REFERENCE FAILURE
												printf("** Fix this file and run again\n");
												printf("\t** GROUP FILE REF [%s]\n",line.path);
												proj_ok = false;			// Bad proj
												break;
												
							}
						}
						break;
	}
}

/***********************************************************************
*
*   void storeline()	- If line item is complete store in FILE List
*
*	INPUT:	NONE
*				Use current line struct
*
*	OUTPUT:	NONE
*				If current line item is complete store into
*				file or include file list
*	12-Mar-2006	Add code to catch archive type files
*	12-Oct-2006	Add code to check for correct name of target
*
***********************************************************************/

void CMaker::storeline()
{
	FILE_LIST *f;
	FILE_LIST *lp;
	FILE_LIST *list = NULL;		// Init value
	char *name;
	bool add = true;
	
	// (1) Special preprocessing for EXE line output
	// We only have a path but no name, extract the name from the path and
	// add it
	
	if( line.type == EXE_FILE or line.type == DYLIB_FILE )
	{
		name = this->getfname( line.path );
		if( name != NULL )
		{
			strcpy(line.name,name);
			line.file_name = true;
		}
		
		// If target present check for name match, if no match
		// then bail
		
		if( target != NULL && strcmp(target,line.name) != 0 )
		{
			if( line.type == DYLIB_FILE )
				dylib_flag = false;	// Forget we are doing a dylib
			return;				// Bail on this item
		}
	}
	
	// (2) Special processing on SOURCE and INCLUDE where 
	// they only have a path but no name
	
	if( line.type == SOURCE_FILE ||
		line.type == INCLUDE_FILE)
	{
		if( strlen(line.name) == 0 )
		{
			strcpy(line.name,line.path);
			line.file_name = true;
		}
	}
	
	// (3) If no path just put the name there
	// If name but no path just path = name
	
	if( strlen(line.path) == 0 )
	{
		strcpy(line.path,line.name);
	}

	if( debug )
	{
		printf("Line Type = %d\n",line.type);
		printf("Line Name [%s]\n",line.name);
		printf("Line Path [%s]\n\n",line.path);
	}
	
	// (4) Is it complete
	
	if( line.file_name && line.file_type &&
		line.type != UNK_TYPE)
	{
		// (5) Build a struct to hold the data
				
        f = (FILE_LIST *)calloc(1,sizeof(FILE_LIST));
        if( f == NULL )
        {
            printf("*** CANNOT ALLOCATE FILE LIST ***\n");
			return;
        }
        
        // (6) Put the data into the element
        f->name = (char *)calloc(1,strlen(line.name)+1);
        if( f->name == NULL )
        {
            printf("*** CANNOT ALLOCATE FILE LIST NAME ***\n");
			return;
        }
        strcpy(f->name,line.name);

        f->path = (char *)calloc(1,strlen(line.path)+1);
        if( f->path == NULL )
        {
            printf("*** CANNOT ALLOCATE FILE LIST PATH ***\n");
            return;
        }
        strcpy(f->path,line.path);
		
		// Add the type and type marker
		
		f->file_type = line.file_type;
		f->type = line.type;
		
		// (7) Now depending on the type of file add it to the correct list
		
		switch( line.type )
		{
			case SOURCE_FILE:		list = cfiles;		// Add to source list
									break;
									
			case INCLUDE_FILE:		list = includes;	// Add to include list
									break;
									
			case EXE_FILE:			// Just put this as the pgmid pointer
			case DYLIB_FILE:		// Also use DYLIB pointer also
									pgmid = f;
									return;			// && exit
								
			case ARCHIVE_FILE:		list = libs;	// REF:JS12032006 Add to library list
									break; 
									
			case LIB_FILE:			if( frameworks )
										list = libs;	// Add to library list
									else
									{
										add = false;
										list = NULL;	// Don't
									}
									break;
									
			default:				add = false;
									list = NULL;	// Handle unknown
									break;
		}
		        
        // (8) Add the element onto to the list
        // Two cases, list MT so just put in what we have
        //			  list not MT so put on the end
		// If add flag false then just skip all of this
		
		if( add )
		{
        
			if( list == NULL )
				list = f;
			else
			{
				lp = list;
				while( lp->next != NULL )
					lp = lp->next;
					
				lp->next = f;
			}
			
			// (9) Now update the class pointer
			
			switch( line.type )
			{
				case SOURCE_FILE:		cfiles = list;		// Add to source list
										break;
										
				case INCLUDE_FILE:		includes = list;	// Add to include list
										break;
				
				case ARCHIVE_FILE:		libs = list;		// REF:JS12032006 Add archive files to list
										break;
				
				case LIB_FILE:			libs = list;		// Add to library list
										break;
										
				default:				break;
			}
		}
		else
		{
			// Tidy up thrown up line
			
			free( f->name );
			free( f->path );
			free( f );
		}
	}
}
        
/***********************************************************************
*
*   void clrkvp()	- Init KVP
*
*	INPUT:	NONE
*
*	OUTPUT:	NONE
*				KVP struct readyed for next search
*
***********************************************************************/

void CMaker::clrkvp()
{
	kvp.state = 0;
	kvp.key[0] = '\0';
	kvp.value[0] = '\0';
}

/***********************************************************************
*
*   bool getkvp(int type,char *token)	- Get next KVP from stream
*
*	INPUT:	type	- Token type
*			token	- Token string
*
*	OUTPUT:	TRUE if KVP completed (waiting in kvp)
*
***********************************************************************/

bool CMaker::getkvp( int type,char *token)
{
	bool rtnval = false;
	
	// Depending on KVP state do actions
	
	switch( kvp.state )
	{
		case 0:			// Do we have a string ?
						if( type == P_TOK_NAME )
						{
							strcpy(kvp.key,token);
							kvp.state = 1;		// Look for =
						}
						break;
						
		case 1:			// Look for '='
						if( type == P_TOK_LINK )
						{
							kvp.state = 2;		// Look for next name
						}
						else
						{
							kvp.state = 0;		// Failed back to start
						}
						break;
						
		case 2:			// Look for value, cat on fields till END hit
						// This makes the PATH come out right
						if( type == P_TOK_NAME || type == 0)
						{
							strcat(kvp.value,token);
							//kvp.state = 3;		// Look for ; to finish
						}
						else
						{
							if( type == P_TOK_END )
							{
								kvp.state = 4;
								rtnval = true;
							}
							else
							{
								kvp.state = 0;		// Failed
							}
						}
						break;
												
		case 4:			rtnval = true;			// Just lock here
						break;
	}
	
	return( rtnval );
}						

#pragma mark -- BUILD FILE --            
/***********************************************************************
*
*   bool build_make( char *compiler )	- Build the makefile 
*
*	INPUT:	pgmid		- Creator pogram ID
*			compiler	- String name of compiler
*			cflags		- Compiler options
*			ldflags	- Linker options
*
*	OUTPUT:	TRUE if makefile OK, FALSE if not
*
*  13-Oct-2006 Change here to specify object file lcn
*
***********************************************************************/


bool CMaker::build_make( char *id,char *compiler,char *cflags,char *ldflags, char *bin_dir)
{
    //get Info.plist filename.
    string info_plist = find_file_end_with("Info.plist");
    //get *.pch filename.
    string local_pch = find_file_end_with(".pch");

    //guess Resources need to be put into .app dir.
    string res_dir = find_dir_end_with("Resources");
    string res2_dir = find_dir_end_with("images");
   
    string lproj_dir = find_dir_end_with(".lproj");
 
    /*remove .app from program name if it is exists.
      ios xcode application always end with .app.
    */
    if(endWith(pgmid->name, ".app"))
    {
        string t = pgmid->name;
        t = t.substr(0,t.length()-4);
        t = m_replace(t, " ", "_");
        pgmid->name = strdup(t.c_str());
        
    }

    
    FILE_LIST *fl;
	time_t t;
	char buf[32];
	int n;
	int count = 1;
	bool first = true;
    
    // (1) Build the header of the make file and some macros
    
	fprintf(make,"###################################################\n");
    fprintf(make,"#\n");
	
	// Check here for a good program list
	fprintf(make,"# Makefile for %s\n",pgmid->name);
	fprintf(make,"# Creator [%s]\n",id);
	
	// REF:JS16032003 Put out time
	t = time(NULL);
	ctime_r( &t,buf );
	n = strlen(buf);
	buf[n-1] = '\0';		// Remove CR
	fprintf(make,"# Created: [%s]\n",buf);
	
    fprintf(make,"#\n");
	fprintf(make,"###################################################\n");

    fprintf(make,"\n");
    fprintf(make,"#\n");
    fprintf(make,"# Macros\n");
    fprintf(make,"#\n");
    fprintf(make,"\n");

    fprintf(make,"IPHONE_IP:=\n");
    fprintf(make,"PROJECTNAME:=%s\n",pgmid->name);
    fprintf(make,"APPFOLDER:=$(PROJECTNAME).app\n");
    fprintf(make,"INSTALLFOLDER:=$(PROJECTNAME).app\n\n");
	fprintf(make, "\nBUILDDIR=./build\n\n");
    fprintf(make,"CC = %s\n",compiler);
    fprintf(make,"CFLAGS = %s\n",cflags);
	
	this->linker_line(ldflags);
    

    // () handle resources
    if(!res_dir.empty())
        fprintf(make,"\n\nRESOURCES+=$(wildcard ./Resources/*)\n");
    if(!res2_dir.empty())
        fprintf(make,"\n\nRESOURCES+=$(wildcard ./images/*)\n");
    if(!lproj_dir.empty())
        fprintf(make,"RESOURCES+=$(wildcard ./*.lproj)\n");


    // (2) Trim the paths from the include file list
    // and build the INCLUDE macro

    // (2.1) handle Info.plist
    if(!info_plist.empty())
        fprintf(make,"\nINFOPLIST:=$(wildcard *Info.plist)\n\n");
    else
        fprintf(stderr,"Info.plist NOT found.\n");

 
	fprintf(make,"\n");
	fprintf(make,"#\n");
	fprintf(make,"# INCLUDE directories for %s\n",pgmid->name);
	fprintf(make,"#\n");
	fprintf(make,"\n");
    includes = this->trim_paths(includes);
	fl = includes;

	
	// (2a) Put in include line for HERE
	if( fl != NULL )
			fprintf(make,"CFLAGS += -I.\\\n");		// Multiple lines
	else
			fprintf(make,"CFLAGS += -I.\n");		// Single line

	while( fl != NULL )
	{
		// REF:JS27092003
		// Put the items on separate lines
		if( fl->next != NULL )
			fprintf(make,"\t\t-I%s\\\n",fl->path);	// Middle item
		else
			fprintf(make,"\t\t-I%s\n",fl->path);	// Last item
		
		fl = fl->next;
	}
	fprintf(make,"\n\n");

    // (2a.1) If .pch file exists, add it to CFLAGS
    if(!local_pch.empty())
			fprintf(make,"CFLAGS += -include %s\n",strdup(local_pch.c_str()));		
        
    
    // (3) Put out the major product line
    // Note we have to remove .c from all the file names
    
    fprintf(make,"#\n");
    fprintf(make,"# Build %s\n",pgmid->name);
    fprintf(make,"#\n");
    fprintf(make,"\n");
    fprintf(make,"%s : \\\n",pgmid->name);  // REF:JS06032004 Make alone on the line
    this->trim_files( cfiles );				// Remove the extension from the name
    fl = cfiles;
	
	// Change this loop to put out file and then on the next pass
	// put the \<RET> since we don't know if this might or might not
	// be the last file
	// Put two \n's at the end of the section to terminate it
	// NOTE: Make sure first = TRUE at start of loop
	
    while( fl != NULL )
    {
		if( target_files->check(fl->name))
		{
			if( first )
			{
				first = false;
			}
			else
			{
				fprintf(make,"\\\n");
			}
			fprintf(make,"\t\t%s/%s.o",obj,fl->name);
		}
        fl = fl->next;
    }
    fprintf(make,"\n");			// Terminate the section
								// NOTE: Remove extra LF between this and 
								// compile line REF:JS03112006

    fprintf(make,"\t$(CC) $(LDFLAGS) \\\n");	// REF:JS06032004 alone on line
    fl = cfiles;

	// Loop for all the o files
	
    while( fl != NULL )
    {
		if( target_files->check(fl->name))
		{
			fprintf(make,"\t\t%s/%s.o\\\n",obj,fl->name);
		}
        fl = fl->next;
    }
	
	// Put in the output line
	
    fprintf(make,"\t\t-o %s\n",pgmid->name);
    
	// (3.1) Insert 'clean'
	// REF:JS13102006 Add in OBJ file lcn
	
	fprintf(make,"\nclean : \n");
	fprintf(make,"\t\trm -rf\\\n");
	fl = cfiles;

	// Loop through all the files to delete
	// NOTE: We don't need the fancy check from above since each .o
	// stands alone and we add in the last line to delete the ultimate product
	// which closes the \ things.
	
	while( fl != NULL )
    {
		if( target_files->check(fl->name))
		{
			fprintf(make,"\t\t%s/%s.o\\\n",obj,fl->name);
		}
        fl = fl->next;
    }
	fprintf(make, "\t\t%s\n", pgmid->name);
	fprintf(make,"\t\trm -rf ./build\n");

    // (3.2) Insert 'dist'
	fprintf(make, "\ndist : %s\n", pgmid->name);
	fprintf(make, "\t\trm -rf $(BUILDDIR)\n");
	fprintf(make, "\t\tmkdir -p $(BUILDDIR)/$(APPFOLDER)\n");
    fprintf(make, "ifneq ($(RESOURCES),)\n");
    fprintf(make, "\t\tcp -r $(RESOURCES) $(BUILDDIR)/$(APPFOLDER)\n");
    fprintf(make, "endif\n");
    
	fprintf(make, "\t\tcp $(INFOPLIST) $(BUILDDIR)/$(APPFOLDER)/Info.plist\n");
	fprintf(make, "\t\t@echo \"APPL????\" > $(BUILDDIR)/$(APPFOLDER)/PkgInfo\n");
    fprintf(make, "\t\tldid -S $(PROJECTNAME)\n");
    fprintf(make, "\t\tmv $(PROJECTNAME) $(BUILDDIR)/$(APPFOLDER)\n");
    fprintf(make, "\t\tmkdir -p $(BUILDDIR)/Payload\n");
    fprintf(make, "\t\tcd $(BUILDDIR)/Payload; ln -s ../$(APPFOLDER) .\n");
    fprintf(make, "\t\tcd $(BUILDDIR); zip -r $(PROJECTNAME).ipa Payload > /dev/null\n");
    fprintf(make, "\t\trm -fr $(BUILDDIR)/Payload\n");
    
    	
	// (3.3) Insert 'install'
	fprintf(make, "\ninstall : dist\n");
    fprintf(make, "\t\tping -t 3 -c 1 $(IPHONE_IP)\n");
    fprintf(make, "\t\tssh root@$(IPHONE_IP) 'rm -fr /Applications/$(INSTALLFOLDER)'\n");
    fprintf(make, "\t\tscp -r $(BUILDDIR)/$(APPFOLDER) root@$(IPHONE_IP):/Applications/$(INSTALLFOLDER)\n");
    fprintf(make, "\t\t@echo \"Application $(INSTALLFOLDER) installed\"\n");
    fprintf(make, "\t\tssh mobile@$(IPHONE_IP) 'uicache'\n");
    
	// (3.3) Insert 'uninstall'
	fprintf(make, "\nuinstall:\n");
    fprintf(make, "\t\tping -t 3 -c 1 $(IPHONE_IP)\n");
    fprintf(make, "\t\tssh root@$(IPHONE_IP) 'rm -fr /Applications/$(INSTALLFOLDER)'\n");
    fprintf(make, "\t\t@echo \"Application $(INSTALLFOLDER) uninstalled\"\n");
	
    // (4) Now build each of the parts
    
    fprintf(make,"\n");
    fprintf(make,"#\n");
    fprintf(make,"# Build the parts of %s\n",pgmid->name);
    fprintf(make,"#\n");
    fprintf(make,"\n");
    
    fl = cfiles;
    while( fl != NULL )
    {
        // Print the dependency line
        // then print the compile line
        
		if( target_files->check(fl->name))
		{
			// REF:JS09272003 Add counter
			// REF:JS13102006 Add in OBJ lcn
			fprintf(make,"\n# Item # %d -- %s --\n",count++,fl->name);
			fprintf(make,"%s/%s.o : %s\n",obj,fl->name,fl->path);
			fprintf(make,"\t$(CC) $(CFLAGS) %s -c -o %s/%s.o\n\n",
										fl->path,
										obj,
										fl->name);
		}
        
        fl = fl->next;
    }
    
    fprintf(make,"\n##### END RUN ####\n");
    return( true );
}

/***********************************************************************
*
*   void linker_line( void )	- Build the LNK options line 
*
*	INPUT:	lnk_opts	- Linker options
*
*	OUTPUT:	NONE
*			Add any required frameworks
*
***********************************************************************/

void CMaker::linker_line( char *lnk_opts )
{
	FILE_LIST *lib = libs;
    
    // (1) Put out the linker macro def
	// REF:JS22082005 Change here to
	// If Libs present then put a \and \n on the end
	// of any opts present
	// If no libs then just a \n for the option line

    fprintf(make,"LDFLAGS = %s",lnk_opts);
	
	// Only put out the default stuff if the didn't
	// add anything.
	
	if( dylib_flag && strlen(lnk_opts) != 0 )
	{
		// Put out the dylib stuff if necessary
		// I.e. lnk_opts are not input
		
		fprintf(make,"\t\t-dynamiclib\\\n");
		fprintf(make,"\t\t-Wl,-single_module\\\n");
		fprintf(make,"\t\t-compatibility_version 1\\\n");
		fprintf(make,"\t\t-current_version 1");
	}
	
	// (2) Now do we have any frameworks ?
	// If not just terminate the above line with a \n 
	// otherwise escape the \n with a '\ '
	
	if( lib == NULL )
	{
		fprintf(make,"\n");			// Terminate the options line
	}
	else
	{
		fprintf(make,"\\\n");		// Escape the \ \n and do next
	
		// Two types of files here,
		// LIB_FILE = FRAMEWORK
		// ARCHIVE_FILE = link file
		// FRAMEWORK - strip extension and put out -framework
		// ARCHIVE = -l<name>
		
		while( lib != NULL )
		{
			switch( lib->type )
			{
				case LIB_FILE:	// FRAMEWORK type
								this->trim_file( lib );		// Remove extension
								fprintf(make,"\t\t-framework %s",lib->name);
								break;
								
				case ARCHIVE_FILE:	// Put these out all as just name with no extension
								this->trim_file( lib );
								fprintf(make,"\t\t-l%s",lib->name);
								break;
								
				default:		break;
								
			}
			
			// Loop around all of these putting \ at the ends of the lines
			
			fprintf(make,"\\\n");		// Middle line

			// Now go to next
				
			lib = lib->next;
		}
		
		// Now do another loop to put in -L lines to show
		// where to look for the above files
		// Trim this lib list down to unique extensions
		
		lib = libs;
		this->trim_paths(lib);
		while( lib != NULL )
		{
			fprintf(make,"\t\t-L%s",lib->path);
			
			// Terminate the line in the correct fashion
			
			if( lib->next != NULL )
				fprintf(make,"\\\n");		// Middle line
			else
				fprintf(make,"\n");			// Last line
				
			lib = lib->next;
		}
	}
	
	// (3) All done, put a CR on the macro & quit
	
	fprintf(make,"\n");
}

/***********************************************************************
*
*   void trim_files( FILE_LIST *list )	- Remove the extension from all makes
*
*	INPUT:	list	- File list
*
*	OUTPUT:	All names in the list of the form xxx.c -> xxx
*
***********************************************************************/

void CMaker::trim_files( FILE_LIST *list)
{
    // Loop through all the names
    
    while( list != NULL )
    {
		trim_file( list );
		
        // Move to next
        
        list = list->next;
    }
}

/***********************************************************************
*
*   void trim_file( FILE_LIST *list )	- Remove the extension from one
*
*	INPUT:	list	- File list
*
*	OUTPUT:	Convert first name in list from  xxx.c -> xxx
*
***********************************************************************/

void CMaker::trim_file( FILE_LIST *list)
{
    int n,i;
    
	// Get the length of the name for the search
	
	n = strlen( list->name );
	
	// Scan backwards and put a null in on the "."
	
	for( i=n-1; i >= 0; i-- )
	{
		if( list->name[i] == '.' )
		{
			list->name[i] = '\0';	// Remove it...
			break;
		}
	}
        
}

/***********************************************************************
*
*   bool trim_paths( FILE_LIST *list )	- Remove the file name from the path
*
*	INPUT:	list	- File list
*
*	OUTPUT:	All paths changed from xx/xx/thing.c -> xx/xx
*			Duplicate paths eliminated
*			Also remove 0 length paths, and return new path struct
*
***********************************************************************/

FILE_LIST *CMaker::trim_paths( FILE_LIST *list)
{
    int n,i;
	FILE_LIST *base;
	FILE_LIST *scan,*scanp,*del;
	
	// Save the base of the list
	
	base = list;
	if( base != NULL )
	{
		// Loop through all the names
		// Trim file names from path and remove 
		// 0 length paths
		
		scan = base;
		scanp = NULL;
		while( scan != NULL )
		{
			// Get the length of the name for the search
			
			n = strlen( scan->path );
			
			// Scan backwards and put a null in on the "/"
			
			for( i=n-1; i >= 0; i-- )
			{
				if( scan->path[i] == '/' )
				{
					scan->path[i] = '\0';	// Remove it...
					break;
				}
			}
			
			// Special case, if i < 0 here then no '\' in path to remove it
			
			if( i < 0 )
			{
				scan->path[0] = '\0';
			}
			
			if( strlen(scan->path) == 0 )
			{
				// To be here we have a 0 length name
				// So we have to drop the value at scan
				// Save the deleted one for later
				
				del = scan;
				
				// Two cases, If scanp == NULL we are at the start
				//			  so link around scan, by putting scan->next
				//			  to link->next.  Then set scanp and advance 
				//		      to next scan element
				//
				//			  If scanp <> NULL then link around
				//			  scan and move scan ahead
				//
				// If name not length 0 just advance scan & scanp
				if( scanp == NULL )
				{
					base = scan->next;		// Link around from list
					scanp = NULL;			// Still at list start
					scan = base;			// Start again from front
				}
				else
				{
					scanp->next = scan->next;		// Link around from scanp
					scan = scan->next;
				}
				
				// Remove the deleted element
				
				free( del->name);
				free( del->path);
				free( del );
			}
			else
			{
				// No drop here so just move to next
				
				scanp = scan;
				scan = scan->next;
			}
		}
		
		//*******************************************************
		// Scan the list in a double loop to eliminate duplicates
		
		list = base;
		while( list != NULL )
		{
			scan = list->next;
			scanp = NULL;
			while( scan != NULL)
			{
				// Is this a duplicate ?
				
				if( strcmp(scan->path,list->path) == 0 )
				{
					// To be here we have a matching element
					// So we have to drop the value at scan
					// Save the deleted one for later
					
					del = scan;
					
					// Two cases, If scanp == NULL we are at the start
					//			  so link around scan, by putting scan->next
					//			  to link->next.  Then set scanp and advance 
					//		      to next scan element
					//
					//			  If scanp <> NULL then link around
					//			  scan and move scan ahead
					//
					// If no match just advance scan & scanp
					if( scanp == NULL )
					{
						list->next = scan->next;		// Link around from list
						scanp = list;
						scan = scan->next;
					}
					else
					{
						scanp->next = scan->next;		// Link around from scanp
						scan = scan->next;
					}
					
					// Remove the deleted element
					
					free( del->name);
					free( del->path);
					free( del );
				}
				else
				{
					// No drop here so just move to next
					
					scanp = scan;
					scan = scan->next;
				}
			}
			
			// Move outer search index
			// Move to next one in list
			
			list = list->next;
		}
	}
	
	return( base );
}

/***********************************************************************
*
*  void build_print(  )	- Print out list of include & c file
*
*	INPUT:	NONE
*
*	OUTPUT:	NONE
*			Print list of include and c files
*
***********************************************************************/

void CMaker::build_print()
{
	FILE_LIST *l;
	int count;
	
	// (1) Print C files
	
	l = cfiles;
	printf("\n***** C FILE LIST ****\n");
	count = 1;
	while( l != NULL )
	{
		printf("File [%3d] %s\n",count++,l->path);
		l = l->next;
	}

	// (2) Print H files
	
	l = includes;
	printf("\n***** H FILE LIST ****\n");
	count = 1;
	while( l != NULL )
	{
		printf("File [%3d] %s\n",count++,l->path);
		l = l->next;
	}
	// (3) Print FRAMEWORKS files
	
	l = libs;
	printf("\n***** LIB LIST ****\n");
	count = 1;
	while( l != NULL )
	{
		printf("Framework [%3d] %s\n",count++,l->path);
		l = l->next;
	}
}

#pragma mark -- File Name/Path extraction --
/***********************************************************************
*
*  char *getfname( char *path )	- Return file name from a path
*
*	INPUT:	file name and path
*
*	OUTPUT:Pointer to internal buffer with just the file name
*
***********************************************************************/

char *CMaker::getfname( char *path )
{
	char *start;
	int i,n;
	
	// (1) Get the length and to scan backward through the file
	
	n = strlen( path );
	
	// (2) Scan backwards through the path and find the first / (or none)
	
	for( i=n-1; i>=0; i-- )
	{
		if( path[i] == '/' )
			break;
	}
	
	// (3) Now get the pointer to where this starts
	// and copy the data into out local buffer
	// then return the pointer to that
	
	start = (char *)&path[i+1];
	strncpy( namestore,start,256 );
	start = (char *)&namestore[0];
	return( start );
}
	
/***********************************************************************
*
*  char *getfpath( char *path )	- Return path name from a path
*								  less the terminating file
*
*	INPUT:	file name and path
*
*	OUTPUT:Pointer to internal buffer with just the file name
*
***********************************************************************/

char *CMaker::getfpath( char *path )
{
	char *start;
	int i,n;
	
	// (1) Copy the path into the path buffer for our work
	
	strncpy(pathstore,path,256);
	
	// (1) Get the length and to scan backward through the path
	
	n = strlen( path );
	
	// (2) Scan backwards through the path and find the first / (or none)
	// Set that to NUL to cut it off there
	
	for( i=n-1; i>=0; i-- )
	{
		if( pathstore[i] == '/' )
		{
			break;
		}
	}
	if( i < 0 )
		i =0;
		
	pathstore[i] = '\0';

	
	// (3) Now if the length of this is 0 the return ./
	// otherwise return what we got
	
	if( strlen(pathstore) == 0 )
	{
		strcpy(pathstore,"./");
	}
	
	start = (char *)&pathstore[0];
	return( start );
}
