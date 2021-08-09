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
#ifndef WG_CCOLUMNHEADER_DOT_H
#define WG_CCOLUMNHEADER_DOT_H
#pragma once

#include <wg_cicondisplay.h>
#include <wg_ctextdisplay.h>

namespace wg
{
	class CColumnHeader;
	typedef	StrongComponentPtr<CColumnHeader>	CColumnHeader_p;
	typedef	WeakComponentPtr<CColumnHeader>		CColumnHeader_wp;
	 
	//____ CColumnHeader ___________________________________________________

	class CColumnHeader : public WidgetComponent
	{
	public:
		CColumnHeader(Widget * pWidget);

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance ____________________________________________

		void			setSkin(Skin * pSkin);
		inline Skin_p	skin() const { return m_pSkin; }

		//.____ Misc __________________________________________________

		inline CColumnHeader_p	ptr() { return CColumnHeader_p(this); }


	protected:
		void			_setSortOrder(SortOrder order);
		inline SortOrder _sortOrder() const { return m_sortOrder; }


		void			_setState(State state);
		inline State	_state() const { return m_state; }

		void			_setSize(SizeSPX size, int scale);
		inline SizeSPX	_size() const { return m_size; }

		SizeSPX			_preferredSize(int scale) const;
		spx				_matchingWidth(spx height, int scale) const;
		spx				_matchingHeight(spx width, int scale) const;

		bool			_receive(Msg * pMsg);

		void			_render(GfxDevice * pDevice, const RectSPX& _canvas);

		inline Object *			_object() override { return WidgetComponent::_object(); }
		inline const Object *	_object() const override { return WidgetComponent::_object(); }

	private:
		Skin_p			m_pSkin;

		State			m_state;
		SizeSPX			m_size;
		int				m_scale = 64;

		bool			m_bPressed;
		SortOrder		m_sortOrder;
	};



} // namespace wg
#endif //WG_CCOLUMNHEADER_DOT_H
