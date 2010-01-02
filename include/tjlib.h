#ifndef	TJLIB_DOT_H
#define TJLIB_DOT_H

#ifdef	__cplusplus
extern "C" {
#endif


typedef struct
{
	int		nStr;
	int		maxStr;
	char ** pStr;
} TJ_StrArray;

typedef enum
{
	TJERR_OK 		= 0,
	TJERR_ERROR		= 1,
	TJERR_FILEOPEN	= 2,
	TJERR_FILEWRITE	= 3,
	
} TJ_ErrCode;


TJ_StrArray	* 	TJ_LoadStrArray( const char * pName );
TJ_ErrCode		TJ_SaveStrArray( const char * pName, const TJ_StrArray * pArray );
void			TJ_FreeStrArray( TJ_StrArray * pArray );

TJ_StrArray	*	TJ_CreateStrArray( int maxStr );
void			TJ_PushStr( TJ_StrArray * pArray, char * pStr );


TJ_StrArray	*	TJ_SplitStr( const char * pDelimeter, const char * pStr );
char *			TJ_JoinStrArray( const char * pSeparator, const TJ_StrArray * pArray );
void			TJ_SortStrArrayAscend( TJ_StrArray * pArray );

char *			TJ_SubStr( const char * pBeg, const char * pEnd );
char *			TJ_CloneStr( const char * pOrg );


unsigned int	TJ_FileSize( const char * pName );
void * 			TJ_LoadFile( const char * pName );
unsigned int	TJ_SaveFile( const char * pName, void * pBuf, int nBytes );

char * 			TJ_GetFilenameExtension( const char * pName );
void 			TJ_ChangeFilenameExtension( char * pName, const char * pNewExtension );	// Appends new extension if there was none.

int				TJ_ReadUTF8( const char ** ppText ); // Fast, but not especially robust.
void			TJ_WriteUTF8( char ** ppText, int character );


	
#ifdef	__cplusplus
}
#endif
	
#endif //TJLIB_DOT_H
