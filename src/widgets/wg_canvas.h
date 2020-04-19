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
#ifndef WG_CANVAS_DOT_H
#define WG_CANVAS_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_gfxdevice.h>
#include <wg_ccanvas.h>


namespace wg
{

	class Canvas;
	typedef	StrongPtr<Canvas>	Canvas_p;
	typedef	WeakPtr<Canvas>		Canvas_wp;





	//____ Canvas ____________________________________________________________
	/**
	 * @brief Widget displaying a surface on which rendering operations can be performed.
	 *
	 * The Canvas widget displays a surface onto which various rendering operations can be
	 * performed using a GfxDevice.
	 *
	 * To setup a Canvas, you at least need to provide a GfxDevice of your choice, that
	 * should be unique to this Canvas and not used for anything else. Optionally you
	 * can also provide a SurfaceFactory and specify PixelFormat and size for the surface. Please see
	 * the CCanvas interface documentation for details.
	 *
	 * Please note that "canvas" is not only the name of this widget, but used throughout
	 * WonderGUI for surfaces and rectangular areas that are destinations for drawing operations.
	 * To make a clearer distinction we try to write Canvas with a capital C when
	 * referring to the widget and a small c everywhere else, but through
	 * CamelCasing we still end up writing canvas with a capital C when not referring to this widget
	 * (like "setCanvas()" or "pCanvas").
	 *
	 **/


	class Canvas : public Widget
	{
		friend class ICanvas;

	public:

		//.____ Creation ______________________________________________________

		static Canvas_p	create() { return Canvas_p(new Canvas()); }

		//.____ Components ____________________________________________________

		CCanvas		canvas;


		//.____ Identification ________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		MU				matchingHeight(MU width) const override;
		MU				matchingWidth(MU height) const override;

		Size			preferredSize() const override;

		//.____ Appearance ____________________________________________________

		void			setSkin(Skin * pSkin) override;

	protected:
		Canvas();
		virtual ~Canvas();
		virtual Widget* _newOfMyType() const override { return new Canvas(); };


		void			_cloneContent( const Widget * _pOrg ) override;
		void			_resize(const Size& size) override;
		void			_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		bool			_alphaTest(const Coord& ofs) override;

	};



} // namespace wg
#endif //WG_CANVAS_DOT_H
