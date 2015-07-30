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

class WgExtendedSkin;
typedef	WgStrongPtr<WgExtendedSkin,WgSkinPtr>	WgExtendedSkinPtr;

class WgExtendedSkin : public WgSkin
{
public:
	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgExtendedSkinPtr	cast( const WgObjectPtr& pObject );

	virtual void	setContentPadding( WgBorder padding );
	virtual void	setContentShift( WgStateEnum state, WgCoord shift );

	virtual void	setHoveredContentShift( WgCoord shift );
	virtual void	setPressedContentShift( WgCoord shift );
	virtual void	setSelectedContentShift( WgCoord shift );
	virtual void	setFocusedContentShift( WgCoord shift );

	virtual WgSize	minSize() const;
	virtual WgSize	preferredSize() const;

	virtual WgSize	contentPadding() const;
	virtual WgSize	sizeForContent( const WgSize contentSize ) const;
	virtual WgRect	contentRect( const WgRect& canvas, WgState state ) const;

	virtual	bool	isStateIdentical( WgState state, WgState comparedTo ) const;


protected:
	WgBorder	m_contentPadding;
	WgCoord		m_contentShift[WG_NB_STATES];	
};


#endif //WG_EXTENDEDSKIN_DOT_H
