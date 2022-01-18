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
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_object.h>
#include <wg_pointers.h>
#include <wg_gfxstream.h>


namespace wg
{

	class CGfxInStream;
	typedef	StrongComponentPtr<CGfxInStream>	CGfxInStream_p;
	typedef	WeakComponentPtr<CGfxInStream>		CGfxInStream_wp;


	//____ CGfxInStream ________________________________________________________

	class CGfxInStream : public Component, GfxStream
	{
	public:

		//____ Holder ___________________________________________________

		class Holder /** @private */
		{
		public:

			//TODO: Add const version?
			virtual Object * _object() = 0;
			virtual const Object * _object() const = 0;

			virtual bool	_hasChunk() = 0;
			virtual GfxStream::Header	_peekChunk() = 0;	// Is only called if _hasChunk() has returned true.

			virtual char	_pullChar() = 0;
			virtual short	_pullShort() = 0;
			virtual int		_pullInt() = 0;
			virtual float	_pullFloat() = 0;
			virtual void	_pullBytes(int nBytes, char * pBytes) = 0;
			virtual void	_skipBytes(int nBytes) = 0;

			virtual bool	_isStreamOpen() = 0;
			virtual void	_closeStream() = 0;
			virtual bool	_reopenStream() = 0;
		};



		CGfxInStream(Holder * pHolder) : m_pHolder(pHolder) {};

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		inline void		close() { m_pHolder->_closeStream(); }
		inline bool		isOpen() { return m_pHolder->_isStreamOpen(); }
		inline bool		reopen() { return m_pHolder->_reopenStream(); }

		void			skip(int bytes) { m_pHolder->_skipBytes(bytes); }

		bool				isEmpty();
		GfxStream::Header	peek();

		//.____ Misc __________________________________________________

		inline CGfxInStream_p	ptr() { return CGfxInStream_p(this); }

		//.____ Operators _____________________________________________

		CGfxInStream& operator>> (Header& header);

		CGfxInStream& operator>> (int16_t&);
		CGfxInStream& operator>> (uint16_t&);
		CGfxInStream& operator>> (int&);
		CGfxInStream& operator>> (float&);
		CGfxInStream& operator>> (bool&);

		CGfxInStream& operator>> (CoordI&);
		CGfxInStream& operator>> (CoordF&);
		CGfxInStream& operator>> (SizeI&);
		CGfxInStream& operator>> (SizeF&);
		CGfxInStream& operator>> (RectI&);
		CGfxInStream& operator>> (RectF&);
		CGfxInStream& operator>> (BorderI&);

		CGfxInStream& operator>> (HiColor&);
		CGfxInStream& operator>> (Direction&);
		CGfxInStream& operator>> (BlendMode&);
        CGfxInStream& operator>> (TintMode&);
		CGfxInStream& operator>> (Axis&);
		CGfxInStream& operator>> (PixelFormat&);
		CGfxInStream& operator>> (ScaleMode&);
        CGfxInStream& operator>> (GfxFlip&);
        CGfxInStream& operator>> (XSections&);
        CGfxInStream& operator>> (YSections&);
		CGfxInStream& operator>> (CanvasRef&);

        CGfxInStream& operator>> (const DataChunk&);

		CGfxInStream& operator>> (int[2][2]);
		CGfxInStream& operator>> (float[2][2]);


	protected:
		Object *		_object() override { return m_pHolder->_object(); }
		const Object *	_object() const override { return m_pHolder->_object(); }

		Holder * 		m_pHolder;
	};


} // namespace wg
#endif //WG_CGFXINSTREAM_DOT_H
#pragma once
