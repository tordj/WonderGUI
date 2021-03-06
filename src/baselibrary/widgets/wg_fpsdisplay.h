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

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size		preferredSize() const override;


	protected:
		FpsDisplay();
		virtual ~FpsDisplay();
		virtual Widget* _newOfMyType() const override { return new FpsDisplay(); };

		void		_setState( State state ) override;
		void		_update(int microPassed, int64_t microsecTimestamp) override;
		void		_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;
		void		_cloneContent( const Widget * _pOrg ) override;

	private:

	const static int c_tickBuffer = 64;


		int *			m_pTickBuffer;
		int				m_tickBufferOfs;
	};




} // namespace wg
#endif //FpsDisplay_DOT_H
