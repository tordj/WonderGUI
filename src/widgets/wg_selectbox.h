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
#ifndef WG_SELECTBOX_DOT_H
#define WG_SELECTBOX_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_ctextdisplay.h>
#include <wg_cvector.h>

namespace wg
{

	//____ SelectBoxEntry ________________________________________________________

	class SelectBoxEntry : private Text
	{
	public:
		SelectBoxEntry( int id, String& text ) : m_id(id), m_string(text) {};
		
		inline int		id() { return m_id; }
		inline String	text() { return m_string; }

	protected:
	
		int			m_id;
		String 		m_string;		
	};
		

	//____ SelectBox __________________________________________________________

	class SelectBox;
	typedef	StrongPtr<SelectBox>		SelectBox_p;
	typedef	WeakPtr<SelectBox>		SelectBox_wp;


	class SelectBox : public Widget, protected CVector<SelectBoxEntry>::Holder
	{
	public:
		//.____ Creation __________________________________________

		static SelectBox_p	create() { return SelectBox_p(new SelectBox()); }

		//.____ Components ____________________________________

		CTextDisplay			text;
		CVector<SelectBoxEntry>	entries;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static SelectBox_p	cast( Object * pObject );

		//.____ Appearance ________________________________________________________


	protected:
		SelectBox();
		virtual ~SelectBox();
		virtual Widget* _newOfMyType() const override { return new SelectBox(); };

		SizeI		_preferredSize() const override;
		void		_cloneContent( const Widget * _pOrg ) override;
		void		_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		bool		_alphaTest( const CoordI& ofs ) override;

		class TextAccess : public CTextDisplay { friend class SelectBox; };
		const TextAccess& _text() const { return static_cast<const TextAccess&>(text); }
		TextAccess& _text() { return static_cast<TextAccess&>(text); }

		//
		
		void	_didAddEntries(SelectBoxEntry * pEntry, int nb) override;
		void	_didMoveEntries(SelectBoxEntry * pFrom, SelectBoxEntry * pTo, int nb) override;
		void	_willEraseEntries(SelectBoxEntry * pSlot, int nb) override;

		//
		
		

	private:


	};




} // namespace wg
#endif //WG_SELECTBOX_DOT_H
