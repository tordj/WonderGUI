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

	//____ matchingHeight() _________________________________________________________________
	
	MU SideCanvas::matchingHeight(MU width) const
	{
		return m_pHolder->_sideCanvasMatchingHeight(this,width);
	}
	
	//____ matchingWidth() _________________________________________________________________

	MU SideCanvas::matchingWidth(MU height) const
	{
		return m_pHolder->_sideCanvasMatchingWidth(this,height);
	}

	//____ preferredSize() _________________________________________________________________

	Size SideCanvas::preferredSize() const
	{
		return m_pHolder->_sideCanvasPreferredSize(this);
	}
	
	//____ minSize() _________________________________________________________________
	
	Size SideCanvas::minSize() const
	{
		return m_pHolder->_sideCanvasMinSize(this);
	}
	
	//____ maxSize() _________________________________________________________________
	
	Size SideCanvas::maxSize() const
	{
		return m_pHolder->_sideCanvasMaxSize(this);
	}

	//____ _collectPatches() _________________________________________________________________

	void SideCanvas::_collectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		m_pHolder->_sideCanvasCollectPatches(this,container, geo, clip);
	}

	//____ _maskPatches() _________________________________________________________________

	void SideCanvas::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		m_pHolder->_sideCanvasMaskPatches(this,patches,geo,clip,blendMode);
	}

	//____ _render() _________________________________________________________________

	void SideCanvas::_render( GfxDevice * pDevice, const Rect& canvas, const Rect& window )
	{
		m_pHolder->_sideCanvasRender(this,pDevice,canvas,window);
	}

	//____ _refresh() _________________________________________________________________

	void SideCanvas::_refresh()
	{
		m_pHolder->_sideCanvasRefresh(this);
	}
	
	//____ _resize() _________________________________________________________________
	
	void SideCanvas::_resize( const Size& size )
	{
		m_pHolder->_sideCanvasResize(this,size);
	}
	
	//____ _setSkin() _________________________________________________________________
	
	void SideCanvas::_setSkin( Skin * pSkin )
	{
		m_pHolder->_sideCanvasSetSkin(this,pSkin);		
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
	
	bool SideCanvas::_alphaTest( const Coord& ofs )
	{
		return m_pHolder->_sideCanvasAlphaTest(this,ofs);		
	}
	
	//____ _Holder::_sideCanvasMatchingHeight() ____________________________________________
	
	MU SideCanvas::Holder::_sideCanvasMatchingHeight( const SideCanvas * pCanvas, MU width ) const
	{
		return _sideCanvasPreferredSize(pCanvas).h;
	}
	
	//____ _Holder::_sideCanvasMatchingWidth() ____________________________________________

	MU SideCanvas::Holder::_sideCanvasMatchingWidth( const SideCanvas * pCanvas, MU height ) const
	{
		return _sideCanvasPreferredSize(pCanvas).w;		
	}

	//____ _Holder::_sideCanvasMinSize() ____________________________________________

	Size SideCanvas::Holder::_sideCanvasMinSize( const SideCanvas * pCanvas ) const
	{
		return pCanvas->Widget::minSize();
	}
	
	//____ _Holder::_sideCanvasMaxSize() ____________________________________________

	Size SideCanvas::Holder::_sideCanvasMaxSize( const SideCanvas * pCanvas ) const
	{
		return pCanvas->Widget::maxSize();
	}

	//____ _Holder::_sideCanvasCollectPatches() ____________________________________________

	void SideCanvas::Holder::_sideCanvasCollectPatches( SideCanvas * pCanvas, Patches& container, const Rect& geo, const Rect& clip )
	{
		return pCanvas->Widget::_collectPatches(container, geo, clip);
	}

	//____ _Holder::_sideCanvasMaskPatches() ____________________________________________

	void SideCanvas::Holder::_sideCanvasMaskPatches( SideCanvas * pCanvas, Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
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
		return pCanvas->Widget::_setSkin(pSkin);
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