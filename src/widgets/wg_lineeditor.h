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

#ifndef	WG_LINEEDITOR_DOT_H
#define	WG_LINEEDITOR_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_itexteditor.h>

namespace wg
{

	class LineEditor;
	typedef	StrongPtr<LineEditor>	LineEditor_p;
	typedef	WeakPtr<LineEditor>		LineEditor_wp;

	class LineEditor:public Widget
	{
	public:
		//.____ Creation __________________________________________

		static LineEditor_p	create() { return LineEditor_p(new LineEditor()); }

		//.____ Interfaces _______________________________________

		ITextEditor		text;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static LineEditor_p	cast( Object * pObject );


	protected:
		LineEditor();
		virtual ~LineEditor();
		virtual Widget* _newOfMyType() const override { return new LineEditor(); }

		SizeI			_preferredSize() const override;

		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void			_setSize( const SizeI& size ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;
		void			_setSkin( Skin * pSkin ) override;

		CoordI			_componentPos( const Component * pComponent ) const override;
		SizeI			_componentSize( const Component * pComponent ) const override;
		RectI			_componentGeo( const Component * pComponent ) const override;

		void			_componentRequestRender(const Component * pComponent) override;
		void			_componentRequestRender( const Component * pComponent, const RectI& rect ) override;
		void			_componentRequestResize( const Component * pComponent ) override;
		void			_componentRequestInView(const Component * pComponent, const RectI& preferred, const RectI& prio) override;

	private:

		CTextEditor	m_text;
		int				m_textScrollOfs;
	};






} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
