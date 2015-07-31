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
#ifndef WG_EXTENDEDSKIN_DOT_H
#define WG_EXTENDEDSKIN_DOT_H

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

namespace wg 
{
	
	class ExtendedSkin;
	typedef	WgStrongPtr<ExtendedSkin,Skin_p>	ExtendedSkin_p;
	
	class ExtendedSkin : public Skin
	{
	public:
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static ExtendedSkin_p	cast( const Object_p& pObject );
	
		virtual void	setContentPadding( Border padding );
		virtual void	setContentShift( StateEnum state, Coord shift );
	
		virtual void	setHoveredContentShift( Coord shift );
		virtual void	setPressedContentShift( Coord shift );
		virtual void	setSelectedContentShift( Coord shift );
		virtual void	setFocusedContentShift( Coord shift );
	
		virtual Size	minSize() const;
		virtual Size	preferredSize() const;
	
		virtual Size	contentPadding() const;
		virtual Size	sizeForContent( const Size contentSize ) const;
		virtual Rect	contentRect( const Rect& canvas, State state ) const;
	
		virtual	bool	isStateIdentical( State state, State comparedTo ) const;
	
	
	protected:
		Border	m_contentPadding;
		Coord		m_contentShift[WG_NB_STATES];	
	};
	
	

} // namespace wg
#endif //WG_EXTENDEDSKIN_DOT_H
