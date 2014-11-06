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

	inline int		Min() const { return m_min; }
	inline int		Max() const { return m_max; }
	inline int 		Begin() const { return m_begin; }
	inline int		Length() const { return m_length; }

	inline float	RelativePos() const { return (m_begin+m_length/2)/(float)(m_max-m_min); }
	inline float	RelativeBegin() const { return m_begin/(float)(m_max-m_min); }
	inline float	RelativeLength() const { return m_length/(float)(m_max-m_min); };


	const static int	MAX = 0x00FFFFFF;
	const static int	MIN = 0xFF000000;
protected:
	void	_onModified() { static_cast<WgSpanHolder*>(m_pHolder)->_onSpanModified(this); }
	int		_stepSize() { static_cast<WgSpanHolder*>(m_pHolder)->_spanStepSize(this); }
	int		_skipSize() { static_cast<WgSpanHolder*>(m_pHolder)->_spanSkipSize(this); }

	int		m_min;
	int		m_max;

	int		m_begin;
	int		m_length;
};



#endif //WG_SPANITEM_DOT_H
