/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_IEDITVALUE_DOT_H
#define WG_IEDITVALUE_DOT_H

#ifndef WG_IMODIFVALUE_DOT_H
#	include <wg_imodifvalue.h>
#endif

#ifndef WG_CURSOR_DOT_H
#	include <wg_cursor.h>
#endif

class WgIEditValue;
typedef	WgISmartPtr<WgIEditValue,WgIModifValuePtr>		WgIEditValuePtr;
typedef	WgWeakPtr<WgIEditValue,WgIModifValuePtr>		WgIEditValueWeakPtr;

/**
 * @brief Interface to a value display field with editable text
 * 
 * The value in an editable value field can be set through the API, and is
 * editable through the UI.
 * 
 */
 
class WgIEditValue : public WgIModifValue
{
public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgIEditValuePtr	Cast( const WgInterfacePtr& pInterface );				// Provided just for completeness sake.

	virtual void			SetEditMode(WgTextEditMode mode) = 0;
	virtual WgTextEditMode	EditMode() const = 0;

	virtual bool			IsEditable() const = 0;
	virtual bool			IsSelectable() const = 0;

	virtual void			SetCursorSkin( const WgCursorPtr& pCursor ) = 0;
	virtual WgCursorPtr		CursorSkin() const = 0;

	// Calling these methods gets field into edit mode, displaying cursor.

	virtual int				InsertAtCursor( const WgCharSeq& str ) = 0;
	virtual bool			InsertAtCursor( Uint16 c ) = 0;

	virtual int				Append( const WgCharSeq& seq ) = 0;
	virtual int				Insert( int ofs, const WgCharSeq& seq ) = 0;
	virtual int				Replace( int ofs, int nDelete, const WgCharSeq& seq ) = 0;
	virtual int				Delete( int ofs, int len ) = 0;
	virtual void			DeleteSelected() = 0;

	virtual void			Select( int ofs, int len ) = 0;
	virtual void			SelectAll() = 0;
	virtual int				SelectionStart() const = 0;
	virtual int				SelectionLength() const = 0;
	virtual void			ClearSelection() = 0;

	virtual void			GoBOL() = 0;
	virtual void			GoEOL() = 0;
	virtual void			GoBOF() = 0;
	virtual void			GoEOF() = 0;
};


#endif //WG_IEDITVALUE_DOT_H