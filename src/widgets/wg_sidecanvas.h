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

#ifndef	WG_SIDECANVAS_DOT_H
#define	WG_SIDECANVAS_DOT_H
#pragma once


#include <wg_widget.h>

namespace wg
{
	
	
	class SideCanvas;
	typedef	StrongPtr<SideCanvas>	SideCanvas_p;
	typedef	WeakPtr<SideCanvas>		SideCanvas_wp;
	
	
	class SideCanvas : public Widget
	{
	public:

		//____ Holder _____________________________________________________
	
		class Holder		 /** @private */
		{
			friend class SideCanvas;
		protected:

			virtual MU			_sideCanvasMatchingHeight( const SideCanvas * pCanvas, MU width ) const;
			virtual MU			_sideCanvasMatchingWidth( const SideCanvas * pCanvas, MU height ) const;

			virtual Size		_sideCanvasPreferredSize( const SideCanvas * pCanvas ) const = 0;
			virtual Size		_sideCanvasMinSize( const SideCanvas * pCanvas ) const;
			virtual Size		_sideCanvasMaxSize( const SideCanvas * pCanvas ) const;

			virtual void		_sideCanvasCollectPatches( SideCanvas * pCanvas, Patches& container, const Rect& geo, const Rect& clip );
			virtual void		_sideCanvasMaskPatches( SideCanvas * pCanvas, Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode );

			virtual void		_sideCanvasRender( SideCanvas * pCanvas, GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) = 0;

			virtual void		_sideCanvasRefresh( SideCanvas * pCanvas);
			virtual void		_sideCanvasResize( SideCanvas * pCanvas, const Size& size ) = 0;
			virtual void		_sideCanvasSetSkin( SideCanvas * pCanvas,  Skin * pSkin ) ;
			virtual void		_sideCanvasSetState( SideCanvas * pCanvas,  State state );

			virtual void		_sideCanvasReceive( SideCanvas * pCanvas,  Msg * pMsg );
			virtual	bool		_sideCanvasAlphaTest( SideCanvas * pCanvas,  const Coord& ofs ) = 0;			
		};

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		MU			matchingHeight(MU width) const override;
		MU			matchingWidth(MU height) const override;

		Size		preferredSize() const override;
		Size		minSize() const override;
		Size		maxSize() const override;


	protected:
	
		SideCanvas( Holder * pHolder );
		virtual ~SideCanvas();
		virtual Widget* _newOfMyType() const override { return nullptr; };

		void		_collectPatches( Patches& container, const Rect& geo, const Rect& clip ) override;
		void		_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode ) override;

		void		_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;

		void		_refresh() override;
		void		_resize( const Size& size ) override;
		void		_setSkin( Skin * pSkin ) override;
		void		_setState( State state ) override;

		void		_receive( Msg * pMsg ) override;
		bool		_alphaTest( const Coord& ofs ) override;

		Holder * 	m_pHolder;
	};
	
	
	
} // namespace wg




#endif // WG_SIDECANVAS_DOT_H
