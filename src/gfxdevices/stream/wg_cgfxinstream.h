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

#ifndef	WG_CGFXINSTREAM_DOT_H
#define	WG_CGFXINSTREAM_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_types.h>
#include <wg_pointers.h>

#include <tuple>

namespace wg
{

	class CGfxInStream;
	typedef	StrongComponentPtr<CGfxInStream>	CGfxInStream_p;
	typedef	WeakComponentPtr<CGfxInStream>		CGfxInStream_wp;


	//____ CGfxInStream ________________________________________________________

	class CGfxInStream : public Component
	{
	public:

		//____ Holder ___________________________________________________

		class Holder /** @private */
		{
		public:

			//TODO: Add const version?
			virtual Object * _object() = 0;
			virtual const Object * _object() const = 0;

			virtual bool _hasStreamChunks() const = 0;
			virtual std::tuple<int, const DataSegment *> _showStreamChunks() = 0;
			virtual	void _discardStreamChunks(int bytes) = 0;
			virtual bool _fetchStreamChunks() = 0;
		};


		CGfxInStream(Holder * pHolder) : m_pHolder(pHolder) {};

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		inline bool hasChunks() const { return m_pHolder->_hasStreamChunks(); }
		inline std::tuple<int, const DataSegment*> showChunks() { return m_pHolder->_showStreamChunks(); }
		inline void discardChunks(int bytes) { return m_pHolder->_discardStreamChunks(bytes); }
		inline bool fetchChunks() { return m_pHolder->_fetchStreamChunks(); }

		//.____ Misc __________________________________________________

		inline CGfxInStream_p	ptr() { return CGfxInStream_p(this); }

	protected:
		Object *		_object() override { return m_pHolder->_object(); }
		const Object *	_object() const override { return m_pHolder->_object(); }

		Holder * 			m_pHolder;
	};


} // namespace wg
#endif //WG_CGFXINSTREAM_DOT_H
#pragma once
