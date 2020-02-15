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

	const char SideCanvas::CLASSNAME[] = {"SideCanvas"};

	//____ Constructor ____________________________________________________________

	SideCanvas::SideCanvas( Holder * pHolder ) : m_pHolder(pHolder)
	{
	}

	//____ Destructor _____________________________________________________________

	SideCanvas::~SideCanvas()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool SideCanvas::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * SideCanvas::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	SideCanvas_p SideCanvas::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SideCanvas_p( static_cast<SideCanvas*>(pObject) );

		return 0;
	}

	//____ _matchingHeight() _________________________________________________________________
	
	int SideCanvas::_matchingHeight(int width) const
	{
		return m_pHolder->_sideCanvasMatchingHeight(this,width);
	}
	
	//____ _matchingWidth() _________________________________________________________________

	int SideCanvas::_matchingWidth(int height) const
	{
		return m_pHolder->_sideCanvasMatchingWidth(this,height);
	}

	//____ _preferredSize() _________________________________________________________________

	SizeI SideCanvas::_preferredSize() const
	{
		return m_pHolder->_sideCanvasPreferredSize(this);
	}
	
	//____ _minSize() _________________________________________________________________
	
	SizeI SideCanvas::_minSize() const
	{
		return m_pHolder->_sideCanvasMinSize(this);
	}
	
	//____ _maxSize() _________________________________________________________________
	
	SizeI SideCanvas::_maxSize() const
	{
		return m_pHolder->_sideCanvasMaxSize(this);
	}

	//____ _collectPatches() _________________________________________________________________

	void SideCanvas::_collectPatches( Patches& container, const RectI& geo, const RectI& clip )
	{
		m_pHolder->_sideCanvasCollectPatches(this,container, geo, clip);
	}

	//____ _maskPatches() _________________________________________________________________

	void SideCanvas::_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
	{
		m_pHolder->_sideCanvasMaskPatches(this,patches,geo,clip,blendMode);
	}

	//____ _render() _________________________________________________________________

	void SideCanvas::_render( GfxDevice * pDevice, const RectI& canvas, const RectI& window )
	{
		m_pHolder->_sideCanvasRender(this,pDevice,canvas,window);
	}

	//____ _refresh() _________________________________________________________________

	void SideCanvas::_refresh()
	{
		m_pHolder->_sideCanvasRefresh(this);
	}
	
	//____ _resize() _________________________________________________________________
	
	void SideCanvas::_resize( const SizeI& size )
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
	
	bool SideCanvas::_alphaTest( const CoordI& ofs )
	{
		return m_pHolder->_sideCanvasAlphaTest(this,ofs);		
	}
	
	
	
	int SideCanvas::Holder::_sideCanvasMatchingHeight( const SideCanvas * pCanvas, int width ) const
	{
		return _sideCanvasPreferredSize(pCanvas).h;
	}
	
	int SideCanvas::Holder::_sideCanvasMatchingWidth( const SideCanvas * pCanvas, int height ) const
	{
		return _sideCanvasPreferredSize(pCanvas).w;		
	}

	SizeI SideCanvas::Holder::_sideCanvasMinSize( const SideCanvas * pCanvas ) const
	{
		return pCanvas->Widget::_minSize();
	}
	
	SizeI SideCanvas::Holder::_sideCanvasMaxSize( const SideCanvas * pCanvas ) const
	{
		return pCanvas->Widget::_maxSize();
	}

	void SideCanvas::Holder::_sideCanvasCollectPatches( SideCanvas * pCanvas, Patches& container, const RectI& geo, const RectI& clip )
	{
		return pCanvas->Widget::_collectPatches(container, geo, clip);
	}

	void SideCanvas::Holder::_sideCanvasMaskPatches( SideCanvas * pCanvas, Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
	{
		return pCanvas->Widget::_maskPatches(patches,geo,clip,blendMode);
	}

	void SideCanvas::Holder::_sideCanvasRefresh( SideCanvas * pCanvas)
	{
		return pCanvas->Widget::_refresh();
	}

	void SideCanvas::Holder::_sideCanvasSetSkin( SideCanvas * pCanvas,  Skin * pSkin ) 
	{
		return pCanvas->Widget::_setSkin(pSkin);
	}

	void SideCanvas::Holder::_sideCanvasSetState( SideCanvas * pCanvas,  State state )
	{
		return pCanvas->Widget::_setState(state);
	}

	void SideCanvas::Holder::_sideCanvasReceive( SideCanvas * pCanvas,  Msg * pMsg )
	{
		return pCanvas->Widget::_receive(pMsg);
	}

	
}