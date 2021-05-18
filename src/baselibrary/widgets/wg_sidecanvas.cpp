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

#include <wg_sidecanvas.h>

namespace wg
{
	using namespace Util;

	const TypeInfo SideCanvas::TYPEINFO = { "SideCanvas", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	SideCanvas::SideCanvas( Holder * pHolder ) : m_pHolder(pHolder)
	{
	}

	//____ Destructor _____________________________________________________________

	SideCanvas::~SideCanvas()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SideCanvas::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _matchingHeight() _________________________________________________________________
	
	spx SideCanvas::_matchingHeight(spx width, int scale) const
	{
		return m_pHolder->_sideCanvasMatchingHeight(this,width,scale);
	}
	
	//____ _matchingWidth() _________________________________________________________________

	spx SideCanvas::_matchingWidth(spx height, int scale) const
	{
		return m_pHolder->_sideCanvasMatchingWidth(this,height,scale);
	}

	//____ _preferredSize() _________________________________________________________________

	SizeSPX SideCanvas::_preferredSize(int scale) const
	{
		return m_pHolder->_sideCanvasPreferredSize(this, scale);
	}
	
	//____ _minSize() _________________________________________________________________
	
	SizeSPX SideCanvas::_minSize(int scale) const
	{
		return m_pHolder->_sideCanvasMinSize(this,scale);
	}
	
	//____ _maxSize() _________________________________________________________________
	
	SizeSPX SideCanvas::_maxSize(int scale) const
	{
		return m_pHolder->_sideCanvasMaxSize(this,scale);
	}

	//____ _collectPatches() _________________________________________________________________

	void SideCanvas::_collectPatches( Patches& container, const RectSPX& geo, const RectSPX& clip )
	{
		m_pHolder->_sideCanvasCollectPatches(this,container, geo, clip);
	}

	//____ _maskPatches() _________________________________________________________________

	void SideCanvas::_maskPatches( Patches& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode )
	{
		m_pHolder->_sideCanvasMaskPatches(this,patches,geo,clip,blendMode);
	}

	//____ _render() _________________________________________________________________

	void SideCanvas::_render( GfxDevice * pDevice, const RectSPX& canvas, const RectSPX& window )
	{
		m_pHolder->_sideCanvasRender(this,pDevice,canvas,window);
	}

	//____ _refresh() _________________________________________________________________

	void SideCanvas::_refresh()
	{
		m_pHolder->_sideCanvasRefresh(this);
	}
	
	//____ _resize() _________________________________________________________________
	
	void SideCanvas::_resize( const SizeSPX& size, int scale )
	{
		m_pHolder->_sideCanvasResize(this,size,scale);
	}
			
	//____ _setState() _________________________________________________________________
	
	void SideCanvas::_setState( State state )
	{
		m_pHolder->_sideCanvasSetState(this,state);		
	}

	//____ _receive() _________________________________________________________________

	void SideCanvas::_receive( Msg * pMsg )
	{
		m_pHolder->_sideCanvasReceive(this,pMsg);
	}

	//____ _alphaTest() _________________________________________________________________
	
	bool SideCanvas::_alphaTest( const CoordSPX& ofs )
	{
		return m_pHolder->_sideCanvasAlphaTest(this,ofs);		
	}
	
	//____ _Holder::_sideCanvasMatchingHeight() ____________________________________________
	
	spx SideCanvas::Holder::_sideCanvasMatchingHeight( const SideCanvas * pCanvas, spx width, int scale ) const
	{
		return _sideCanvasPreferredSize(pCanvas,scale).h;
	}
	
	//____ _Holder::_sideCanvasMatchingWidth() ____________________________________________

	spx SideCanvas::Holder::_sideCanvasMatchingWidth( const SideCanvas * pCanvas, spx height, int scale ) const
	{
		return _sideCanvasPreferredSize(pCanvas,scale).w;		
	}

	//____ _Holder::_sideCanvasMinSize() ____________________________________________

	SizeSPX SideCanvas::Holder::_sideCanvasMinSize( const SideCanvas * pCanvas, int scale ) const
	{
		return pCanvas->Widget::_minSize(scale);
	}
	
	//____ _Holder::_sideCanvasMaxSize() ____________________________________________

	SizeSPX SideCanvas::Holder::_sideCanvasMaxSize( const SideCanvas * pCanvas, int scale ) const
	{
		return pCanvas->Widget::_maxSize(scale);
	}

	//____ _Holder::_sideCanvasCollectPatches() ____________________________________________

	void SideCanvas::Holder::_sideCanvasCollectPatches( SideCanvas * pCanvas, Patches& container, const RectSPX& geo, const RectSPX& clip )
	{
		return pCanvas->Widget::_collectPatches(container, geo, clip);
	}

	//____ _Holder::_sideCanvasMaskPatches() ____________________________________________

	void SideCanvas::Holder::_sideCanvasMaskPatches( SideCanvas * pCanvas, Patches& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode )
	{
		return pCanvas->Widget::_maskPatches(patches,geo,clip,blendMode);
	}

	//____ _Holder::_sideCanvasRefresh() ____________________________________________

	void SideCanvas::Holder::_sideCanvasRefresh( SideCanvas * pCanvas)
	{
		return pCanvas->Widget::_refresh();
	}

	//____ _Holder::_sideCanvasSetSkin() ____________________________________________

	void SideCanvas::Holder::_sideCanvasSetSkin( SideCanvas * pCanvas,  Skin * pSkin ) 
	{
		pCanvas->skin = pSkin;
	}

	//____ _Holder::_sideCanvasSetState() ____________________________________________

	void SideCanvas::Holder::_sideCanvasSetState( SideCanvas * pCanvas,  State state )
	{
		return pCanvas->Widget::_setState(state);
	}

	//____ _Holder::_sideCanvasReceive() ____________________________________________

	void SideCanvas::Holder::_sideCanvasReceive( SideCanvas * pCanvas,  Msg * pMsg )
	{
		return pCanvas->Widget::_receive(pMsg);
	}

	
}