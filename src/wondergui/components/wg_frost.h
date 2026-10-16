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
#ifndef	WG_FROST_DOT_H
#define	WG_FROST_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_component.h>
#include <wg_geo.h>
#include <wg_skin.h>
#include <wg_surface.h>
#include <wg_patches.h>
#include <wg_blurbrush.h>

#include <functional>
#include <vector>

namespace wg
{
	class GfxDevice;

	//____ Frost _____________________________________________________________
	/**
	 * @brief Frosted glass behind panes that are placed on top of a backdrop.
	 *
	 * Frost keeps the backdrop, the content behind the panes, cached in a surface. The
	 * widget renders the backdrop into the cache through _updateBackdrop() and copies it
	 * to the canvas with _renderBackdrop().
	 *
	 * Each pane gets its background from a blurred copy of the part of the backdrop it
	 * covers. The blur is cached in the Pane, which is owned by the widget (typically
	 * by a slot). It is recalculated when the pane has moved or changed size, when the
	 * settings of the Frost have changed and when the widget has invalidated the pane
	 * because the backdrop behind it has changed (see _paneDamage()). Content on top of
	 * a pane can therefore change without the blur being recalculated.
	 *
	 * The blur is calculated at a reduced resolution. The backdrop is shrunk by the
	 * downscale factor, with a blur of half the downscale on the way to keep thin details
	 * from flickering as panes move. It is then blurred in a number of passes, the first
	 * with the given radius and each following pass with half the radius of the previous
	 * one, which fills in the gaps that the sparse taps of a Blurbrush leave at large radii.
	 *
	 * After the blur, the color is blended on top, giving the frost a light or dark haze.
	 *
	 * A mask skin shapes the frost, i.e. gives it rounded corners. The frost is multiplied
	 * into the skin, so the alpha of the skin decides where the frost is and the color of
	 * the skin tints it. A white skin leaves the colors of the frost as they are. A mask
	 * costs a surface of the full size of the pane.
	 *
	 * Only the content of the backdrop is frosted, not panes below the pane. The backdrop
	 * is rendered without CanvasLayers.
	 */

	class Frost : public Component
	{
	public:

		//____ Pane ___________________________________________________________
		/**
		 * @brief The cached frost of one pane.
		 *
		 * Owned by whoever owns the pane, typically a slot.
		 */

		class Pane
		{
			friend class Frost;
		public:
			void		invalidate() { m_bValid = false; }
			bool		isValid() const { return m_bValid; }
			void		clear();						///< Releases the surfaces.

		protected:
			Surface_p	m_pBlur[2];						// Blurred backdrop at reduced resolution, ping-pong surfaces.
			int			m_result = 0;					// Index of the surface in m_pBlur holding the result.
			Surface_p	m_pMasked;						// Full resolution frost shaped by the mask skin.
			RectSPX		m_region;						// Area of the backdrop that m_pBlur covers.
			RectSPX		m_geo;							// Geometry of the pane in the backdrop when calculated.
			int			m_version = -1;					// Version of the Frost settings and backdrop when calculated.
			bool		m_bValid = false;
		};

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			HiColor		color = HiColor::Transparent;	// Blended on top of the blur.
			int			downscale = 4;					// Blur is calculated at 1/downscale of the resolution of the backdrop.
			Skin_p		mask;							// Frost is multiplied into this skin, shaping and tinting it.
			int			passes = 3;						// Number of blur passes, each with half the radius of the previous one.
			pts			radius = 16;					// Radius of the first blur pass.
			HiColor		tint = HiColor::White;			// Tint of the frost when rendered to the canvas.
		};

		Frost(Widget* pWidget);
		virtual ~Frost() {};

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _____________________________________________

		void				setColor(HiColor color);
		HiColor				color() const { return m_color; }

		void				setDownscale(int factor);
		int					downscale() const { return m_downscale; }

		void				setMask(Skin* pSkin);
		Skin_p				mask() const { return m_pMask; }

		void				setPasses(int passes);
		int					passes() const { return m_passes; }

		void				setRadius(pts radius);
		pts					radius() const { return m_radius; }

		void				setTint(HiColor tint);
		HiColor				tint() const { return m_tint; }

		//.____ Content _______________________________________________________

		Surface_p			backdrop() const { return m_pBackdrop; }

		//.____ Internal ______________________________________________________

		void				_initFromBlueprint(const Blueprint& blueprint);

		// The backdrop. Coordinates are relative to the backdrop.

		void				_setBackdropSize(SizeSPX size, int scale);
		void				_invalidateBackdrop(const RectSPX& rect);
		void				_invalidateBackdrop();
		void				_updateBackdrop(GfxDevice* pDevice, const std::function<void(GfxDevice* pDevice, const RectSPX& canvas)>& renderFunc);
		void				_renderBackdrop(GfxDevice* pDevice, const RectSPX& canvas);

		// Panes. Geometry of panes is relative to the backdrop.

		spx					_reach() const;
		RectSPX				_paneDamage(const RectSPX& backdropChange, const RectSPX& paneGeo) const;
		void				_renderPane(GfxDevice* pDevice, Pane& pane, const RectSPX& paneGeo, const RectSPX& canvas, State state);

	protected:

		void				_settingsChanged();
		void				_updatePane(GfxDevice* pDevice, Pane& pane, const RectSPX& paneGeo, State state);
		void				_updateBrushes();
		RectSPX				_blurSource(const Pane& pane, const RectSPX& area) const;
		int					_firstPassRadius() const;
		Surface_p			_createSurface(GfxDevice* pDevice, SizeI pixelSize);

		HiColor				m_color = HiColor::Transparent;
		int					m_downscale = 4;
		Skin_p				m_pMask;
		int					m_passes = 3;
		pts					m_radius = 16;
		HiColor				m_tint = HiColor::White;

		Surface_p			m_pBackdrop;
		PatchesSPX			m_backdropPatches;
		SizeSPX				m_backdropSize;
		int					m_scale = 64;

		int					m_version = 0;				// Increased when settings change or the backdrop is recreated.

		Blurbrush_p			m_pPrefilter;				// Blur used when shrinking the backdrop.
		std::vector<Blurbrush_p>	m_passBrushes;		// One for each pass.
		bool				m_bBrushesDirty = true;
	};


} // namespace wg
#endif	// WG_FROST_DOT_H
