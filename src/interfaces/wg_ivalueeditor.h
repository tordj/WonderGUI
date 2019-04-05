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

#ifndef WG_IVALUEEDITOR_DOT_H
#define WG_IVALUEEDITOR_DOT_H
#pragma once

#include <wg_ivaluedisplay.h>
#include <wg_cvalueeditor.h>


namespace wg
{

	class IValueEditor;
	typedef	StrongInterfacePtr<IValueEditor>	IValueEditor_p;
	typedef	WeakInterfacePtr<IValueEditor>	IValueEditor_wp;

	/**
	 * @brief Interface for a ValueEditor component.
	 *
	 * This interface provides access to modify the value and appearance of
	 * a ValueEditor component.
	 *
	 */

	class IValueEditor : public IValueDisplay
	{
	public:
		/** @private */

		IValueEditor(CValueEditor * pComponent) : IValueDisplay(pComponent) {}

		//.____ State __________________________________________________

		virtual void			setEditMode(TextEditMode mode) = 0;
		virtual TextEditMode	editMode() const = 0;

		virtual bool			isEditable() const = 0;
		virtual bool			isSelectable() const = 0;

		//.____ Content _____________________________________________

		// Calling these methods gets component into edit mode, displaying caret.

		virtual int				insertAtCaret( const CharSeq& str ) = 0;
		virtual bool			insertAtCaret( uint16_t c ) = 0;

		virtual int				append( const CharSeq& seq ) = 0;
		virtual int				insert( int ofs, const CharSeq& seq ) = 0;
		virtual int				replace( int ofs, int nDelete, const CharSeq& seq ) = 0;
		virtual int				erase( int ofs, int len ) = 0;
		virtual void			eraseSelected() = 0;

		//.____ Control _____________________________________________

		virtual void			select( int ofs, int len ) = 0;
		virtual void			selectAll() = 0;
		virtual int				selectionBegin() const = 0;
		virtual int				selectionLength() const = 0;
		virtual void			clearSelection() = 0;

		virtual void			goBol() = 0;
		virtual void			goEol() = 0;
		virtual void			goBof() = 0;
		virtual void			goEof() = 0;

		//.____ Misc __________________________________________________

		inline IValueEditor_p		ptr() { return IValueEditor_p(this); }


	private:
		inline	CValueEditor * 	_comp() { return static_cast<CValueEditor*>(m_pComponent); }
	};



} // namespace wg
#endif //WG_IVALUEEDITOR_DOT_H
