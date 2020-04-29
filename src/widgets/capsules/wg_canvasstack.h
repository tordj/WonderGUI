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
#ifndef WG_CANVASSTACK_DOT_H
#define WG_CANVASSTACK_DOT_H
#pragma once

#include <wg_capsule.h>
#include <wg_canvas.h>
#include <wg_patches.h>
#include <vector>

namespace wg
{

	class CanvasStack;
	typedef	StrongPtr<CanvasStack>		CanvasStack_p;
	typedef	WeakPtr<CanvasStack>	CanvasStack_wp;

	//____ CanvasStack ________________________________________________________

	class CanvasStack : public Capsule
	{
	public:
		//.____ Creation __________________________________________

		static CanvasStack_p	create() { return CanvasStack_p(new CanvasStack()); }

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;



		void		setPreferredSize(Size preferred);
		Size		preferredSize() const override;
		MU			matchingWidth(MU height) const override;
		MU			matchingHeight(MU height) const override;


		void		setCanvases(int nbCanvases);
		Surface_p	canvas(int idx) const;
		void		forceRedraw();

		void		setCreateFunc(const std::function<Surface_p(int, SizeI)>& func);
		void		setClearFunc(const std::function<void(GfxDevice*, std::vector<Surface_p>&)>& func);
		void		setCombineFunc(const std::function<void(GfxDevice*, Surface*, std::vector<Surface_p>&)>& func);

		Skin_p		redirectSkin(Skin * pSkin, int canvasIdx);
		Capsule_p	redirectWidget(Widget * pWidget, int canvasIdx);

	protected:
		CanvasStack();
		~CanvasStack();
		Widget*		_newOfMyType() const override { return new CanvasStack(); };

		void		_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		void		_cloneContent(const Widget * _pOrg) override;
		void		_resize(const Size& size) override;

		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const Rect& rect) override;

	private:

		Surface_p	_createCanvas(int canvasIdx, SizeI size);
		void		_clearCanvases(GfxDevice * pDevice);
		void		_combineCanvases(GfxDevice * pDevice);

		void		_regenCanvases(int nbCanvases, SizeI canvasSize);

		Size					m_preferred;
		bool					m_bRendering = false;

		std::vector<Surface_p>	m_canvases;
		Surface_p				m_pEndCanvas;
		Patches					m_patches;			// Patches from internal request render calls.

		std::function<Surface_p(int, SizeI)>								m_createFunc;
		std::function<void(GfxDevice*, std::vector<Surface_p>&)>				m_clearFunc;
		std::function<void(GfxDevice*, Surface*, std::vector<Surface_p>&)>	m_combineFunc;

		//____ CustomSkin _____________________________________________________

		class CustomSkin : public Skin
		{
		public:
			CustomSkin(CanvasStack * pHolder, Skin * pSkin, int canvasIdx);

			const TypeInfo&		typeInfo(void) const override;
			const static TypeInfo	TYPEINFO;

			Size	minSize() const override;
			Size	preferredSize() const override;

			Size	sizeForContent(const Size& contentSize) const override;
			Border	contentPadding(State state) const override;
			Size	contentPaddingSize() const override;
			Coord	contentOfs(State state) const override;
			Rect	contentRect(const Rect& canvas, State state) const override;

			bool	isOpaque() const override;
			bool	isOpaque(State state) const override;
			bool	isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

			bool	isStateIdentical(State state, State comparedTo, float fraction = 1.f) const override;

			bool	markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction = 1.f) const override;
			void 	render(GfxDevice * pDevice, const Rect& canvas, State state, float fraction = 1.f) const override;

			bool	ignoresFraction() const override;
			Rect	fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const override;

		protected:

			CanvasStack_wp	m_pStack;
			Skin_p			m_pSkin;
			int				m_canvasIdx;
		};

		//____ CustomCapsule __________________________________________________

		class CustomCapsule : public Capsule
		{
		public:
			CustomCapsule() {};
			CustomCapsule(CanvasStack * pHolder, int canvasIdx);

			const TypeInfo&	typeInfo(void) const override;
			const static TypeInfo	TYPEINFO;

		protected:
			virtual Widget* _newOfMyType() const override { return new CustomCapsule(); };

			void			_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;

		private:
			CanvasStack_wp	m_pStack;
			int				m_canvasIdx = 0;
		};

	};


} // namespace wg
#endif //WG_CANVASSTACK_DOT_H
