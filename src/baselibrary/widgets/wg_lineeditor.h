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

		CTextEditor		editor;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

		SizeSPX			_preferredSize(int scale = -1) const override;

	protected:
		LineEditor();
		virtual ~LineEditor();
		virtual Widget* _newOfMyType() const override { return new LineEditor(); }


		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void			_resize( const SizeSPX& size, int scale = -1 ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;

		CoordSPX		_componentPos( const GeoComponent * pComponent ) const override;
		SizeSPX			_componentSize( const GeoComponent * pComponent ) const override;
		RectSPX			_componentGeo( const GeoComponent * pComponent ) const override;

		void			_componentRequestRender(const GeoComponent * pComponent) override;
		void			_componentRequestRender( const GeoComponent * pComponent, const RectSPX& rect ) override;
		void			_componentRequestResize( const GeoComponent * pComponent ) override;
		void			_componentRequestInView(const GeoComponent * pComponent, const RectSPX& mustHave, const RectSPX& niceToHave) override;

	private:

		spx				m_textScrollOfs;
	};






} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
