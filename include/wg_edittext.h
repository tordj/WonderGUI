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

#ifndef WG_EDITTEXT_DOT_H
#define WG_EDITTEXT_DOT_H

#ifndef WG_MODTEXT_DOT_H
#	include <wg_modtext.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

class WgEditText;
typedef	WgIStrongPtr<WgEditText,WgModTextPtr>		WgEditTextPtr;
typedef	WgIWeakPtr<WgEditText,WgModTextWeakPtr>		WgEditTextWeakPtr;

/**
 * @brief Interface to a text field with editable text.
 *
 * Interface to a text field with editable text.
 * The text in an editable text field can be set through the API, and is
 * editable through the UI.
 * 
 */
 
class WgEditText : public WgModText
{
public:
	WgEditText( WgTextField * pField ) : WgModText(pField) {};

	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgEditTextPtr	Cast( const WgInterfacePtr& pInterface );				// Provided just for completeness sake.
	inline WgEditTextPtr	Ptr() { return WgEditTextPtr(_object(),this); }

	inline void			SetEditMode(WgTextEditMode mode) { m_pField->SetEditMode(mode); }
	inline WgTextEditMode	EditMode() const { return m_pField->EditMode(); }

	inline bool			IsEditable() const { return m_pField->IsEditable(); }
	inline bool			IsSelectable() const { return m_pField->IsSelectable(); }

	inline void			SetCursorSkin( const WgCaretPtr& pCursor ) { m_pField->SetCursorSkin(pCursor); }
	inline WgCaretPtr	CursorSkin() const { return m_pField->CursorSkin(); }

	inline int			InsertAtCursor( const WgCharSeq& str ) { return m_pField->InsertAtCursor(str); }
	inline bool			InsertAtCursor( Uint16 c ) { return m_pField->InsertAtCursor(c); }

	inline void			GoBOL() { m_pField->GoBOL(); }
	inline void			GoEOL() { m_pField->GoEOL(); }
	inline void			GoBOT() { m_pField->GoBOT(); }
	inline void			GoEOT() { m_pField->GoEOT(); }

	inline void			Select( int ofs, int len ) { m_pField->Select(ofs,len); }
	inline void			SelectAll() { m_pField->SelectAll(); }
	inline int			SelectionStart() const { return m_pField->SelectionStart(); }
	inline int			SelectionLength() const { return m_pField->SelectionLength(); }
	inline void			ClearSelection() { m_pField->ClearSelection(); }
	inline void			DeleteSelected() { m_pField->DeleteSelected(); }
};


#endif //WG_EDITTEXT_DOT_H
