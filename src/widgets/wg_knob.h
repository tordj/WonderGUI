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
#ifndef WG_KNOB_DOT_H
#define WG_KNOB_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

class WgKnob;
typedef	WgStrongPtr<WgKnob,WgWidgetPtr>		WgKnobPtr;
typedef	WgWeakPtr<WgKnob,WgWidgetWeakPtr>	WgKnobWeakPtr;

//____ WgKnob ____________________________________________________________

class WgKnob : public WgWidget
{
public:
	static WgKnobPtr	create() { return WgKnobPtr(new WgKnob()); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgKnobPtr	cast( const WgObjectPtr& pObject );

    void    setValue( float value );
    
	WgSize	preferredSize() const;

protected:
	WgKnob();
	virtual ~WgKnob();
	virtual WgWidget* _newOfMyType() const { return new WgKnob(); };

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	bool	_onAlphaTest( const WgCoord& ofs, const WgSize& sz );

private:

};


#endif //WG_KNOB_DOT_H
