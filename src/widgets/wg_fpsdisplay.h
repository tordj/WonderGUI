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

#ifndef	WG_FPSDISPLAY_DOT_H
#define WG_FPSDISPLAY_DOT_H
#pragma once

#include <wg_widget.h>
#include	<wg_ctextdisplay.h>

namespace wg
{


	class Font;

	class FpsDisplay;
	typedef	StrongPtr<FpsDisplay>		FpsDisplay_p;
	typedef	WeakPtr<FpsDisplay>	FpsDisplay_wp;

	class FpsDisplay : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static FpsDisplay_p	create() { return FpsDisplay_p(new FpsDisplay()); }


		//.____ Components _______________________________________

		CTextDisplay			labels;
		CStaticTextDisplay		values;


		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static FpsDisplay_p	cast( Object * pObject );


	protected:
		FpsDisplay();
		virtual ~FpsDisplay();
		virtual Widget* _newOfMyType() const override { return new FpsDisplay(); };

		SizeI		_preferredSize() const override;

		void		_receive( Msg * pMsg ) override;
		void		_setState( State state ) override;
		void		_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void		_cloneContent( const Widget * _pOrg ) override;

		class TextAccess : public CTextDisplay { friend class FpsDisplay; };
		const TextAccess& _labels() const { return static_cast<const TextAccess&>(labels); }
		TextAccess& _labels() { return static_cast<TextAccess&>(labels); }

		class StaticTextAccess : public CStaticTextDisplay { friend class FpsDisplay; };
		const StaticTextAccess& _values() const { return static_cast<const StaticTextAccess&>(values); }
		StaticTextAccess& _values() { return static_cast<StaticTextAccess&>(values); }


	private:

	const static int c_tickBuffer = 64;


		int *			m_pTickBuffer;
		int				m_tickBufferOfs;
		RouteId			m_tickRouteId;
	};




} // namespace wg
#endif //FpsDisplay_DOT_H
