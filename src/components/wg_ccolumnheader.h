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
	//____ CColumnHeader ___________________________________________________

	class CColumnHeader : public GeoComponent, protected GeoComponent::Holder   /** @private */
	{
	public:
		CColumnHeader(GeoComponent::Holder * pHolder);

		//____ Subcomponents __________________________________

		CIconDisplay		icon;
		CIconDisplay		arrow;
		CTextDisplay		label;

		//____ Methods _____________________________________

		void			setSortOrder( SortOrder order );
		inline SortOrder sortOrder() const { return m_sortOrder; }

		void			setSkin( Skin * pSkin );
		inline Skin_p	skin() const { return m_pSkin; }

		void			setState( State state );
		inline State	state() const { return m_state; }

		void			setSize( SizeI size );
		inline SizeI		size() const { return m_size; }

		SizeI			preferredSize() const;
		int				matchingWidth( int height ) const;
		int				matchingHeight( int width ) const;

		bool			receive( Msg * pMsg );

		void			render( GfxDevice * pDevice, const RectI& _canvas );


	protected:

		Object *		_object() override;
		const Object *	_object() const override;

		CoordI	_componentPos( const GeoComponent * pComponent ) const override;
		SizeI	_componentSize( const GeoComponent * pComponent ) const override;
		RectI	_componentGeo( const GeoComponent * pComponent ) const override;
		CoordI	_globalComponentPos( const GeoComponent * pComponent ) const override;
		RectI	_globalComponentGeo( const GeoComponent * pComponent ) const override;

		void	_componentRequestRender( const GeoComponent * pComponent ) override;
		void	_componentRequestRender( const GeoComponent * pComponent, const RectI& rect ) override;
		void	_componentRequestResize( const GeoComponent * pComponent ) override;

		void	_componentRequestFocus( const GeoComponent * pComponent ) override;
		void	_componentRequestInView( const GeoComponent * pComponent ) override;
		void	_componentRequestInView( const GeoComponent * pComponent, const RectI& mustHave, const RectI& niceToHave ) override;

		void	_receiveComponentNotif( GeoComponent * pComponent, ComponentNotif notification, int value, void * pData ) override;


		Skin_p			m_pSkin;

		State			m_state;
		SizeI			m_size;

		bool			m_bPressed;
		SortOrder		m_sortOrder;
	};



} // namespace wg
#endif //WG_CCOLUMNHEADER_DOT_H
