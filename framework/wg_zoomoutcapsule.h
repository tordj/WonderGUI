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
#ifndef WG_GIZMO_MODZOOM_DOT_H
#define WG_GIZMO_MODZOOM_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_capsule.h>
#endif

class WgSurfaceFactory;

//____ WgZoomOutCapsule __________________________________________________________

class WgZoomOutCapsule : public WgCapsule
{
public:
	WgZoomOutCapsule();
	virtual ~WgZoomOutCapsule();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgZoomOutCapsule(); };
	
	void			SetSkin(const WgSkinPtr& pSkin);
    void            SetOutlineSkin(const WgSkinPtr& pSkin);
    
    void            SetButtonSkin(const WgSkinPtr& pSkin);
    WgSkinPtr       ButtonSkin() const { return m_pButtonSkin; };
    
//	int				MatchingPixelHeight(int pixelWidth) const;
//	int				MatchingPixelWidth(int pixelHeight) const;

    WgSize          MinPixelSize() const;
    void            SetMinPointSize( WgSize min );
    
    
    WgSize          PreferredPixelSize() const;

    void            SetInnerTransition(float scale);                                            // Zoom out level of the contained widget.
    float           InnerTransition() const { return m_innerTransitionFactor; }
    
    void            SetInOutTransition(float factor );                                        // 0.0 -> 1.0. From non-existent (size 0,0) to full size and fully faded in.
    float           InOutTransition() const { return m_transitionFactor; }

    void            SetOutlineMode(bool bOutline);
    bool            IsOutlineMode() const { return m_bOutlineMode; }
    
    void            SetTint( WgColor tint );
    WgColor         Tint() const { return m_tint; }
    
    WgRect          ContentAreaPixels() const;
    WgRect          ContentAreaPoints() const;

    WgWidget *      FindWidget( const WgCoord& ofs, WgSearchMode mode );

    
protected:
    
    void    _renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );

	void	_onCloneContent( const WgWidget * _pOrg );
    void	_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window );
	bool	_onAlphaTest( const WgCoord& ofs );
    void    _onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
    
    void    _setScale( int scale );

    WgRect  _screenshotArea( WgRect canvas ) const;
    WgRect  _buttonArea( WgRect canvas ) const;
    void    _regenScreenshot();

    void    _updateButtonState( WgSize gizmoCanvas, WgCoord pointerPos );
    
private:
    
	WgSurface * m_pScreenshot = nullptr;
    WgSkinPtr   m_pButtonSkin;
    WgSkinPtr   m_pOutlineSkin;
    
    float       m_innerTransitionFactor = 1.f;
    float       m_minInnerZoom = 0.5f;
    float       m_innerZoom = 1.f;

    float       m_transitionFactor = 1.f;
    float       m_outerZoom = 1.f;
    WgColor     m_tint = WgColor::White;

    WgState     m_buttonState = WgStateEnum::Normal;
    bool        m_bButtonPressed = false;
    
    bool        m_bOutlineMode = false;
    
    WgSize      m_minPointSize;
};


#endif //WG_GIZMO_MODZOOM_DOT_H
