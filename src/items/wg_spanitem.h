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

#ifndef WG_ITEM_DOT_H
#	include <wg_item.h>
#endif

namespace wg 
{
	
	class SpanItem;
	
	//____ SpanHolder ___________________________________________________________
	
	class SpanHolder : public ItemHolder
	{
	public:
		virtual void	_onSpanModified( SpanItem * pItem ) = 0;
		virtual int		_spanStepSize( SpanItem * pItem ) = 0;
		virtual int		_spanSkipSize( SpanItem * pItem ) = 0;
	};
	
	//____ SpanItem _____________________________________________________________
	
	class SpanItem : public Item
	{
		friend class SpanHolder;
	public:
		SpanItem( SpanHolder * pHolder );
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
		void	_onModified() { static_cast<SpanHolder*>(m_pHolder)->_onSpanModified(this); }
		int		_stepSize() { return static_cast<SpanHolder*>(m_pHolder)->_spanStepSize(this); }
		int		_skipSize() { return static_cast<SpanHolder*>(m_pHolder)->_spanSkipSize(this); }
	};
	
	
	

} // namespace wg
#endif //WG_SPANITEM_DOT_H
