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

#ifndef WG_EDITVALUE_DOT_H
#define WG_EDITVALUE_DOT_H

#ifndef WG_MODVALUE_DOT_H
#	include <wg_modvalue.h>
#endif

#ifndef WG_EDITVALUEFIELD_DOT_H
#	include <wg_editvaluefield.h>
#endif


#ifndef WG_CARET2_DOT_H
#	include <wg_caret2.h>
#endif

namespace wg 
{
	
	class EditValue;
	typedef	StrongInterfacePtr<EditValue,ModValue_p>		EditValue_p;
	typedef	WeakInterfacePtr<EditValue,ModValue_wp>	EditValue_wp;
	
	/**
	 * @brief Interface to a value display field with editable text.
	 *
	 * Interface to a value display field with editable text.
	 *
	 * The value in an editable value field can be set through the API, and is
	 * editable through the UI.
	 * 
	 */
	 
	class EditValue : public ModValue
	{
	public:
		EditValue(EditValueField * pField) : ModValue(pField) {}
	
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static EditValue_p		cast( const Interface_p& pInterface );				// Provided just for completeness sake.
		inline EditValue_p		ptr() { return EditValue_p(_object(),this); }
	
		virtual void			setEditMode(TextEditMode mode) = 0;
		virtual TextEditMode	editMode() const = 0;
	
		virtual bool			isEditable() const = 0;
		virtual bool			isSelectable() const = 0;
	
		virtual void			setCaret( const Caret2_p& pCaret ) = 0;
		virtual Caret2_p		caret() const = 0;
	
		// Calling these methods gets field into edit mode, displaying cursor.
	
		virtual int				insertAtCaret( const CharSeq& str ) = 0;
		virtual bool			insertAtCaret( uint16_t c ) = 0;
	
		virtual int				append( const CharSeq& seq ) = 0;
		virtual int				insert( int ofs, const CharSeq& seq ) = 0;
		virtual int				replace( int ofs, int nDelete, const CharSeq& seq ) = 0;
		virtual int				erase( int ofs, int len ) = 0;
		virtual void			eraseSelected() = 0;
	
		virtual void			select( int ofs, int len ) = 0;
		virtual void			selectAll() = 0;
		virtual int				selectionBegin() const = 0;
		virtual int				selectionLength() const = 0;
		virtual void			clearSelection() = 0;
	
		virtual void			goBol() = 0;
		virtual void			goEol() = 0;
		virtual void			goBof() = 0;
		virtual void			goEof() = 0;
	
	private:
		inline	EditValueField * 	_field() { return static_cast<EditValueField*>(m_pField); }
	};
	
	

} // namespace wg
#endif //WG_EDITVALUE_DOT_H
