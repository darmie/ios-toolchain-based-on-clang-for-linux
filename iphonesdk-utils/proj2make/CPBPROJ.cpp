/************************************************************************
*																		*
* 			Pandora Products - Jim Schimpf PBXPROJ File Reader			*
*					Copyright 2002 Jim Schimpf							*
*  																		*
*		MAC FILE SYNTAX ANALYZER FOR Mac Project Builder Files			*
*																		*
*		Module:CPBXPROJ.h												*
*																		*
*	Version		Date	Person  	Description							*
*-----------------------------------------------------------------------*
*	0.1		28-Jul-2002	J.Schimpf	Initial Version						*
*	0.2		30-Jul-2002 J.Schimpf	Add parent pointer					*
*	0.3		31-Jul-2002 J.Schimpf	Allow both relative & abs paths		*
*	0.4		14-Mar-2003	J.Schimpf	Put isa's in their own symbol table	*
*																		*
*	DESCRIPTION:													    *
*		This file reads the tokens from a PBXPROJ file and produces a 	*
*	file structure linked list of objects								*
*																		*
*	Productions:														*
*		PROPERTY	<name> = <name>;									*
*		OBJECT		<name> = { <Child objects or props> } ;				*
*		COMMENT		// <Rest of line>									*
*																		*
*																		*
************************************************************************/

#include "CPBPROJ.h"

/***********************************************************************
					*** CREATE/DELETE PBXPROB ***
***********************************************************************/
/***********************************************************************
*
*  CPbxProj()	- Init for object extract
*
*	INPUT:	fp	- Open to pbxproj file 
*
*	OUTPUT:	NONE
*			Start run and parse the input file 
*
***********************************************************************/

CPbxProj::CPbxProj(CpbxLexFile *fp,bool debug_flag) 
{
	// (1) Attach to the input object & set debug
	
	in = fp;
	debug = debug_flag;
	
	// (2) Build the symbol table & init object for run
	
	syms = new CSymbol;
	base = NULL;
	
	// (3) Start the recursive parse, note, parent == NULL
	
	base = this->build_object(NULL,"");	
	
	// (4) Show debug dump
	
	if( debug )
	{
		printf("\n*** FILE STRUCT DUMP ***\n");
		this->printPBX(base);
	}
}	

/***********************************************************************
*
*  ~CPbxProj()	- Recover memory from object
*
*	INPUT:	fp	- Open to pbxproj file 
*
*	OUTPUT:	NONE
*			Start run and parse the input file 
*
***********************************************************************/

CPbxProj::~CPbxProj(void)
{
	PBXPROJ *obj,*sibling;
	
	// Do a recursive memory release
	// Depth first do all children then siblings
	
	obj = base;
	while( obj != NULL )
	{
		sibling = obj->sibling;
		this->remove_object( obj );
		
		obj = sibling;
	}
}

/***********************************************************************
					*** CLASS API ***
***********************************************************************/
/***********************************************************************
*
* PBXPROJ *GetPBX(void) - Get base object of PBXPROJ
*
*	INPUT:	NONE
*
*	OUTPUT:	Base object pointer
*
***********************************************************************/
PBXPROJ *CPbxProj::GetPBX(void)
{
	return( base );
}

/***********************************************************************
*
* void printPBX( PBXPROJ *proj ) - Print listing of object
*
*	INPUT:	proj	- Start printing at this level
*
*	OUTPUT:	NONE
*		Printed listing depth first
*
***********************************************************************/

void CPbxProj::printPBX( PBXPROJ *proj )
{
	// Print this at level 0
	
	this->printPBXint( 0,proj );
}

/***********************************************************************
*
* PBXPROJ *find_object( PBXPROJ *parent,char *name,char *value ) - Find a node
*
*	INPUT:	parent	- Start search here (if NULL use base)
*			name	- Find match for this name (ignored if strlen == 0)
*			isa		- Find matching isa (ignored if strlen == 0 )
*					  Also name and isa are mutually exclusive
*			value	- Find match for this value (ignored if strlen == 0 )
*
*	OUTPUT:	Pointer to node found, or NULL if not
*
***********************************************************************/

PBXPROJ *CPbxProj::find_object( PBXPROJ *parent,char *name,char *value )
{
	PBXPROJ *object;
	bool name_srch = false;
	bool val_srch = false;
	bool head_srch = false;
	
	// (1) Look at the start search point, if NULL then use base
	
	if( parent != NULL )
		object = parent;
	else
	{
		head_srch = true;		// Mark that we are starting at the top
		object = base;
	}
		
	// (2) What kind of search will we do ?
	
	if( strlen( name ) != 0 )
		name_srch = true;
	if( strlen(value) != 0 )
		val_srch = true;
		
	if( !name_srch && !val_srch )
		return( NULL );				// Bad search so quit now
		
	// (3a) If name_srch starting at top only then do symbol table lookup
	
	if( !val_srch && head_srch)
	{
		object = (PBXPROJ *)syms->find( name );
		return( object );
	}
		
	// (3) Now do recursive search and try to find the object
	
	object = this->find_objint( name_srch,name,val_srch,value,object );
	
	return( object );
}

/***********************************************************************
*
* PBXPROJ *find_objectbysym( int nth,char *name ) - Find a node
*
*	INPUT:	nth		- Find nth occurance (0 - 1st...)
*			name	- Find match for this name (ignored if strlen == 0)
*
*	OUTPUT:	Pointer to node found, or NULL if not
*
***********************************************************************/

PBXPROJ *CPbxProj::find_objectbysym( int nth,char *name )
{
	PBXPROJ *object;
	
	// (1) Just do symbol table look up and find nth
	
	object = (PBXPROJ *)syms->find( nth,name );
	return( object );
}

/***********************************************************************
					*** PRIVATE ROUTINES ***
***********************************************************************/
/***********************************************************************
*
* void printPBXint( int indent, PBXPROJ *proj ) - Print listing of object
*
*	INPUT:	indent	- Indent this many spaces
*			proj	- Start printing at this level
*
*	OUTPUT:	NONE
*		Printed listing depth first
*
***********************************************************************/

void CPbxProj::printPBXint( int indent,PBXPROJ *proj )
{
	int i;
	PBXPROJ *p;
	
	// (0) Bail on MT level
	
	if( proj == NULL )
		return;
	
	// (1) Indent for level
	
	for( i=0; i<indent; i++ )
		printf(" ");
			
	// Print the data on this one
	
	printf("NAME [%s] VALUE [%s]\n",proj->name,proj->value);
	
	// (2) Print the children
	
	p = proj->child;
	while( p != NULL )
	{
		this->printPBXint( indent+1,p );
		p = p->sibling;
	}
}

/***********************************************************************
*
* PBXPROJ *build_newobject( PBXPROJ *parent,char *name ) - Get the next object
*
*	INPUT:	parent	- Parent object
*			name	- Name of this object
*
*	OUTPUT:	Return pointer to next object
*
*	We should see
*			=		P_TOK_LINK
*	then
*			{		P_TOK_OBJ_START recurse to new object
*	or
*			<name>	P_TOK_NAME
*	or
*			"/"<name>	Path
*
*	then
*			P_TOK_END	End of object
*
***********************************************************************/

PBXPROJ	*CPbxProj::build_object( PBXPROJ *parent,char *name )
{
	PBXPROJ	*object,*obj;
	int i = 0;
	
	// (1) Build the object 
	
	object = (PBXPROJ *)calloc(1,sizeof(PBXPROJ));
	if( object == NULL )
	{
		printf("*** OUT OF MEMORY OBJECT CREATION ***\n");
		return( NULL );
	}
	
	// (2a) Add name (if any)
	
	if( name != NULL )
	{
		object->name = (char *)calloc(1,1+strlen(name));
		if( object->name == NULL )
		{
			printf("*** OUT OF MEMORY OBJECT NAME CREATION ***\n");
			this->remove_object( object );
			return( NULL );
		}

		//if( strcmp("380C529C048BB3250060D0EE",name) == 0 )
		if( strcmp("PRODUCT_NAME",name) == 0 )
		{
			i++;
		}
			
		strcpy(object->name,name);
		syms->add( object->name,object);
	}

	// (2) Now finish the object
	
	this->finish_object( object );
	
	// (3) And link it to parent
    
    object->parent = parent;
	
	// (2) All done add object to parent
	// NOTE: If parent == null then don't bother
	// as this is the top
	
	if( parent != NULL )
	{
		// NOTE: Add to the end
		// Two cases, original socket is MT so put on parent
		//            not MT so find end
		
		if( parent->child == NULL )
			parent->child = object;
		else
		{
			obj = parent->child;
			while( obj->sibling != NULL )
				obj = obj->sibling;
				
			obj->sibling = object;
		}
	}


	return( object );
}

/***********************************************************************
*
* void finish_object( PBXPROJ *parent,char *name ) - Pull tokens for next obj
*
*	INPUT:	parent	- Parent object
*			name	- Name of this object
*
*	OUTPUT:	NONE
*
*	We should see
*			=		P_TOK_LINK
*	then
*			{		P_TOK_OBJ_START recurse to new object
*	or
*			<name>	P_TOK_NAME
*	or
*			"/"<name>	Path
*
*	then
*			P_TOK_END	End of object
*
***********************************************************************/

void CPbxProj::finish_object( PBXPROJ *object )
{
	int type;
	char token[MAX_C_TOKEN];
	bool run = true;
	bool link_seen = false;
		
	// (1) Now read till object is closed
	
	while( run )
	{
		type = in->get_token( token );
		switch( type )
		{
			// Just skip over the "=" sign
			
			case P_TOK_LINK:	link_seen = true;
								break;
			
			// We have a name if we have not seen a link yet then
			// then this is the name of a child object
			// If we have then it's the value of the object
										
			case P_TOK_NAME:	if( !link_seen )
									this->build_object( object,token );
								else
								{
                                    // If name is .. then it's a path and get that
                                    
                                    if( strcmp("..",token) == 0 )
                                        this->do_path( token,false );	// Relative path
                                        
									// Put the value on the object
									object->value = (char *)calloc(1,1+strlen(token));
									if( object->value == NULL )
									{
										printf("*** OBJECT OUT OF MEMORY VALUE CREATION ***\n");
										this->remove_object( object );
										return;
									}
									strcpy(object->value,token);
									link_seen = false;
								}
								break;
			
			// Here => starting on a new child object
			// so recurse
								
			case P_TOK_OBJ_START:	this->finish_object( object);
									link_seen = false;
                                    run = false;
									break;
			
									
			case P_TOK_OBJ_END:		link_seen = false;
									break;

			// Object closed, at this point put it into the symbol table
			// NOTE Put symtable add here so that simple key = value also
			// are put into the symbol table
									
			case P_TOK_LIST:
			case P_TOK_END:			run = false;
									break;
			
			// Bad stuff in buffer
									
			case TOK_BUF_MT:
			case TOK_BUF_EOF:		run = false;
									break;
									
			// Look for '/' marker if found we have either a comment or a path
			// Check for this
			
			case P_TOK_PATH:		type = in->get_token(token);
									if(  type == P_TOK_PATH )
										this->do_comment();
									else
									{
										this->do_path( token,true );	// Absolute path
										object->value = (char *)calloc(1,1+strlen(token));
										if( object->value == NULL )
										{
											printf("*** OUT OF MEMORY PATH ALLOC ***\n");
											this->remove_object( object );
											return;
										}
										else
											strcpy(object->value,token );
										link_seen = false;
									}
									break;
		}
	}
	
}

/***********************************************************************
*
* void do_comment( void ) - Remove a comment line
*
*	INPUT:	NONE
*
*	OUTPUT:	We have just hit a / so read till EOL found
*
***********************************************************************/

void CPbxProj::do_comment( )
{
	int type;
	char token[MAX_C_TOKEN];
	
	while( 1 )
	{
		type = in->get_token( token );
		
		if( debug )
			printf("COMMENT [%s]\n",token);
		
		if( type == TOK_BUF_EOL )
			break;
	}
}

/***********************************************************************
*
* void do_path( char *path ) - Pull out a path
*
*	INPUT:	path	- First name in PATH
*			abs		- TRUE if absolute path (has leading /)
*
*	OUTPUT:	NONE
*				Full path in path
*	Path is of the form /<name1>/<name2>/.....;
*	path passed is <name1> so build the full path and return in path
*
***********************************************************************/

void CPbxProj::do_path( char *path,bool abs )
{
	char token[MAX_C_TOKEN];
	int type;
	bool run = true;
	bool path_mark = false;

	// (1) Rebuild the first part of the path
	
    if( abs )
        sprintf(token,"/%s",path);			// Leading '//
    else
        strcpy(token,path);					// Relative path
        
	strcpy(path,token);
	
	// (2) Loop pulling the rest till we hit an end
	
	while( run )
	{
		type = in->get_token( token );
		switch( type )
		{
		
			// Put the next chunk on, either a path separator or
			// a new name
					
			case P_TOK_PATH:	path_mark = true;
								strcat(path,"/");
								break;
												
			case P_TOK_NAME:	if( path_mark )
								{
									strcat(path,token);
									path_mark = false;
								}
								else
								{
									// Handle enbedded spaces in names
									
									strcat(path,token);
									path_mark = false;
								}
								break;
								
			// Handle various end conditions
			
			case P_TOK_LIST:
			case TOK_BUF_EOL:
			case TOK_BUF_MT:
			case P_TOK_END:	
									in->unget_token();	// Return terminator upstairs
									run = false;
									break;

			default:			// Everything else just cat on
			
								strcat(path,token);
								path_mark = false;
								break;
		}
	}
}		
/***********************************************************************
*
* PBXPROJ *find_objint( bool name_src,char *name,bool val_srch,char *value,PBXPROJ *object )
*
*	INPUT:	name_srch	- True if name should be checked
*			name		- Name to search for
*			val_srch	- True if value should be checked
*			value		- Value to search for
*			parent	- Start search here (if NULL use base)
*
*	OUTPUT:	Pointer to node found, or NULL if not
*
***********************************************************************/

PBXPROJ *CPbxProj::find_objint( bool name_srch,char *name,bool val_srch,char *value,PBXPROJ *object )
{
	PBXPROJ *obj,*objc;
	bool name_flag;
	bool val_flag;
	
	// (0) Quit on bad object
	
	if( object == NULL )
		return( NULL );
	
	// (1) Search siblings first
	
	obj = object;
	while( obj != NULL )
	{
		// Check name
		if( name_srch )
		{
			if( strcmp( name,obj->name) == 0 )
				name_flag = true;
			else
				name_flag = false;
		}
		else
			name_flag = true;		// If not checked then mark true
		
		// Check value	
		if( val_srch )
		{
			if( obj->value != NULL && 
                strcmp( value,obj->value) == 0 )
				val_flag = true;
			else
				val_flag = false;
		}
		else
			val_flag = true;		// If not checked then mark true
			
		// Do we have it ?
		
		if( val_flag && name_flag )
			return( obj );			// SUCCESS
			
		// (2) Check children if we didn't find it here
	
		objc = find_objint( name_srch,name,val_srch,value,obj->child );
		if( objc != NULL )
			return( objc );

		// Otherwise loop for next
		
		obj = obj->sibling;
	}
	
	// (3) Falling out here => we failed.....
	
	return( NULL );
}
	
/***********************************************************************
*
* void remove_object( PBXPROJ *object ) - Remove this object and all children
*
*	INPUT:	object	- Object and children to be removed
*
*	OUTPUT:	NONE
*			Remove this object and all children
*
***********************************************************************/

void CPbxProj::remove_object( PBXPROJ *object )
{
	
	// (1) Is this trip necessary ?
	
	if( object == NULL )
		return;
		
	// (2) Take out the children
	
	this->remove_object( object->child );
	
	// (e) Finally remove the object
	
	if( object->name != NULL )
		free( object->name );
	if( object->value != NULL )
		free( object->value );
	free( object );
}