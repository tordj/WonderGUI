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

#ifndef	WG_PANEL_DOT_H
#define	WG_PANEL_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

class WgPatches;


class WgPanelHook : public WgHook
{
public:
	WgPanelHook *	Prev() const { return static_cast<WgPanelHook*>(_prevHook()); }
	WgPanelHook *	Next() const { return static_cast<WgPanelHook*>(_nextHook()); }
	WgPanel*		Parent() const;

	virtual bool	SetVisible( bool bVisible );
	bool			IsVisible() { return m_bVisible; }

	virtual bool	SetPadding( WgBorders padding );
	WgBorders		Padding() const;

protected:
	WgPanelHook() : m_bVisible(true) {}
	 virtual ~WgPanelHook() {};

	 WgWidgetHolder* _holder() const { return _parent(); }

	WgSize		_paddedPreferredSize() const { return m_pWidget->PreferredSize() + m_padding; }
	int			_paddedWidthForHeight( int paddedHeight ) const { return m_pWidget->WidthForHeight( paddedHeight - m_padding.Height() ) + m_padding.Width(); }
	int			_paddedHeightForWidth( int paddedWidth ) const { return m_pWidget->HeightForWidth( paddedWidth - m_padding.Width() ) + m_padding.Height(); }

	WgSize		_sizeFromPolicy( WgSize specifiedSize, WgSizePolicy widthPolicy, WgSizePolicy heightPolicy ) const;

	bool			m_bVisible;
	WgBorders		m_padding;
};


class WgPanel : public WgContainer
{
public:
	void		SetFocusGroup( bool bFocusGroup ) { m_bFocusGroup = bFocusGroup; }
	bool		IsFocusGroup() const { return m_bFocusGroup; }

	void		SetRadioGroup( bool bRadioGroup ) { m_bRadioGroup = bRadioGroup; }
	bool		IsRadioGroup() const { return m_bRadioGroup; }

	void		SetTooltipGroup( bool bTooltipGroup ) { m_bTooltipGroup = bTooltipGroup; }
	bool		IsTooltipGroup() const { return m_bTooltipGroup; }

	void		SetMaskOp( WgMaskOp operation );
	WgMaskOp	MaskOp() const { return m_maskOp; }



	inline WgPanelHook *	FirstHook() const { return static_cast<WgPanelHook*>(_firstHook()); }
	inline WgPanelHook *	LastHook() const { return static_cast<WgPanelHook*>(_lastHook()); }

	// Overloaded from WgWidgetHolder

	bool				IsPanel() const;

	WgPanel *			CastToPanel();
	const WgPanel *		CastToPanel() const;

protected:
	WgPanel();
	virtual ~WgPanel() {};

	virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	virtual bool 	_onAlphaTest( const WgCoord& ofs );
	virtual void	_onCloneContent( const WgPanel * _pOrg );


	bool		m_bFocusGroup;
	bool		m_bRadioGroup;
	bool		m_bTooltipGroup;	// All descendants belongs to the same tooltip group.
	WgMaskOp	m_maskOp;			// Specifies how container masks background.
};



#endif //WG_PANEL_DOT_H
