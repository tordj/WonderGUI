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
#ifndef	WG_EDITVALUEFIELD_DOT_H
#define WG_EDITVALUEFIELD_DOT_H

#ifndef WG_MODIFVALUEFIELD_DOT_H
#	include <wg_modifvaluefield.h>
#endif

class WgEditValueField;

//____ WgEditValueHolder ___________________________________________________________

class WgEditValueHolder : public WgModifValueHolder
{
public:
	virtual void		_onValueEdited( WgEditValueField * pField ) = 0;
};

class WgEditValueField;
typedef	WgCompStrongPtr<WgEditValueField,WgModifValueFieldPtr>			WgEditValueFieldPtr;
typedef	WgCompWeakPtr<WgEditValueField,WgModifValueFieldWeakPtr>		WgEditValueFieldWeakPtr;

//____ WgEditValueField ____________________________________________________________

class WgEditValueField : public WgModifValueField
{
public:
	WgEditValueField();
	~WgEditValueField();

	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgEditValueFieldPtr	Cast( const WgComponentPtr& pComponent );

	WgEditValueFieldPtr	Ptr() { return WgEditValueFieldPtr(this); }


	void				SetEditMode(WgTextEditMode mode);
	inline WgTextEditMode EditMode() const { return m_editMode; }

	inline bool			IsEditable() const { return m_editMode == WG_TEXT_EDITABLE; }
	inline bool			IsSelectable() const { return m_editMode != WG_TEXT_STATIC; }

	void				SetCursorSkin( const WgCaretPtr& pCursor );
	inline WgCaretPtr	CursorSkin() const { return m_pCursorSkin; }

	int					InsertAtCursor( const WgCharSeq& str );
	bool				InsertAtCursor( Uint16 c );

	int					Append( const WgCharSeq& seq );
	int					Insert( int ofs, const WgCharSeq& seq );
	int					Replace( int ofs, int nDelete, const WgCharSeq& seq );
	int					Delete( int ofs, int len );
	void				DeleteSelected();

	void				Select( int ofs, int len );
	void				SelectAll();
	inline int			SelectionStart() const { return m_selBeg; }
	inline int			SelectionLength() const { return m_selEnd - m_selBeg; }
	void				ClearSelection();

	void				GoBOL();
	void				GoEOL();
	void				GoBOF();
	void				GoEOF();

protected:
	void 				_setHolder( WgEditValueHolder * pHolder ) { m_pHolder = pHolder; }
	void				_onValueEdited() { static_cast<WgEditValueHolder*>(m_pHolder)->_onValueEdited(this); }
	
	WgTextEditMode		m_editMode;
	WgCaretPtr			m_pCursorSkin;
	int					m_cursorOfs;		// -1 = No cursor.
	int					m_selBeg;
	int					m_selEnd;
};


#endif //WG_EDITVALUEFIELD_DOT_H
