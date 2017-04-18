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
#ifndef	WG_SPANITEM_DOT_H
#define WG_SPANITEM_DOT_H
#pragma once

#include <wg_item.h>

namespace wg 
{
		
	//____ SpanItem _____________________________________________________________
	
	class SpanItem : public Item
	{
	public:
		SpanItem( ItemHolder * pHolder );
		virtual ~SpanItem() {};
	
		bool	setMin( int min );
		bool	setMax( int max );
		bool	setRange( int min, int max );
	
		void	setSpan( int begin, int length );
		void	setBegin( int begin );
		void	setLength( int length );
	
		void	setRelativeSpan( float begin, float length );
		void	setRelativePos( float pos );
		void	setRelativeBegin( float begin );
		void	setRelativeLength( float length );
	
		bool	stepForward();
		bool	stepBackward();
		bool	skipForward();
		bool	skipBackward();
	
		inline float	relativePos() const { return (begin+length/2)/(float)(max-min); }
		inline float	relativeBegin() const { return begin/(float)(max-min); }
		inline float	relativeLength() const { return length/(float)(max-min); };
	
	
		const static int	MAX = 0x00FFFFFF;
		const static int	MIN = 0xFF000000;
	
		// Directly accessible for holder.
	
		int		min;
		int		max;
	
		int		begin;
		int		length;
	
	
	protected:
		void	_onModified() { _notify( ItemNotif::SpanModified, nullptr ); }
		int		_stepSize() { return 0; }	//TODO: Implement!!!
		int		_skipSize() { return 0; }	//TODO: Implement!!!
	};
	
	
	

} // namespace wg
#endif //WG_SPANITEM_DOT_H
