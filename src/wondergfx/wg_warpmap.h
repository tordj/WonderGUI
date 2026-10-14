/*=========================================================================

                             >>> WonderGUI <<<

  This file is part of Tord Bärnfors' WonderGUI UI Toolkit and copyright
  Tord Bärnfors, Sweden [mail: first name AT barnfors DOT c_o_m].

                                -----------

  The WonderGUI UI Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                                -----------

  The WonderGUI UI Toolkit is also available for use in commercial
  closed source projects under a separate license. Interested parties
  should contact Bärnfors Technology AB [www.barnfors.com] for details.

=========================================================================*/
#ifndef WG_WARPMAP_DOT_H
#define WG_WARPMAP_DOT_H
#pragma once

#include <wg_object.h>
#include <wg_gfxtypes.h>
#include <wg_geo.h>

namespace wg
{
	class Warpmap;
	typedef	StrongPtr<Warpmap>	Warpmap_p;
	typedef	WeakPtr<Warpmap>	Warpmap_wp;

	//____ Warpmap _____________________________________________________________
	/**
	 * @brief Bends a surface or an edgemap into another shape when drawn.
	 *
	 * A Warpmap covers a rectangle of size.w * size.h pixels with a grid of nodes, one node every
	 * cellSize pixels in both directions plus one at the right and bottom edge. Each node holds
	 * the position in the source that is drawn at that point, normalized so that 0.0 -> 1.0 covers
	 * the source, u along its width and v along its height. Positions between nodes are interpolated
	 * bilinearly. Where a position falls outside 0.0 -> 1.0 nothing is drawn.
	 *
	 * Smaller cells follow curved shapes more closely. A circle of radius r is off by
	 * roughly cellSize^2 / (8 * r) pixels.
	 *
	 * An axis can be periodic (uPeriod/vPeriod > 0), as when u is the angle of a ring. Positions are
	 * then read modulo the period and interpolation between nodes across the seam takes the short way.
	 * With a period of 1.0 the source wraps around, which is seamless if the source is tiling. A period
	 * larger than 1.0 leaves a gap, such as a 270 degree arc having uPeriod = 360/270.
	 *
	 * Warpmaps are created by a WarpmapFactory, which is specific to the backend, or by a generator
	 * such as CircularWarp.
	 */

	class Warpmap : public Object
	{
	public:

		struct Blueprint
		{
			int				cellSize	= 16;				// Pixels between nodes.
			Finalizer_p		finalizer	= nullptr;
			SizeI			size;							// Mandatory. Size in pixels.
			float			uPeriod		= 0.f;				// 0 = not periodic.
			float			vPeriod		= 0.f;				// 0 = not periodic.
		};

		//.____ Identification __________________________________________

		const TypeInfo&     typeInfo(void) const override;
		const static TypeInfo   TYPEINFO;

		//.____ Geometry ______________________________________________________

		inline SizeI	pixelSize() const { return m_size; }
		inline int		cellSize() const { return m_cellSize; }
		inline SizeI	nodes() const { return m_nodes; }				// Nodes horizontally and vertically.

		//.____ Content _______________________________________________________

		virtual bool	setPeriods( float uPeriod, float vPeriod );
		inline float	uPeriod() const { return m_uPeriod; }
		inline float	vPeriod() const { return m_vPeriod; }

		// Nodes are stored row by row. Region is given in nodes, pitch is nodes between rows
		// in source/destination, 0 meaning region.w.

		bool			importNodes( const RectI& region, const CoordF * pSource, int pitch = 0 );
		bool			exportNodes( const RectI& region, CoordF * pDestination, int pitch = 0 ) const;

		inline const CoordF*	nodeData() const { return m_pNodes; }

	protected:
		Warpmap() {};
		Warpmap( const Blueprint& bp );
		virtual ~Warpmap();

		static bool		_validateBlueprint( const Blueprint& bp );

		// Called when nodes in region or the periods have changed (region is then all nodes).
		// Typically all you need to override when subclassing.

		virtual void	_nodesUpdated( const RectI& region ) = 0;

		SizeI			m_size;
		int				m_cellSize = 16;
		SizeI			m_nodes;
		float			m_uPeriod = 0.f;
		float			m_vPeriod = 0.f;

		CoordF*			m_pNodes = nullptr;					// m_nodes.w * m_nodes.h, row by row.
	};

} // namespace wg
#endif // WG_WARPMAP_DOT_H
