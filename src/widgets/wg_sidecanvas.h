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

			virtual int			_sideCanvasMatchingHeight( const SideCanvas * pCanvas, int width ) const;
			virtual int			_sideCanvasMatchingWidth( const SideCanvas * pCanvas, int height ) const;

			virtual SizeI		_sideCanvasPreferredSize( const SideCanvas * pCanvas ) const = 0;
			virtual SizeI		_sideCanvasMinSize( const SideCanvas * pCanvas ) const;
			virtual SizeI		_sideCanvasMaxSize( const SideCanvas * pCanvas ) const;

			virtual void		_sideCanvasCollectPatches( SideCanvas * pCanvas, Patches& container, const RectI& geo, const RectI& clip );
			virtual void		_sideCanvasMaskPatches( SideCanvas * pCanvas, Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode );

			virtual void		_sideCanvasRender( SideCanvas * pCanvas, GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) = 0;

			virtual void		_sideCanvasRefresh( SideCanvas * pCanvas);
			virtual void		_sideCanvasResize( SideCanvas * pCanvas, const SizeI& size ) = 0;
			virtual void		_sideCanvasSetSkin( SideCanvas * pCanvas,  Skin * pSkin ) ;
			virtual void		_sideCanvasSetState( SideCanvas * pCanvas,  State state );

			virtual void		_sideCanvasReceive( SideCanvas * pCanvas,  Msg * pMsg );
			virtual	bool		_sideCanvasAlphaTest( SideCanvas * pCanvas,  const CoordI& ofs ) = 0;			
		};

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const override;
		const char *			className( void ) const override;
		static const char		CLASSNAME[];


	protected:
	
		SideCanvas( Holder * pHolder );
		virtual ~SideCanvas();
		virtual Widget* _newOfMyType() const override { return nullptr; };

		int			_matchingHeight(int width) const override;
		int			_matchingWidth(int height) const override;

		SizeI		_preferredSize() const override;
		SizeI		_minSize() const override;
		SizeI		_maxSize() const override;

		void		_collectPatches( Patches& container, const RectI& geo, const RectI& clip ) override;
		void		_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode ) override;

		void		_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;

		void		_refresh() override;
		void		_resize( const SizeI& size ) override;
		void		_setSkin( Skin * pSkin ) override;
		void		_setState( State state ) override;

		void		_receive( Msg * pMsg ) override;
		bool		_alphaTest( const CoordI& ofs ) override;

		Holder * 	m_pHolder;
	};
	
	
	
} // namespace wg




#endif // WG_SIDECANVAS_DOT_H
