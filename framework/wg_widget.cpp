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

#include <wg_widget.h>
#include <wg_base.h>
#include <wg3_context.h>
#include <wg_types.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg3_dataset.h>
#include <wg3_base.h>
#include <wg3_surfacefactory.h>
#include <wg3_util.h>

#	include <wg_rootpanel.h>
#	include <wg_eventhandler.h>


//____ Constructor ____________________________________________________________

WgWidget::WgWidget():m_id(0), m_pHook(0), m_pointerStyle(WgPointerStyle::Default),
					m_markOpacity( 1 ), m_bEnabled(true), m_bOpaque(false),
					m_bFocused(false), m_bSelected(false), m_bTabLock(false), m_bReceiveTick(false), m_scale(WG_SCALE_BASE),
					m_bPickable(false), m_bDropTarget(false)
{

}

//____ Destructor _____________________________________________________________

WgWidget::~WgWidget()
{
	if( m_pHook )
	{
		m_pHook->Parent()->ReleaseChild(this);
	}
}


//____ GetPointerStyle() ________________________________________

WgPointerStyle WgWidget::GetPointerStyle() const
{
	return m_pointerStyle;
}

//____ SetSkin() ______________________________________________________________

void WgWidget::SetSkin(wg::Skin * pSkin)
{
	if (pSkin != m_pSkin)
	{
		m_pSkin = pSkin;
		_requestResize();
		_requestRender();
	}
}

//____ SetEnabled() _______________________________________________________________

void WgWidget::SetEnabled( bool bEnabled )
{
	if( m_bEnabled != bEnabled || IsContainer() )
	{
		m_bEnabled = bEnabled;
		m_state.setEnabled(bEnabled);
		if( bEnabled )
			_onEnable();
		else
			_onDisable();
	}
}

//____ MarkTest() _____________________________________________________________

bool WgWidget::MarkTest( const WgCoord& ofs )
{
	switch( m_markOpacity )
	{
	case 0:
		return true;
	case 256:
		return false;
	default:
		return _onAlphaTest(ofs);
	}
}

//____ CloneContent() _________________________________________________________

bool WgWidget::CloneContent( const WgWidget * _pOrg )
{
	if( _pOrg->Type() != Type() )
		return false;

	m_id			= _pOrg->m_id;
	m_pSkin         = _pOrg->m_pSkin;
	m_scale         = _pOrg->m_scale;
	m_pointerStyle 	= _pOrg->m_pointerStyle;

	m_tooltip		= _pOrg->m_tooltip;
	m_markOpacity	= _pOrg->m_markOpacity;

	m_bOpaque        = _pOrg->m_bOpaque;
	m_bTabLock        = _pOrg->m_bTabLock;

	m_bReceiveTick  = _pOrg->m_bReceiveTick;    // Set if Widget should reveive periodic Tick() events.

	m_bEnabled        = _pOrg->m_bEnabled;
	m_bSelectable     = _pOrg->m_bSelectable;

	m_state           = _pOrg->m_state;

	m_bPickable         = _pOrg->m_bPickable;
	m_pickCategory     = _pOrg->m_pickCategory;
	m_bDropTarget    = _pOrg->m_bDropTarget;



	_onCloneContent( _pOrg );
	return true;
}

//____ _onNewHook() ___________________________________________________________

void WgWidget::_onNewHook( WgHook * pHook )
{
	if( pHook )
	{
		WgRootPanel * pNewRoot = pHook->Root();
		WgRootPanel * pOldRoot = m_pHook ? m_pHook->Root() : 0;


		if( pNewRoot && pNewRoot != pOldRoot )
			_onNewRoot( pNewRoot );
	}

	m_pHook = pHook;

}

//____ _onNewRoot() ___________________________________________________________

void WgWidget::_onNewRoot( WgRootPanel * pRoot )
{
	if( m_bReceiveTick && pRoot )
		pRoot->EventHandler()->_addTickReceiver(this);
}

//____ _startReceiveTicks() ___________________________________________________

void WgWidget::_startReceiveTicks()
{
	if( !m_bReceiveTick )
	{
		m_bReceiveTick = true;

		if( m_pHook )
		{
			WgRootPanel * pRoot = m_pHook->Root();
			if( pRoot )
				pRoot->EventHandler()->_addTickReceiver(this);
		}
	}
}

//____ _stopReceiveTicks() ____________________________________________________

void WgWidget::_stopReceiveTicks()
{
	m_bReceiveTick = false;
}

//____ Local2absPixel() ____________________________________________________________

WgCoord WgWidget::Local2absPixel( const WgCoord& cord ) const
{
	WgCoord c = ScreenPixelPos();
	c.x += cord.x;
	c.y += cord.y;
	return c;
}

//____ Abs2localPixel() ____________________________________________________________

WgCoord WgWidget::Abs2localPixel( const WgCoord& cord ) const
{
	WgCoord c = ScreenPixelPos();
	return WgCoord( cord.x - c.x, cord.y - c.y );
}

//____ Local2absPoint() ____________________________________________________________

WgCoord WgWidget::Local2absPoint( const WgCoord& cord ) const
{
	return (Local2absPixel(cord*m_scale/WG_SCALE_BASE)*WG_SCALE_BASE) / m_scale;
}

//____ Abs2localPoint() ____________________________________________________________

WgCoord WgWidget::Abs2localPoint( const WgCoord& cord ) const
{
	return (Abs2localPixel(cord*m_scale/WG_SCALE_BASE)*WG_SCALE_BASE) / m_scale;
}

//____ Screenshot() ___________________________________________________________

wg::Surface_p WgWidget::Screenshot( int surfaceFlags )
{
	auto pDevice = wg::Base::activeContext()->gfxDevice();
	auto pFactory =  wg::Base::activeContext()->surfaceFactory();

	if( !pDevice || !pFactory )
		return nullptr;

	WgSize sz = PixelSize();

	auto pCanvas = pFactory->createSurface(sz,wg::PixelFormat::BGRA_8, surfaceFlags );
	pCanvas->setScale(m_scale/4096.f);

	WgPatches patches;
	patches.add( sz );


	auto pOldCanvas = pDevice->canvas();
	WgColor oldTint = pDevice->tintColor();
	WgBlendMode oldBlendMode = pDevice->blendMode();
	pDevice->setCanvas(pCanvas);
	pDevice->setTintColor( WgColor::White);

	if( pDevice->isRendering() )
	{
		pDevice->setBlendMode(wg::BlendMode::Replace);
		pDevice->fill( WgColor::Transparent );
		pDevice->setBlendMode(wg::BlendMode::Blend);
		_renderPatches(pDevice, sz, sz, &patches);
	}
	else
	{
		pDevice->beginRender();
		pDevice->setBlendMode(wg::BlendMode::Replace);
		pDevice->fill( WgColor::Transparent );
		pDevice->setBlendMode(wg::BlendMode::Blend);
		_renderPatches(pDevice, sz, sz, &patches);
		pDevice->endRender();
	}

	pDevice->setCanvas(pOldCanvas);
	pDevice->setTintColor(oldTint);
	pDevice->setBlendMode(oldBlendMode);

	return pCanvas;
}
//____ CommonAncestor() ___________________________________________________
/**
    @brief Finds closest common ancestor of widgets.

    Finds the closest common ancestor between this and pOtherWidget (if any).
    The result can be one of the specified widgets if one is an ancestor
    of the other.

    @param pOtherWidget    The widget this is checked against.

    @return The closest common ancestor, which can be one of the widgets specifed,
            or nullptr if they are not related.
*/
WgWidget * WgWidget::CommonAncestor(WgWidget* pOtherWidget)
{
    if (pOtherWidget == nullptr)
        return nullptr;

    const int c_maxLevels = 128;

    WgWidget* list1[c_maxLevels];
    WgWidget* list2[c_maxLevels];

    list1[0] = this;
    list2[0] = pOtherWidget;

    int nb1 = _listAncestors(list1+1, c_maxLevels-1) + 1;
    int nb2 = pOtherWidget->_listAncestors(list2+1, c_maxLevels-1) + 1;

    if (nb1 == c_maxLevels-1 || nb2 == c_maxLevels-1)
    {
//        wg::Base::handleError(ErrorSeverity::SilentFail, ErrorCode::Internal, "Widget hierarchy too deep for operation.", this, Widget::TYPEINFO, __func__, __FILE__, __LINE__);
        return nullptr;
    }

    int maxSearchDepth = wg::min(nb1, nb2);

    int i = 0;
    while (i < maxSearchDepth && list1[nb1-1 - i] == list2[nb2-1 - i])
        i++;

    if (i == 0 )
        return nullptr;

    return list1[nb1 - i];
}


//____ setDropTarget() ____________________________________________________

void WgWidget::setDropTarget(bool bDropTarget)
{
	m_bDropTarget = bDropTarget;
}

//____ setPickable() ____________________________________________________________

void WgWidget::setPickable( bool bPickable, int category )
{
	m_bPickable = bPickable;
	m_pickCategory = category;
}


//____ _eventHandler() __________________________________________________________

WgEventHandler * WgWidget::_eventHandler() const
{
	if( m_pHook )
	{
		WgRootPanel * pRoot = m_pHook->Root();
		if( pRoot )
			return pRoot->EventHandler();
	}
	return 0;
}

//____ MatchingPixelHeight() _______________________________________________________

int WgWidget::MatchingPixelHeight( int width ) const
{
	return PreferredPixelSize().h;		// Default is to stick with best height no matter what width.
}

//____ MatchingPixelWidth() _______________________________________________________

int WgWidget::MatchingPixelWidth( int height ) const
{
	return PreferredPixelSize().w;		// Default is to stick with best width no matter what height.
}

//____ MinPixelSize() ______________________________________________________________

WgSize WgWidget::MinPixelSize() const
{
	return WgSize(0,0);
}

//____ MaxPixelSize() ______________________________________________________________

WgSize WgWidget::MaxPixelSize() const
{
	return WgSize(2<<24,2<<24);
}

//____ SetMarked() ____________________________________________________________

bool WgWidget::SetMarked()
{
	return false;
}

//____ SetSelected() __________________________________________________________

bool WgWidget::SetSelected()
{
	if (m_bEnabled)
	{
		m_bSelected = true;

		if (!m_state.isSelected())
		{
			m_state.setSelected(true);
			_requestRender();
		}
		return true;
	}
	else
		return false;
}

//____ SetNormal() ____________________________________________________________

bool WgWidget::SetNormal()
{
	if (m_bEnabled)
	{
		m_bSelected = false;
		if (m_state.isSelected())
		{
			m_state.setSelected(false);
			_requestRender();
		}
		return true;
	}
	else
		return false;
}

//____ SetSelectable() ________________________________________________________

void WgWidget::SetSelectable(bool bSelectable)
{
	m_bSelectable = bSelectable;
}


//____ Mode() _________________________________________________________________

WgMode WgWidget::Mode() const
{
	return WgUtil::StateToMode(m_state);

//	if (m_bSelected)
//		return WG_MODE_SELECTED;
//	else if( m_bEnabled )
//		return WG_MODE_NORMAL;
//	else
//		return WG_MODE_DISABLED;
}

//____ PreferredPointSize() ___________________________________________________

WgSize WgWidget::PreferredPointSize() const
{
	WgSize sz = PreferredPixelSize()*WG_SCALE_BASE;

	sz.w += m_scale - 1;		// Preferred size should be rounded up.
	sz.h += m_scale - 1;

	return sz / m_scale;
};

//____ MinPointSize() ___________________________________________________

WgSize WgWidget::MinPointSize() const
{
	WgSize sz = MinPixelSize()*WG_SCALE_BASE;

	sz.w += m_scale - 1;		// Min size should be rounded up.
	sz.h += m_scale - 1;

	return sz / m_scale;
};


//____ MaxPointSize() ___________________________________________________

WgSize WgWidget::MaxPointSize() const
{
	WgSize sz = MaxPixelSize();
	return sz*WG_SCALE_BASE / m_scale;
};



//____ _getBlendMode() _________________________________________________________

WgBlendMode WgWidget::_getBlendMode() const
{
	WgContainer * pParent = Parent();
	if( pParent )
		return pParent->_getBlendMode();
	else
		return WgBlendMode::Blend;		// We always start out with WgBlendMode::Blend.
}

//____ _listAncestors() ________________________________________________________

int WgWidget::_listAncestors(WgWidget* array[], int max)
{
    int n = 0;
    WgWidget* p = Parent();
    while (p != nullptr && n < max)
    {
        array[n++] = p;
        p = p->Parent();
    }
    return n;
}

//____ _queueEvent() __________________________________________________________

void WgWidget::_queueEvent( WgEvent::Event * pEvent )
{
	if( m_pHook )
	{
		WgRootPanel * pRoot = m_pHook->Root();
		if( pRoot )
		{
			pRoot->EventHandler()->QueueEvent(pEvent);
			return;
		}
	}

	delete pEvent;		// Can't queue event, silently delete it.
}

//____ _requestPreRenderCall() ___________________________________________________

bool WgWidget::_requestPreRenderCall()
{
	if( m_pHook )
	{
		WgRootPanel * pRoot = m_pHook->Root();
		if( pRoot )
		{
			pRoot->_addPreRenderCall(this);
			return true;
		}
	}

	return false;
}


//____ _preRender() ____________________________________________________________

void WgWidget::_preRender()
{
	// By default we do nothing.
}

//____ _renderPatches() ________________________________________________________

void WgWidget::_renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
	pDevice->setClipList(_pPatches->size(), _pPatches->begin());
	_onRender( pDevice, _canvas, _window );
}

//____ Fillers _______________________________________________________________

void WgWidget::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
		container.add( WgRect( geo, clip ) );
}

void WgWidget::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( (m_bOpaque && blendMode == WgBlendMode::Blend) || blendMode == WgBlendMode::Replace )
	{
		patches.sub( WgRect( geo, clip ) );
	}
}

void WgWidget::_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window )
{
	if (m_pSkin)
	{
		_renderSkin( m_pSkin, pDevice, m_state, _canvas, m_scale);
	}
}

void WgWidget::_onNewSize( const WgSize& size )
{
	_requestRender();
}

void WgWidget::_setScale( int scale )
{
	m_scale = scale;
}

void WgWidget::_onRefresh()
{
	_requestRender();
}

void WgWidget::_onEvent( const WgEvent::Event * _pEvent, WgEventHandler * pHandler )
{
	WgState state = m_state;
	bool    bForward = true;

	switch( _pEvent->Type() )
	{
		case WG_EVENT_DROP_PICK:
		{
			auto pEvent = static_cast<const WgEvent::DropPick*>(_pEvent);
			if (!pEvent->hasPayload())
			{
				const_cast<WgEvent::DropPick*>(pEvent)->setPayload(wg::Dataset<WgWidgetWeakPtr>::create(this));
			}
			bForward = false;
			break;
		}
		case WG_EVENT_DROP_ENTER:
			state.setTargeted(true);
			bForward = false;
			break;
		case WG_EVENT_DROP_LEAVE:
			state.setTargeted(false);
			bForward = false;
			break;

		default:
			break;
	}

	if( state != m_state )
	{
		//TODO: We should have a virtual method for setting state, just like WG3.

		m_state = state;
		_requestRender();
	}

	if( bForward && (_pEvent->IsKeyEvent() || _pEvent->IsMouseEvent() || _pEvent->Type() == WG_EVENT_MOUSEWHEEL_ROLL) && _pEvent->Type() != WG_EVENT_MOUSE_ENTER && _pEvent->Type() != WG_EVENT_MOUSE_LEAVE )
		pHandler->ForwardEvent( _pEvent );
}

bool WgWidget::_onAlphaTest( const WgCoord& ofs )
{
	if (m_pSkin)
		return _markTestSkin( m_pSkin, ofs, PixelSize(), WgStateEnum::Normal, m_markOpacity, m_scale);
	else
		return false;
}

void WgWidget::_onEnable()
{
	_requestRender();
}

void WgWidget::_onDisable()
{
	_requestRender();
}

void WgWidget::_onGotInputFocus()
{
	m_bFocused = true;
	m_state.setFocused(true);
	_queueEvent(new WgEvent::FocusGained(this));
}

void WgWidget::_onLostInputFocus()
{
	m_bFocused = false;
	m_state.setFocused(false);
	_queueEvent(new WgEvent::FocusLost(this));
}

bool WgWidget::TempIsInputField() const
{
	return false;
}

Wg_Interface_TextHolder* WgWidget::TempGetText()
{
	return 0;
}

//____ _renderSkin() ____________________________________________________________

void WgWidget::_renderSkin( wg::Skin * pSkin, wg::GfxDevice * pDevice, wg::State state, const wg::RectI& rect, int scale, float value1, float value2 )
{
	int pixelQuarters = (scale * 4) / 4096;

	int globalPixelQuarters = wg::MU::qpixPerPoint();
	if( pixelQuarters == globalPixelQuarters )
		pSkin->render( pDevice, wg::Rect::fromPX(rect), state, value1, value2 );
	else
	{
		WgBase::_setQuartersPerPoint(pixelQuarters);
		pSkin->render( pDevice, wg::Rect::fromPX(rect), state, value1, value2 );
		WgBase::_setQuartersPerPoint(globalPixelQuarters);
	}
}

//____ _markTestSkin() ______________________________________________________________

bool WgWidget::_markTestSkin( wg::Skin * pSkin, const wg::CoordI& ofs, const wg::RectI& canvas, wg::State state, int opacityTreshold, int scale, float value1, float value2) const
{
	int pixelQuarters = (scale * 4) / 4096;

	int globalPixelQuarters = wg::MU::qpixPerPoint();
	if( pixelQuarters == globalPixelQuarters )
		return pSkin->markTest( wg::Coord::fromPX(ofs), wg::Rect::fromPX(canvas), state, opacityTreshold, value1, value2 );
	else
	{
		WgBase::_setQuartersPerPoint(pixelQuarters);
		bool ret = pSkin->markTest( wg::Coord::fromPX(ofs), wg::Rect::fromPX(canvas), state, opacityTreshold, value1, value2 );
		WgBase::_setQuartersPerPoint(globalPixelQuarters);
		return ret;
	}
}

//____ _skinMinSize() __________________________________________________________

wg::SizeI WgWidget::_skinMinSize( wg::Skin * pSkin, int scale ) const
{
	int pixelQuarters = (scale * 4) / 4096;

	int globalPixelQuarters = wg::MU::qpixPerPoint();
	if( pixelQuarters == globalPixelQuarters )
		return pSkin->minSize().px();
	else
	{
		WgBase::_setQuartersPerPoint(pixelQuarters);
		wg::SizeI ret = pSkin->minSize().px();
		WgBase::_setQuartersPerPoint(globalPixelQuarters);
		return ret;
	}

}

//____ _skinPreferredSize() __________________________________________________________

wg::SizeI WgWidget::_skinPreferredSize( wg::Skin * pSkin, int scale ) const
{
	int pixelQuarters = (scale * 4) / 4096;

	int globalPixelQuarters = wg::MU::qpixPerPoint();
	if( pixelQuarters == globalPixelQuarters )
		return pSkin->preferredSize().px();
	else
	{
		WgBase::_setQuartersPerPoint(pixelQuarters);
		wg::SizeI ret = pSkin->preferredSize().px();
		WgBase::_setQuartersPerPoint(globalPixelQuarters);
		return ret;
	}
}

//____ _skinSizeForContent() __________________________________________________________

wg::SizeI WgWidget::_skinSizeForContent(wg::Skin * pSkin, const wg::SizeI contentSize, int scale) const
{
	int pixelQuarters = (scale * 4) / 4096;

	int globalPixelQuarters = wg::MU::qpixPerPoint();
	if( pixelQuarters == globalPixelQuarters )
		return pSkin->sizeForContent(wg::Size::fromPX(contentSize)).px();
	else
	{
		WgBase::_setQuartersPerPoint(pixelQuarters);
		wg::SizeI ret = pSkin->sizeForContent(wg::Size::fromPX(contentSize)).px();
		WgBase::_setQuartersPerPoint(globalPixelQuarters);
		return ret;
	}
}

//____ _skinContentPadding() __________________________________________________________

wg::SizeI WgWidget::_skinContentPadding(wg::Skin * pSkin, int scale) const
{
	int pixelQuarters = (scale * 4) / 4096;

	int globalPixelQuarters = wg::MU::qpixPerPoint();
	if( pixelQuarters == globalPixelQuarters )
		return pSkin->contentPadding(m_state).px();
	else
	{
		WgBase::_setQuartersPerPoint(pixelQuarters);
		wg::SizeI ret = pSkin->contentPadding(m_state).px();
		WgBase::_setQuartersPerPoint(globalPixelQuarters);
		return ret;
	}
}

//____ _skinContentOfs() __________________________________________________________

wg::CoordI WgWidget::_skinContentOfs(wg::Skin * pSkin, wg::State state, int scale) const
{
	int pixelQuarters = (scale * 4) / 4096;

	int globalPixelQuarters = wg::MU::qpixPerPoint();
	if( pixelQuarters == globalPixelQuarters )
		return pSkin->contentOfs(state).px();
	else
	{
		WgBase::_setQuartersPerPoint(pixelQuarters);
		wg::CoordI ret = pSkin->contentOfs(state).px();
		WgBase::_setQuartersPerPoint(globalPixelQuarters);
		return ret;
	}
}

//____ _skinContentRect() __________________________________________________________

wg::RectI WgWidget::_skinContentRect(wg::Skin * pSkin, const wg::RectI& canvas, wg::State state, int scale) const
{
	int pixelQuarters = (scale * 4) / 4096;

	int globalPixelQuarters = wg::MU::qpixPerPoint();
	if( pixelQuarters == globalPixelQuarters )
		return pSkin->contentRect(wg::Rect::fromPX(canvas),state).px();
	else
	{
		WgBase::_setQuartersPerPoint(pixelQuarters);
		wg::RectI ret = pSkin->contentRect(wg::Rect::fromPX(canvas),state).px();
		WgBase::_setQuartersPerPoint(globalPixelQuarters);
		return ret;
	}
}

