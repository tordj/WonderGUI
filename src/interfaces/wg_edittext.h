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

#ifndef WG_EDITTEXTFIELD_DOT_H
#	include <wg_edittextfield.h>
#endif


#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

namespace wg 
{
	
	class EditText;
	typedef	StrongInterfacePtr<EditText,ModText_p>	EditText_p;
	typedef	WeakInterfacePtr<EditText,ModText_wp>	EditText_wp;
	
	/**
	 * @brief Interface to a value display field with editable text.
	 *
	 * Interface to a value display field with editable text.
	 *
	 * The value in an editable value field can be set through the API, and is
	 * editable through the UI.
	 * 
	 */
	 
	class EditText : public ModText
	{
	public:
		EditText(EditTextField * pField) : ModText(pField) {}
	
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static EditText_p		cast( const Interface_p& pInterface );				// Provided just for completeness sake.
		inline EditText_p		ptr() { return EditText_p(_object(),this); }
/*	
		void			setEditMode(TextEditMode mode);
		TextEditMode	editMode() const;
	
		bool			isEditable() const;
		bool			isSelectable() const;
	
		void			setCaret( const Caret_p& pCaret );
		Caret_p			caret() const;
	
		// Calling these methods gets field into edit mode, displaying caret.
	
		int				insertAtCaret( const CharSeq& str );
		bool			insertAtCaret( uint16_t c );
	
		int				append( const CharSeq& seq );
		int				insert( int ofs, const CharSeq& seq );
		int				replace( int ofs, int nDelete, const CharSeq& seq );
		int				erase( int ofs, int len );
		void			eraseSelected();
	
		void			select( int ofs, int len );
		void			selectAll();
		int				selectionBegin() const;
		int				selectionLength() const;
		void			clearSelection();
	
		void			goBol();
		void			goEol();
		void			goBof();
		void			goEof();
*/

//		inline void		setMaxLines( int nLines ) { m_maxLines = nLines; }
//		inline int		maxLines() { return m_maxLines; }

	
	private:
		inline	EditTextField * 	_field() { return static_cast<EditTextField*>(m_pField); }
	};
	
	

} // namespace wg
#endif //WG_EDITTEXT_DOT_H
