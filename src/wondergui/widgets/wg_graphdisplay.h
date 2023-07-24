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

#ifndef	WG_GRAPHDISPLAY_DOT_H
#define WG_GRAPHDISPLAY_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_dynamicvector.h>

namespace wg
{


	class Graph
	{
		
	};





	class GraphDisplay;
	typedef	StrongPtr<GraphDisplay>		GraphDisplay_p;
	typedef	WeakPtr<GraphDisplay>	GraphDisplay_wp;

	class GraphDisplay : public Widget , private DynamicVector<Graph>::Holder
	{
	public:

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
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};


		//.____ Creation __________________________________________

		static GraphDisplay_p	create( const Blueprint& blueprint ) { return GraphDisplay_p(new GraphDisplay()); }


		//.____ Components _______________________________________

		DynamicVector<Graph>	graphs;


		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

	protected:
		GraphDisplay();
		virtual ~GraphDisplay();

		void		_didAddEntries(Graph* pEntry, int nb) override;
		void		_didMoveEntries(Graph* pFrom, Graph* pTo, int nb) override;
		void		_willEraseEntries(Graph* pEntry, int nb) override;

		void		_setState(State state) override;
		void		_update(int microPassed, int64_t microsecTimestamp) override;
		void		_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window) override;

	private:

	};




} // namespace wg
#endif //GRAPHDISPLAY_DOT_H
