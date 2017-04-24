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
#pragma once

#include <wg_skin.h>

namespace wg 
{
	
	class ExtendedSkin;
	typedef	StrongPtr<ExtendedSkin,Skin_p>	ExtendedSkin_p;
	
	class ExtendedSkin : public Skin
	{
	public:
		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static ExtendedSkin_p	cast( const Object_p& pObject );
	
		//.____ Geometry _________________________________________________
		
		virtual Size	minSize() const;
		virtual Size	preferredSize() const;
		virtual Size	sizeForContent( const Size contentSize ) const;

		virtual void	setContentPadding( Border padding );
		virtual Size	contentPadding() const;
		virtual Coord	contentOfs( State state ) const;
		virtual Rect	contentRect( const Rect& canvas, State state ) const;

		//.____ Behavior _______________________________________________________

		virtual void	setContentShift( StateEnum state, Coord shift );
	
		virtual void	setHoveredContentShift( Coord shift );
		virtual void	setPressedContentShift( Coord shift );
		virtual void	setSelectedContentShift( Coord shift );
		virtual void	setFocusedContentShift( Coord shift );

		//.____ Misc ____________________________________________________
	
		virtual	bool	isStateIdentical( State state, State comparedTo ) const;
	
	
	protected:
		Border	m_contentPadding;
		Coord		m_contentShift[StateEnum_Nb];	
	};
	
	

} // namespace wg
#endif //WG_EXTENDEDSKIN_DOT_H
