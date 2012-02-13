/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@swipnet.se].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------
	
  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@swipnet.se] for details.

=========================================================================*/

#ifndef	WDG_SHADER_DOT_H
#define	WDG_SHADER_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif


class Wdg_Shader:public WgWidget, public WgEmitter
{
	public:

		WIDGET_CONSTRUCTORS(Wdg_Shader,WgWidget)
		virtual ~Wdg_Shader();

		virtual const char * Type() const;
		static const char * GetMyType();

		void		SetColor( const WgColor& color);
		void		SetTintMode( WgTintMode mode );
		void		SetBlendMode( WgBlendMode mode );

		inline WgColor			Color() { return m_tintColor; }
		inline WgBlendMode	BlendMode() { return m_blendMode; }
		inline WgTintMode	TintMode() { return m_tintMode; }

	protected:
		WgWidget * NewWidgetOfMyType() const;

		virtual void	RenderRecursively(Uint8 _layer, const WgRect& clip);
		WgEmitter * GetEmitter() { return this; }

	private:
		void	Init();
		void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
		bool	DoMyOwnMarkTest( int _x, int _y );

		WgColor				m_tintColor;
		WgBlendMode			m_blendMode;
		WgTintMode			m_tintMode;
};

#endif //	WDG_SHADER_DOT_H
