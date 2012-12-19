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
#ifndef WG_FILL_DOT_H
#define WG_FILL_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_COLORSET_DOT_H
#	include <wg_colorset.h>
#endif

//____ WgFiller ____________________________________________________________

class WgFiller : public WgWidget
{
public:
	WgFiller();
	virtual ~WgFiller();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgFiller(); };

	void	SetColors( const WgColorsetPtr& pColors );
	const	WgColorsetPtr	GetColors() const		{ return m_pColors; }

	void	SetDefaultSize( const WgSize& size );
	WgSize	DefaultSize() const;

protected:

	bool	_isColorChanged( WgMode newMode ) const;
	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	bool	_onAlphaTest( const WgCoord& ofs );
	void	_onEnable();
	void	_onDisable();

private:

	WgColorsetPtr	m_pColors;
	WgSize			m_defaultSize;
	WgMode			m_mode;

};


#endif //WG_FILL_DOT_H
