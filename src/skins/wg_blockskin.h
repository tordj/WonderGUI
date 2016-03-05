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

namespace wg 
{
	
	
	class BlockSkin;
	typedef	StrongPtr<BlockSkin,ExtendedSkin_p>	BlockSkin_p;
	
	
	class BlockSkin : public ExtendedSkin
	{
		//TODO: Add sanity-checking to all Set-methods.
		//TODO: Optimize rendering based on invisibleSections and opaqueSections!
	
	public:
		static BlockSkin_p create();
		static BlockSkin_p createStatic( const Surface_p& pSurface, Rect block, Border frame = Border(0) );
		static BlockSkin_p createEnable( const Surface_p& pSurface, Size blockSize, Coord ofsEnabled, Coord ofsDisabled, Border frame = Border(0) );
		static BlockSkin_p createClickable( const Surface_p& pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame = Border(0) );
		static BlockSkin_p createSelectable( const Surface_p& pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame = Border(0) );
		static BlockSkin_p createClickSelectable(const Surface_p& pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame = Border(0) );
		static BlockSkin_p createClickSelectableWidget( const Surface_p& pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame = Border(0) );
	
		static BlockSkin_p createStaticFromSurface( const Surface_p& pSurface, Border frame = Border(0) );
		static BlockSkin_p createEnableFromSurface( const Surface_p& pSurface, int blockSpacing, Border blockFrame = Border(0) );
		static BlockSkin_p createClickableFromSurface( const Surface_p& pSurface, int blockSpacing, Border blockFrame = Border(0) );
		static BlockSkin_p createSelectableFromSurface( const Surface_p& pSurface, int blockSpacing, Border blockFrame = Border(0) );
		static BlockSkin_p createClickSelectableFromSurface( const Surface_p& pSurface, int blockSpacing, Border blockFrame = Border(0) );
	
		~BlockSkin() {};
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static BlockSkin_p	cast( const Object_p& pObject );
	
	
		void	setSurface( const Surface_p& pSurf );
		bool	setBlockGeo( Size size, Border frame = Border(0) );
		void	setStateBlock( StateEnum state, const Coord& ofs );
	
		void	setAllBlocks( const Coord& ofs );
		void	setDisabledBlock( const Coord& ofs );
		void	setHoveredBlocks( const Coord& ofs );
		void	setPressedBlocks( const Coord& ofs );
		void	setSelectedBlocks( const Coord& ofs );
	
		void	setTiled( bool bTiled );
		void	setTiledTopBorder( bool bTiled );
		void	setTiledBottomBorder( bool bTiled );
		void	setTiledLeftBorder( bool bTiled );
		void	setTiledRightBorder( bool bTiled );
		void	setTiledCenter( bool bTiled );
	
		void	optimizeRenderMethods();
	
		void	render( GfxDevice * pDevice, const Rect& _canvas, State state, const Rect& _clip ) const;
	
		Size	minSize() const;
		Size	preferredSize() const;
	
		Size	sizeForContent( const Size contentSize ) const;
	
		bool	markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold ) const;
	
		bool	isOpaque() const;
		bool	isOpaque( State state ) const;
		bool	isOpaque( const Rect& rect, const Size& canvasSize, State state ) const;
	
		bool	isStateIdentical( State state, State comparedTo ) const;
	
	private:
		struct StateData
		{
			Coord	ofs;
			int		opaqueSections;
			int		invisibleSections;
		};
	
		BlockSkin();
		void	_setBitFlag( int& bitmask, int bit, bool bSet );
		void	_renderNoClip( GfxDevice * pDevice, const StateData * pState, const Rect& _canvas ) const;
		void	_scanStateBlockSectionArea( StateData * pState, Origo section, const Rect& sectionArea );
	
		static const int ALL_SECTIONS = 0x1FF;
	
		Surface_p	m_pSurface;
		Size			m_dimensions;
		Border		m_frame;
		int				m_tiledSections;
		bool			m_bIsOpaque;
	
		StateData		m_state[StateEnum_Nb];
	};
	

} // namespace wg
#endif //WG_BLOCKSKIN_DOT_H
