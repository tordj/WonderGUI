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

#include	<wg_graphdisplay.h>

namespace wg
{

	const TypeInfo GraphDisplay::TYPEINFO = { "GraphDisplay", &Widget::TYPEINFO };




	Graph::Graph(const Blueprint& bp)
	{

	}

	void Graph::setColors(HiColor fill, HiColor outline, ColorTransition* pTransition)
	{

	}

	void Graph::setGradients(Gradient fill, Gradient outline)
	{

	}

	void Graph::setRange(float begin, float end)
	{

	}

	void Graph::setTopSamples(int nSamples, float* pSamples)
	{

	}

	void Graph::setBottomSamples(int nSamples, float* pSamples)
	{

	}

	void Graph::transitionSamples(ArrayTransition* pTransition, int nTopSamples, float* pNewTopSamples, int nBottomSamples, float* pNewBottomSamples)
	{

	}

	void Graph::setVisible(bool bVisible)
	{

	}

	//____ topSamples() _______________________________________________________

	std::tuple<int, const float*>  Graph::topSamples() const 
	{ 
		return std::make_tuple(m_topSamples.size(), m_topSamples.data()); 
	}

	//____ bottomSamples() ____________________________________________________

	std::tuple<int, const float*>  Graph::bottomSamples() const 
	{ 
		return std::make_tuple(m_bottomSamples.size(), m_bottomSamples.data()); 
	}


}