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
#ifndef WG_BOXSKIN_DOT_H
#define WG_BOXSKIN_DOT_H

#ifndef WG_EXTENDEDSKIN_DOT_H
#	include <wg_extendedskin.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

class WgBoxSkin;
typedef	WgSmartChildPtr<WgBoxSkin,WgExtendedSkinPtr>	WgBoxSkinPtr;

class WgBoxSkin : public WgExtendedSkin
{
public:
	static WgBoxSkinPtr	Create();
	static WgBoxSkinPtr Create( WgColor color, WgBorders frame = 0, WgColor frameColor = WgColor::White);
	
	~WgBoxSkin() {};


    void    SetColor( WgColor color );
    void    SetFrameColor( WgColor color );
    void    SetFrameThickness( WgBorders frame );
    void    SetFrame( WgBorders frame, WgColor color );

	void	SetStateColor( WgStateEnum state, WgColor color );
	void	SetStateColor( WgStateEnum state, WgColor color, WgColor frameColor );

    void Render( wg::GfxDevice * pDevice, WgState mode, const WgRect& _canvas, int scale ) const;
	
	WgSize	MinSize(int scale) const;
	WgSize	PreferredSize(int scale) const;

	WgSize	SizeForContent( const WgSize contentSize, int scale ) const;
	bool	MarkTest( const WgCoord& ofs, const WgSize& canvasSize, WgState state, int opacityTreshold, int scale ) const;

	bool	IsOpaque() const;
	bool	IsOpaque( WgState state ) const;
	bool	IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state, int scale ) const;

	bool	IsStateIdentical(WgState state, WgState comparedTo) const;


private:
	WgBoxSkin();
	WgBoxSkin( WgColor color, WgBorders frame, WgColor frameColor );
	void	_updateOpaqueFlag();
	inline WgSize _scaledFrame(int scale) const { return WgSize((m_frame.width()*scale) >> WG_SCALE_BINALS, (m_frame.height()*scale) >> WG_SCALE_BINALS); }


	bool		m_bOpaque;
	WgColor		m_color[WG_NB_STATES];
	WgColor		m_frameColor[WG_NB_STATES];
    WgBorders   m_frame;
};

#endif //WG_BOXSKIN_DOT_H


