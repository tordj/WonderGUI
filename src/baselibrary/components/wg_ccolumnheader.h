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

	class CColumnHeader : public GeoComponent, protected GeoComponent::Holder
	{
	public:
		CColumnHeader(GeoComponent::Holder * pHolder);

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Components ___________________________________________

		CIconDisplay		icon;
		CIconDisplay		arrow;
		CTextDisplay		text;

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

		void			_setSize(Size size);
		inline Size		_size() const { return m_size; }

		Size			_preferredSize() const;
		MU				_matchingWidth(MU height) const;
		MU				_matchingHeight(MU width) const;

		bool			_receive(Msg * pMsg);

		void			_render(GfxDevice * pDevice, const Rect& _canvas);

		inline Object *			_object() override { return GeoComponent::_object(); }
		inline const Object *	_object() const override { return GeoComponent::_object(); }


		State	_componentState(const GeoComponent* pComponent) const override { return StateEnum::Normal; }
		Coord	_componentPos( const GeoComponent * pComponent ) const override;
		Size	_componentSize( const GeoComponent * pComponent ) const override;
		Rect	_componentGeo( const GeoComponent * pComponent ) const override;
		Coord	_globalComponentPos( const GeoComponent * pComponent ) const override;
		Rect	_globalComponentGeo( const GeoComponent * pComponent ) const override;

		void	_componentRequestRender( const GeoComponent * pComponent ) override;
		void	_componentRequestRender( const GeoComponent * pComponent, const Rect& rect ) override;
		void	_componentRequestResize( const GeoComponent * pComponent ) override;

		void	_componentRequestFocus( const GeoComponent * pComponent ) override;
		void	_componentRequestInView( const GeoComponent * pComponent ) override;
		void	_componentRequestInView( const GeoComponent * pComponent, const Rect& mustHave, const Rect& niceToHave ) override;

		void	_receiveComponentNotif( GeoComponent * pComponent, ComponentNotif notification, int value, void * pData ) override;

		class IconAccess : public CIconDisplay { friend class CColumnHeader; };
		const IconAccess& _icon() const { return static_cast<const IconAccess&>(icon); }
		IconAccess& _icon() { return static_cast<IconAccess&>(icon); }
		const IconAccess& _arrow() const { return static_cast<const IconAccess&>(arrow); }
		IconAccess& _arrow() { return static_cast<IconAccess&>(arrow); }

	private:
		Skin_p			m_pSkin;

		State			m_state;
		Size			m_size;

		bool			m_bPressed;
		SortOrder		m_sortOrder;
	};



} // namespace wg
#endif //WG_CCOLUMNHEADER_DOT_H
