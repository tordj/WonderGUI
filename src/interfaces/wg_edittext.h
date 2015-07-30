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

#ifndef WG_LEGACYMODTEXT_DOT_H
#	include <wg_legacymodtext.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

class WgEditText;
typedef	WgIStrongPtr<WgEditText,WgLegacyModTextPtr>		WgEditTextPtr;
typedef	WgIWeakPtr<WgEditText,WgLegacyModTextWeakPtr>		WgEditTextWeakPtr;

/**
 * @brief Interface to a text field with editable text.
 *
 * Interface to a text field with editable text.
 * The text in an editable text field can be set through the API, and is
 * editable through the UI.
 * 
 */
 
class WgEditText : public WgLegacyModText
{
public:
	WgEditText( WgLegacyTextField * pField ) : WgLegacyModText(pField) {};

	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgEditTextPtr	cast( const WgInterfacePtr& pInterface );				// Provided just for completeness sake.
	inline WgEditTextPtr	ptr() { return WgEditTextPtr(_object(),this); }

	inline void			setEditMode(WgTextEditMode mode) { m_pField->setEditMode(mode); }
	inline WgTextEditMode	editMode() const { return m_pField->editMode(); }

	inline bool			isEditable() const { return m_pField->isEditable(); }
	inline bool			isSelectable() const { return m_pField->isSelectable(); }

	inline void			setCursorSkin( const WgCaretPtr& pCursor ) { m_pField->setCursorSkin(pCursor); }
	inline WgCaretPtr	cursorSkin() const { return m_pField->cursorSkin(); }

	inline int			insertAtCursor( const WgCharSeq& str ) { return m_pField->insertAtCursor(str); }
	inline bool			insertAtCursor( Uint16 c ) { return m_pField->insertAtCursor(c); }

	inline void			goBol() { m_pField->goBol(); }
	inline void			goEol() { m_pField->goEol(); }
	inline void			goBot() { m_pField->goBot(); }
	inline void			goEot() { m_pField->goEot(); }

	inline void			select( int ofs, int len ) { m_pField->select(ofs,len); }
	inline void			selectAll() { m_pField->selectAll(); }
	inline int			selectionStart() const { return m_pField->selectionStart(); }
	inline int			selectionLength() const { return m_pField->selectionLength(); }
	inline void			clearSelection() { m_pField->clearSelection(); }
	inline void			deleteSelected() { m_pField->deleteSelected(); }
};


#endif //WG_EDITTEXT_DOT_H
