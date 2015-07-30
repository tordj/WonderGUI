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
#ifndef WG_BLOCKSKIN_DOT_H
#define WG_BLOCKSKIN_DOT_H

#ifndef WG_EXTENDEDSKIN_DOT_H
#	include <wg_extendedskin.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif


class WgBlockSkin;
typedef	WgStrongPtr<WgBlockSkin,WgExtendedSkinPtr>	WgBlockSkinPtr;


class WgBlockSkin : public WgExtendedSkin
{
	//TODO: Add sanity-checking to all Set-methods.
	//TODO: Optimize rendering based on invisibleSections and opaqueSections!

public:
	static WgBlockSkinPtr create();
	static WgBlockSkinPtr createStatic( const WgSurfacePtr& pSurface, WgRect block, WgBorder frame = WgBorder(0) );
	static WgBlockSkinPtr createEnable( const WgSurfacePtr& pSurface, WgSize blockSize, WgCoord ofsEnabled, WgCoord ofsDisabled, WgBorder frame = WgBorder(0) );
	static WgBlockSkinPtr createClickable( const WgSurfacePtr& pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorder blockFrame = WgBorder(0) );
	static WgBlockSkinPtr createSelectable( const WgSurfacePtr& pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorder blockFrame = WgBorder(0) );
	static WgBlockSkinPtr createClickSelectable(const WgSurfacePtr& pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorder blockFrame = WgBorder(0) );
	static WgBlockSkinPtr createClickSelectableWidget( const WgSurfacePtr& pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorder blockFrame = WgBorder(0) );

	static WgBlockSkinPtr createStaticFromSurface( const WgSurfacePtr& pSurface, WgBorder frame = WgBorder(0) );
	static WgBlockSkinPtr createEnableFromSurface( const WgSurfacePtr& pSurface, int blockSpacing, WgBorder blockFrame = WgBorder(0) );
	static WgBlockSkinPtr createClickableFromSurface( const WgSurfacePtr& pSurface, int blockSpacing, WgBorder blockFrame = WgBorder(0) );
	static WgBlockSkinPtr createSelectableFromSurface( const WgSurfacePtr& pSurface, int blockSpacing, WgBorder blockFrame = WgBorder(0) );
	static WgBlockSkinPtr createClickSelectableFromSurface( const WgSurfacePtr& pSurface, int blockSpacing, WgBorder blockFrame = WgBorder(0) );

	~WgBlockSkin() {};

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgBlockSkinPtr	cast( const WgObjectPtr& pObject );


	void	setSurface( const WgSurfacePtr& pSurf );
	bool	setBlockGeo( WgSize size, WgBorder frame = WgBorder(0) );
	void	setStateBlock( WgStateEnum state, const WgCoord& ofs );

	void	setAllBlocks( const WgCoord& ofs );
	void	setDisabledBlock( const WgCoord& ofs );
	void	setHoveredBlocks( const WgCoord& ofs );
	void	setPressedBlocks( const WgCoord& ofs );
	void	setSelectedBlocks( const WgCoord& ofs );

	void	setTiled( bool bTiled );
	void	setTiledTopBorder( bool bTiled );
	void	setTiledBottomBorder( bool bTiled );
	void	setTiledLeftBorder( bool bTiled );
	void	setTiledRightBorder( bool bTiled );
	void	setTiledCenter( bool bTiled );

	void	optimizeRenderMethods();

	void	render( WgGfxDevice * pDevice, const WgRect& _canvas, WgState state, const WgRect& _clip ) const;

	WgSize	minSize() const;
	WgSize	preferredSize() const;

	WgSize	sizeForContent( const WgSize contentSize ) const;

	bool	markTest( const WgCoord& ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const;

	bool	isOpaque() const;
	bool	isOpaque( WgState state ) const;
	bool	isOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const;

	bool	isStateIdentical( WgState state, WgState comparedTo ) const;

private:
	struct StateData
	{
		WgCoord	ofs;
		int		opaqueSections;
		int		invisibleSections;
	};

	WgBlockSkin();
	void	_setBitFlag( int& bitmask, int bit, bool bSet );
	void	_renderNoClip( WgGfxDevice * pDevice, const StateData * pState, const WgRect& _canvas ) const;
	void	_scanStateBlockSectionArea( StateData * pState, WgOrigo section, const WgRect& sectionArea );

	static const int ALL_SECTIONS = 0x1FF;

	WgSurfacePtr	m_pSurface;
	WgSize			m_dimensions;
	WgBorder		m_frame;
	int				m_tiledSections;
	bool			m_bIsOpaque;

	StateData		m_state[WG_NB_STATES];
};

#endif //WG_BLOCKSKIN_DOT_H
