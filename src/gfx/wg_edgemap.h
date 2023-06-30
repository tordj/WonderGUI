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

#ifndef WG_EDGEMAP_DOT_H
#define WG_EDGEMAP_DOT_H
#pragma once

#include <wg_object.h>
#include <wg_gfxtypes.h>
#include <wg_color.h>
#include <wg_geo.h>
#include <wg_gradient.h>

namespace wg
{

	class Edgemap;
	typedef StrongPtr<Edgemap> Edgemap_p;
	typedef WeakPtr<Edgemap> Edgemap_wp;

	class Edgemap : public Object
	{
	public:

		const static int maxSegments = 16;			// We keep this global for now. Might never need to restrict it more.
		
		struct Blueprint
		{
			const HiColor*		colors 		= nullptr;
			const Gradient *	gradients 	= nullptr;
			int					segments	= 0;				// Mandatory.
			SizeI				size;							// Mandatory.
		};
	   	 
		
    	//.____ Identification __________________________________________

		const TypeInfo&     typeInfo(void) const override;
		const static TypeInfo   TYPEINFO;

		//.____ Geometry ______________________________________________________

		inline SizeI	pixelSize() const { return m_size; }

		//.____ Appearance ____________________________________________________

		virtual bool	setRenderSegments(int nSegments);
		inline int		renderSegments() const { return m_nbRenderSegments; }

		virtual HiColor	color(int segment) const;
		virtual Gradient gradient(int segment) const;

		//.____ Content _______________________________________________________

		inline int		segments() const { return m_nbSegments; }

		inline int		samples() const { return m_size.w + 1; }

		virtual bool 	importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) = 0;

		virtual bool 	importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) = 0;

		virtual bool 	exportSamples( WaveOrigo origo, spx * pDestination, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) = 0;

		virtual bool 	exportSamples( WaveOrigo origo, float * pDestination, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) = 0;

		
	protected:

		Edgemap( const Blueprint& bp ) : m_size(bp.size), m_nbSegments(bp.segments), m_nbRenderSegments(bp.segments) {};
		virtual ~Edgemap() {};
		
		static bool	_validateBlueprint(const Blueprint& bp);

		SizeI       m_size;
		int			m_nbSegments;
		int			m_nbRenderSegments;

		HiColor*	m_pColors = nullptr;		// Initialized by subclass.
		Gradient*	m_pGradients = nullptr;		// Initialized by subclass.
   };


} // namespace wg
#endif // WG_EDGEMAP_DOT_H
