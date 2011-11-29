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

#ifndef	WG_GIZMO_CIRCLEMETER_DOT_H
#define	WG_GIZMO_CIRCLEMETER_DOT_H

#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_INTERFACE_VALUEHOLDER_DOT_H
#	include <wg_interface_valueholder.h>
#endif

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

class	WgSurface;

class WgGizmoCirclemeter : public WgGizmo, public Wg_Interface_ValueHolder
{
	public:
		WgGizmoCirclemeter();
		virtual ~WgGizmoCirclemeter();
		virtual const char * Type() const;
		static const char * GetMyType();

		//____ Methods __________________________________________

		void SetBackgroundSource(	WgSurface * pSurf, Uint32 xNormal, Uint32 yNormal, 
									Uint32 xOver, Uint32 yOver, Uint32 xPress, Uint32 yPress, 
									Uint32 xDisable, Uint32 yDisable );

		void SetForegroundSource(	WgSurface * pSurf, WgRect& src, Uint32 destOfsX, Uint32 destOfsY );

		void SetSliceSource( WgSurface * pSurf, Uint8 blocksPerSlice, Uint8 slicePitch, bool bAscendingValue,
							 Uint32 xOfsSlice1Blocks, Uint32 yOfsSlice1Blocks, 
							 Uint32 xOfsSlice2Blocks, Uint32 yOfsSlice2Blocks, 
							 Uint32 xOfsSlice3Blocks, Uint32 yOfsSlice3Blocks, 
							 Uint32 xOfsSlice4Blocks, Uint32 yOfsSlice4Blocks );

		bool SetStartSlice( Uint8 slice );

		void SetNumberPosition( WgBorders numberBorders, WgOrigo numberAlignment );
		bool SetFonts( WgFont * _pFonts );
		void SetFormat( const WgValueFormat& format );
		const WgValueFormat& GetFormat() const  { return m_format; }

		void SetFullRangeDistance(Uint32 pixels);
		
		WgSize	DefaultSize() const;
		bool	IsAutoEllipsisDefault() const { return false; };

	protected:

		void	_onCloneContent( const WgGizmo * _pOrg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
		void	_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
		bool	_onAlphaTest( const WgCoord& ofs );
		void	_onEnable();
		void	_onDisable();
		void	_onRefresh();

	private:

		void	_valueModified();				///< Called when value has been modified.
		void	_rangeModified();				///< Called when range (and thus fractional value) has been modified.
		void	_representationModified();		///< Called when fonts or value-format has been modified.

		WgValueFormat	m_format;

		bool			m_bRegenText;
		WgText			m_text;
		int				m_renderStyle;			// 0 = up, 1 = mouse_over, 2 = down, 3 = disabled.
		Uint32			m_fullRangeDistance;
		Sint64			m_valueAtPress;

		WgBorders		m_numberBorders;

		WgSurface *		m_pSurfBg;
		WgUCord16		m_normalBg;
		WgUCord16		m_overBg;
		WgUCord16		m_pressBg;
		WgUCord16		m_disableBg;

		WgSurface *		m_pSurfFg;
		WgRect			m_srcFg;
		WgUCord16		m_destOfsFg;

		WgSurface *		m_pSurfSlices;
		Uint8			m_nBlocksPerSlice;
		WgUCord16		m_sliceOfs[4];
		Uint8			m_slicePitch;			// Offset along X-axis from beginning of one slice to beginning of next in same sequence.
		Uint8			m_startSlice;
		Uint8			m_bAscendingBlocks;
		bool			m_bPressed;
};

#endif //	WG_GIZMO_CIRCLEMETER_DOT_H
