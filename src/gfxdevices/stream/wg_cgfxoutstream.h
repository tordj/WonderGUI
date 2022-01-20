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

#ifndef	WG_CGFXOUTSTREAM_DOT_H
#define	WG_CGFXOUTSTREAM_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_pointers.h>
#include <wg_gfxstream.h>
#include <wg_types.h>
#include <wg_geo.h>
#include <wg_color.h>


namespace wg
{

	class CGfxOutStream;
	typedef	StrongComponentPtr<CGfxOutStream>	CGfxOutStream_p;
	typedef	WeakComponentPtr<CGfxOutStream>		CGfxOutStream_wp;


	//____ CGfxOutStream __________________________________________________________

	class CGfxOutStream : public Component, public GfxStream
	{
	public:

		//____ Holder ___________________________________________________

		class Holder /** @private */
		{
		public:
			virtual Object * _object() = 0;
			virtual const Object * _object() const = 0;

			virtual void	_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd) = 0;
		};

		CGfxOutStream(Holder * pHolder);

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		inline void	processChunks(const uint8_t* pBegin, const uint8_t* pEnd) { m_pHolder->_processStreamChunks(pBegin, pEnd); }

		//.____ Misc __________________________________________________

		inline CGfxOutStream_p	ptr() { return CGfxOutStream_p(this); }

	protected:
		Object *				_object() override { return m_pHolder->_object(); }
		const Object *			_object() const  override { return m_pHolder->_object(); }

		Holder * 	m_pHolder;
	};


} // namespace wg
#endif //WG_CGFXOUTSTREAM_DOT_H
