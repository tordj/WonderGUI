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

#ifndef WDG_ROOT_DOT_H
#define WDG_ROOT_DOT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef	WG_WIDGET_DOT_H
#	include  <wg_widget.h>
#endif

class WgInput;

class Wdg_Root : public WgWidget
{

public:

  WIDGET_CONSTRUCTORS(Wdg_Root,WgWidget)
  ~Wdg_Root();

  // Wdg_Root specific methods

	void	Update( Uint32 _msTime );

	int		Render( WgRect * _paDirtyRects, int nRects, int _maxRects, WgRect * _pClip = 0);

	int		BeginRender( WgRect * _paDirtyRects, int nRects, int _maxRects, WgRect * _pClip = 0);
	void	RenderLayer( Uint8 _layer );
	void	EndRender( void );

	inline WgInput * InputObj( void ) { return &m_input; };

  // Methods deactivated on Wdg_Roots

  bool	SetParent( WgWidget * _pParent, WgWidget * pBelow = 0 ) { return false; }
	bool	SetOrigo( int _origo ) { return false; }

  bool	Modal( bool _bOnOff ) { return false; }

  bool	MinSize( int _w, int _h ) { return false; }
	bool	MaxSize( int _w, int _h ) { return false; }

  bool	Top( void ) { return false; }
	bool	Bottom( void ) { return false; }
	bool	Up( void ) { return false; }													
	bool	Down( void ) { return false; }


  // Methods modified on Wdg_Roots

	virtual const char* Type(void) const;
	static const char* GetMyType( void );
	bool SetSize( int _w, int _h, bool bAllowMoveChildren = false );         // Sets MinSize() and MaxSize() as well.
	bool	IsRootWidget() const { return true; }


protected:
	virtual WgWidget * NewOfMyType() const;

	bool	DoMyOwnMarkTest( int _x, int _y );

private:
	void	Init();
	int		ExportDirtyRects( WgDirtyRectObj * _pDirtObj, WgRect * _wpaDirtyRects, int _maxRects );



	WgInput		m_input;
	WgUpdateInfo	m_info;
};




#endif // WDG_ROOT_DOT_H
