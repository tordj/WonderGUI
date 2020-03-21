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
#include <wg_ctexteditor.h>

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

		//.____ Components _______________________________________

		CTextEditor		text;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


	protected:
		LineEditor();
		virtual ~LineEditor();
		virtual Widget* _newOfMyType() const override { return new LineEditor(); }

		SizeI			_preferredSize() const override;

		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void			_resize( const SizeI& size ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;
		void			_setSkin( Skin * pSkin ) override;

		CoordI			_componentPos( const GeoComponent * pComponent ) const override;
		SizeI			_componentSize( const GeoComponent * pComponent ) const override;
		RectI			_componentGeo( const GeoComponent * pComponent ) const override;

		void			_componentRequestRender(const GeoComponent * pComponent) override;
		void			_componentRequestRender( const GeoComponent * pComponent, const RectI& rect ) override;
		void			_componentRequestResize( const GeoComponent * pComponent ) override;
		void			_componentRequestInView(const GeoComponent * pComponent, const RectI& mustHave, const RectI& niceToHave) override;

		class TextAccess : public CTextEditor { friend class LineEditor; };
		const TextAccess& _text() const { return static_cast<const TextAccess&>(text); }
		TextAccess& _text() { return static_cast<TextAccess&>(text); }


	private:

		int				m_textScrollOfs;
	};






} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
