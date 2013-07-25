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
#ifndef WG_FILLER_DOT_H
#define WG_FILLER_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

class WgFiller;
typedef	WgSmartPtr<WgFiller,WgWidgetPtr>		WgFillerPtr;
typedef	WgWeakPtr<WgFiller,WgWidgetWeakPtr>	WgFillerWeakPtr;

//____ WgFiller ____________________________________________________________

class WgFiller : public WgWidget
{
public:
	static WgFillerPtr	Create() { return WgFillerPtr(new WgFiller()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgFillerPtr	Cast( const WgObjectPtr& pObject );

	void	SetPreferredSize( const WgSize& size );
	WgSize	PreferredSize() const;

protected:
	WgFiller();
	virtual ~WgFiller();
	virtual WgWidget* _newOfMyType() const { return new WgFiller(); };

	void	_onCloneContent( const WgWidget * _pOrg );

private:
	WgSize			m_preferredSize;
};


#endif //WG_FILLER_DOT_H
