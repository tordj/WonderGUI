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

#ifndef	WG_CTEXTEDITOR_DOT_H
#define WG_CTEXTEDITOR_DOT_H
#pragma once


#include <wg_ctextdisplay.h>

namespace wg
{

	class CTextEditor;
	typedef	StrongComponentPtr<CTextEditor>	CTextEditor_p;
	typedef	WeakComponentPtr<CTextEditor>	CTextEditor_wp;


	//____ CTextEditor __________________________________________________________________

	class CTextEditor : public CTextDisplay	/** @private */
	{
	public:

		CTextEditor(Holder * pHolder );

		//.____ State __________________________________________________

		void			setEditMode(TextEditMode mode);
		TextEditMode	editMode() const { return m_editMode; };
		inline bool		isSelectable() const { return !(editMode() == TextEditMode::Static); }
		inline bool		isEditable() const { return editMode() == TextEditMode::Editable; }

		//.____ Content _____________________________________________

		int				caretPut(const CharSeq& seq);	// Will insert or overwrite depending on caret mode
		bool			caretPut(uint16_t c);			// " -

		inline int		eraseSelected();

		bool			setMaxLines(int maxLines);
		bool			setMaxChars(int maxChars);
		int				maxLines() const { return m_maxLines; }
		int				maxChars() const { return m_maxChars; }

		//.____ Control _____________________________________________

		// These methods will fail if editMode is Static

		bool			select(int begin, int end);			// Cursor (if enabled) gets end position. End can be smaller than begin.
		bool			selectAll();
		bool			unselect();

		inline bool		hasSelection() const { return m_editState.caretOfs != m_editState.selectOfs; }

		int				selectionBegin() const;				// Begin position of selection, might be after end position.
		int				selectionEnd() const;
		int				selectionSize() const;				// Number of characters that are selected.


		// These methods will fail unless caret is present

		bool			setCaretOfs(int ofs);			// Move cursor to offset. Any selection will be unselected.
		int				caretOfs() const;

		bool			caretLineBegin();
		bool			caretLineEnd();
		bool			caretTextBegin();
		bool			caretTextEnd();

		//.____ Misc __________________________________________________

		inline CTextEditor_p		ptr() { return CTextEditor_p(this); }


	protected:
		void			_receive( Msg * pMsg ) override;

		// Overloaded so we can update caret and selection

		void			_clear() override;

		void			_set(const CharSeq& seq) override;
		void			_set(const CharBuffer * buffer) override;
		void			_set(const String& str) override;

		int				_append(const CharSeq& seq) override;
		int				_insert(int ofs, const CharSeq& seq) override;
		int				_replace(int ofs, int nDelete, const CharSeq& seq) override;
		int				_erase(int ofs, int len) override;


		void			_setState( State state ) override;
		void			_render( GfxDevice * pDevice, const RectI& _canvas ) override;

		//

		bool			caretUp();
		bool			caretDown();
		bool			caretLeft();
		bool			caretRight();

		bool			caretNextWord();
		bool			caretPrevWord();

		bool			caretEraseNextChar();
		bool			caretErasePrevChar();
		bool			caretEraseNextWord();
		bool			caretErasePrevWord();

		inline bool		caretToPos(Coord pos) { return _caretToPos(Util::qpixToRaw(pos)); }			// Move caret as close as possible to the given position.
		bool			caretSelectWord();
		bool			caretSelectLine();				// Full line with hard line ending.

		//





	protected:

		enum class MoveMethod
		{
			Keyboard,
			Mouse,
			ApiCall
		};


		bool				_moveCaret( int caretOfs, MoveMethod method );
		void				_updateDisplayArea();
		void				_updateInsertStyle();

		const EditState * 	_editState() const override;


		void			_caretToEnd();
		bool			_caretToPos(CoordI pos);

		TextEditMode	m_editMode;
		EditState		m_editState;
		int				m_maxLines;
		int				m_maxChars;
	};

} // namespace wg
#endif //WG_CTEXTEDITOR_DOT_H
