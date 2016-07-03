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

#ifndef WG_EDITTEXTITEM_DOT_H
#	include <wg_edittextitem.h>
#endif


namespace wg 
{
	
	class EditText;
	typedef	StrongInterfacePtr<EditText,ModText_p>	EditText_p;
	typedef	WeakInterfacePtr<EditText,ModText_wp>	EditText_wp;
	
	/**
	 * @brief Interface to a value display item with editable text.
	 *
	 * Interface to a value display item with editable text.
	 *
	 * The value in an editable value item can be set through the API, and is
	 * editable through the UI.
	 * 
	 */
	 
	class EditText : public ModText
	{
	public:
		EditText(EditTextItem * pItem) : ModText(pItem) {}
	
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static EditText_p		cast( const Interface_p& pInterface );				// Provided just for completeness sake.
		inline EditText_p		ptr() { return EditText_p(_object(),this); }

		void			setEditMode(TextEditMode mode);
		TextEditMode	editMode() const;
		inline bool		isSelectable() const;
		inline bool		isEditable() const;
		
		inline int		append( const CharSeq& seq );
		inline int		insert( int ofs, const CharSeq& seq );
		inline int		replace( int ofs, int nDelete, const CharSeq& seq );
		inline int		erase( int ofs, int len );
	
		// These methods will fail if editMode is Static

		inline bool		select( int begin, int end );		// Cursor (if enabled) gets end position. End can be smaller than begin.
		inline bool		selectAll();
		inline bool		unselect();
		inline int		eraseSelected();

		inline int		selectionBegin() const;
		inline int		selectionEnd() const;


		// These methods will fail unless caret is present

		inline bool		setCaretPos( int pos );			// Move cursor to position. Any selection will be unselected.
		inline int		caretPos() const;

		inline int		caretPut( const CharSeq& str );	// Will insert or overwrite depending on caret mode
		inline bool		caretPut( uint16_t c );			// " -

		inline bool		caretLineBegin();
		inline bool		caretLineEnd();
		inline bool		caretTextBegin();
		inline bool		caretTextEnd();


//		inline void		setMaxLines( int nLines ) { m_maxLines = nLines; }
//		inline int		maxLines() { return m_maxLines; }

	
	private:
		inline	EditTextItem * 	_item() { return static_cast<EditTextItem*>(m_pItem); }
		inline	const EditTextItem * 	_item() const { return static_cast<EditTextItem*>(m_pItem); }
	};
	
	
	//_____________________________________________________________________________
	inline bool EditText::isSelectable() const 
	{ 
		return !(_item()->editMode() == TextEditMode::Static); 
	}
	
	//_____________________________________________________________________________
	inline bool EditText::isEditable() const 
	{ 
		return (_item()->editMode() == TextEditMode::Editable); 
	}
	
	//_____________________________________________________________________________
	inline int EditText::append( const CharSeq& seq ) 
	{ 
		return _item()->append( seq );
	}

	//_____________________________________________________________________________
	inline int EditText::insert( int ofs, const CharSeq& seq )
	{
		return _item()->insert(ofs, seq);
	}
	
	//_____________________________________________________________________________
	inline int EditText::replace( int ofs, int nDelete, const CharSeq& seq )
	{
		return _item()->replace(ofs, nDelete, seq);
	}
	
	//_____________________________________________________________________________
	inline int EditText::erase( int ofs, int len )
	{
		return _item()->erase(ofs,len);
	}

	//_____________________________________________________________________________
	inline bool EditText::select( int begin, int end )
	{
		return _item()->select(begin,end);
	}
	
	//_____________________________________________________________________________
	inline bool EditText::selectAll()
	{
		return _item()->selectAll();
	}

	//_____________________________________________________________________________
	inline bool EditText::unselect()
	{
		return _item()->unselect();
	}

	//_____________________________________________________________________________
	inline int EditText::eraseSelected()
	{
		return _item()->eraseSelected();
	}

	//_____________________________________________________________________________
	inline int EditText::selectionBegin() const
	{
		return _item()->selectionBegin();
	}

	//_____________________________________________________________________________
	inline int EditText::selectionEnd() const
	{
		return _item()->selectionEnd();
	}
	
	//_____________________________________________________________________________
	inline bool EditText::setCaretPos( int pos )
	{
		return _item()->setCaretPos(pos);
	}

	//_____________________________________________________________________________
	inline int EditText::caretPos() const
	{
		return _item()->caretPos();
	}
	
	//_____________________________________________________________________________
	inline int EditText::caretPut( const CharSeq& seq )
	{
		return _item()->caretPut(seq);
	}
	
	//_____________________________________________________________________________
	inline bool EditText::caretPut( uint16_t c )
	{
		return _item()->caretPut(c);
	}
	
	//_____________________________________________________________________________
	inline bool EditText::caretLineBegin()
	{
		return _item()->caretLineBegin();
	}

	//_____________________________________________________________________________
	inline bool EditText::caretLineEnd()
	{
		return _item()->caretLineEnd();
	}

	//_____________________________________________________________________________
	inline bool EditText::caretTextBegin()
	{
		return _item()->caretTextBegin();
	}

	//_____________________________________________________________________________
	inline bool EditText::caretTextEnd()
	{
		return _item()->caretTextEnd();
	}


	
	
	

} // namespace wg
#endif //WG_EDITTEXT_DOT_H
