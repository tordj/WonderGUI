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
typedef	WgSmartPtr<WgExtendedSkin,WgSkinPtr>	WgExtendedSkinPtr;

class WgExtendedSkin : public WgSkin
{
public:
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgExtendedSkinPtr	Cast( const WgObjectPtr& pObject );

	virtual void	SetContentPadding( WgBorders padding );
	virtual void	SetContentShift( WgStateEnum state, WgCoord shift );

	virtual void	SetHoveredContentShift( WgCoord shift );
	virtual void	SetPressedContentShift( WgCoord shift );
	virtual void	SetSelectedContentShift( WgCoord shift );
	virtual void	SetFocusedContentShift( WgCoord shift );

	virtual WgSize	MinSize() const;
	virtual WgSize	PreferredSize() const;

	virtual WgSize	ContentPadding() const;
	virtual WgSize	SizeForContent( const WgSize contentSize ) const;
	virtual WgRect	ContentRect( const WgRect& canvas, WgState state ) const;

	virtual	bool	IsStateIdentical( WgState state, WgState comparedTo ) const;


protected:
	WgBorders	m_contentPadding;
	WgCoord		m_contentShift[WG_NB_STATES];	
};


#endif //WG_EXTENDEDSKIN_DOT_H