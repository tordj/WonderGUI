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

class WgSpanItem;

//____ WgSpanHolder ___________________________________________________________

class WgSpanHolder : public WgItemHolder
{
public:
	virtual void	_onSpanModified( WgSpanItem * pItem ) = 0;
	virtual int		_spanStepSize( WgSpanItem * pItem ) = 0;
	virtual int		_spanSkipSize( WgSpanItem * pItem ) = 0;
};

//____ WgSpanItem _____________________________________________________________

class WgSpanItem : public WgItem
{
	friend class WgSpanHolder;
public:
	WgSpanItem( WgSpanHolder * pHolder );
	virtual ~WgSpanItem() {};

	bool	SetMin( int min );
	bool	SetMax( int max );
	bool	SetRange( int min, int max );

	void	SetSpan( int begin, int length );
	void	SetBegin( int begin );
	void	SetLength( int length );

	void	SetRelativeSpan( float begin, float length );
	void	SetRelativePos( float pos );
	void	SetRelativeBegin( float begin );
	void	SetRelativeLength( float length );

	bool	StepForward();
	bool	StepBackward();
	bool	SkipForward();
	bool	SkipBackward();

	inline float	RelativePos() const { return (begin+length/2)/(float)(max-min); }
	inline float	RelativeBegin() const { return begin/(float)(max-min); }
	inline float	RelativeLength() const { return length/(float)(max-min); };


	const static int	MAX = 0x00FFFFFF;
	const static int	MIN = 0xFF000000;

	// Directly accessible for holder.

	int		min;
	int		max;

	int		begin;
	int		length;


protected:
	void	_onModified() { static_cast<WgSpanHolder*>(m_pHolder)->_onSpanModified(this); }
	int		_stepSize() { return static_cast<WgSpanHolder*>(m_pHolder)->_spanStepSize(this); }
	int		_skipSize() { return static_cast<WgSpanHolder*>(m_pHolder)->_spanSkipSize(this); }
};



#endif //WG_SPANITEM_DOT_H
