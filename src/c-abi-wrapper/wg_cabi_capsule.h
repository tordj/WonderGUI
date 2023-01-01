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
#ifndef WG_CABICAPSULE_DOT_H
#define WG_CABICAPSULE_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_cabi_root_interface.h>


//TODO: Need to support skin padding.

namespace wg
{

	class CABICapsule;
	typedef	StrongPtr<CABICapsule>		CABICapsule_p;
	typedef	WeakPtr<CABICapsule>		CABICapsule_wp;

	//____ CABICapsule ________________________________________________________
	/**
	* @brief	A widget that controls the size of a single child.
	*
	* The CABICapsule is used to change the size of a widget. This is done either by imposing
	* limits on its minimum and maximum size or by setting a specific default size.
	*
	* Width and height can be controlled separately
	* The CABICapsule overrides the min, max and default size reported by its child, thus
	* affecting its geometry. Exactly how the geometry is affected depends on the parent widget.
	*
	*/

	class CABICapsule : public Widget
	{
	public:

		//____ Blueprint __________________________________________

		struct Blueprint
		{

			Object_p		baggage;
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			int				scale = 64;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};

		//.____ Creation __________________________________________

		static CABICapsule_p	create( );
		static CABICapsule_p	create(const Blueprint& blueprint);

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance ____________________________________________________

		 void		setSkin(Skin* pSkin);

		inline  String tooltip() const;

		 PointerStyle pointerStyle() const;


		//.____ Internal _________________________________________________

		spx			_matchingHeight(spx width, int scale = -1) const override;
		spx			_matchingWidth(spx height, int scale = -1) const override;

		SizeSPX		_defaultSize(int scale = -1) const override;
		SizeSPX		_minSize(int scale = -1) const override;
		SizeSPX		_maxSize(int scale = -1) const override;

		bool		_markTest(const CoordSPX& ofs) override;

		void    	_preRender() override;
		void		_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window) override;

		void		_resize(const SizeSPX& size, int scale = -1) override;
		void		_setState(State state) override;

		void		_receive(Msg* pMsg) override;
		bool		_alphaTest(const CoordSPX& ofs) override;



		bool		_isRootVisible() const;
		RectSPX		_rootWindowSection() const;		// Returns the window section within the childs canvas.
		
		void		_rootRequestRender(const RectSPX& rect);
		void		_rootRequestResize();

		bool		_rootRequestFocus();
		bool		_rootReleaseFocus();

		void		_rootRequestInView(const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea);

		void		_connectRoot(wg_cabi_root_incalls* pCalls);
		void		_disconnectRoot();


	protected:
		CABICapsule();
		CABICapsule(const Blueprint& bp);
		 ~CABICapsule();


		 wg_cabi_root_incalls	m_cabi;

	};


} // namespace wg
#endif //WG_CABICAPSULE_DOT_H
