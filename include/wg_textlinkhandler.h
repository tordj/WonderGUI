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

#ifndef WG_TEXTLINKHANDLER_DOT_H
#define WG_TEXTLINKHANDLER_DOT_H

#include <string>


#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

class WgText;
class WgTextLink;
class WgTextLinkHandler;
class WgCoord;

typedef	WgSmartPtr<class WgTextLink> WgTextLinkPtr;


//____ WgTextLink _____________________________________________________________


class WgTextLink : public WgRefCounted
{
public:
	static WgTextLinkPtr Create( const std::string& link, WgTextLinkHandler * pHandler ) { return new WgTextLink(link,pHandler); }

	WgTextLinkHandler *		Handler() const { return m_pHandler; }
	const std::string&		Link() const { return m_link; }
	bool					HasBeenAccessed() const { return m_bAccessed; }

private:
	bool					m_bAccessed;
	WgTextLinkHandler *		m_pHandler;
	std::string				m_link;

	WgTextLink( const std::string& link, WgTextLinkHandler * pHandler );
	~WgTextLink() {}
};


//____ WgTextLinkHandler ______________________________________________________

class WgTextLinkHandler
{
	friend class WgText;

public:
	WgTextLinkHandler() {}
	virtual ~WgTextLinkHandler() {}

	virtual void	OnPointerEnter( const WgTextLinkPtr& pLink, const WgCoord& screenPos );
	virtual void	OnPointerOver( const WgTextLinkPtr& pLink, const WgCoord& screenPos );
	virtual void	OnPointerExit( const WgTextLinkPtr& pLink, const WgCoord& screenPos );

	virtual void	OnButtonPress( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos );
	virtual void	OnButtonRelease( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos );
	virtual void	OnButtonRepeat( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos );
	virtual void	OnButtonClick( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos );
	virtual void	OnButtonDoubleClick( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos );
};

#endif //WG_TEXTLINKHANDLER_DOT_H
