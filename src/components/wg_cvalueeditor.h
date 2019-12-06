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
#ifndef	WG_CVALUEEDITOR_DOT_H
#define WG_CVALUEEDITOR_DOT_H
#pragma once

#include <wg_valuedisplay.h>
#include <wg_caret.h>

namespace wg
{

	//____ CValueEditor ____________________________________________________________

	class CValueEditor : public CValueDisplay	/** @private */
	{
	public:
		CValueEditor(GeoComponent::Holder * pHolder );
		~CValueEditor();

		void				setEditMode(TextEditMode mode);
		inline TextEditMode editMode() const { return m_editMode; }

		inline bool			isEditable() const { return m_editMode == TextEditMode::Editable; }
		inline bool			isSelectable() const { return m_editMode != TextEditMode::Static; }

		void				setCaret( Caret * pCaret );
		inline Caret_p		caret()
		const { return m_pCaret; }

		int					insertAtCaret( const CharSeq& str );
		bool				insertAtCaret( uint16_t c );

		int					append( const CharSeq& seq );
		int					insert( int ofs, const CharSeq& seq );
		int					replace( int ofs, int nDelete, const CharSeq& seq );
		int					erase( int ofs, int len );
		void				eraseSelected();

		void				select( int ofs, int len );
		void				selectAll();
		void				clearSelection();
		inline int			selectionBegin() const override { return m_selBeg; }
		inline int			selectionEnd() const override { return m_selEnd; }
		inline int			selectionSize() const override { return m_selEnd - m_selBeg; }

		void				goBol();
		void				goEol();
		void				goBof();
		void				goEof();

	protected:
		void				_onValueEdited() { _notify( ComponentNotif::ValueEdited, 0, nullptr ); }

		TextEditMode		m_editMode;
		Caret_p				m_pCaret;
		int					m_caretOfs;		// -1 = No caret.
		int					m_selBeg;
		int					m_selEnd;
	};



} // namespace wg
#endif //WG_CVALUEEDITOR_DOT_H
