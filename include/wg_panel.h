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

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

class WgPatches;

class WgPanel;
typedef	WgSmartPtr<WgPanel,WgContainerPtr>		WgPanelPtr;
typedef	WgWeakPtr<WgPanel,WgContainerWeakPtr>	WgPanelWeakPtr;

class WgPanelHook;
typedef	WgHookTypePtr<WgPanelHook,WgHookPtr>	WgPanelHookPtr;


//____ WgPanelHook ____________________________________________________________

class WgPanelHook : public WgHook
{
public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgPanelHookPtr	Cast( const WgHookPtr& pInterface );

	WgPanelHookPtr	Prev() const { return static_cast<WgPanelHook*>(_prevHook()); }
	WgPanelHookPtr	Next() const { return static_cast<WgPanelHook*>(_nextHook()); }
	WgPanelPtr		Parent() const;

	virtual bool	SetVisible( bool bVisible );
	bool			IsVisible() { return m_bVisible; }

	virtual bool	SetPadding( WgBorders padding );
	WgBorders		Padding() const { return m_padding; }

protected:
	WgPanelHook() : m_bVisible(true) {}
	 virtual ~WgPanelHook() {};

	WgSize		_paddedPreferredSize() const;
	WgSize		_paddedMinSize() const;
	WgSize		_paddedMaxSize() const;
	int			_paddedWidthForHeight( int paddedHeight ) const;
	int			_paddedHeightForWidth( int paddedWidth ) const;
	bool		_isVisible() const { return m_bVisible; }				// Overloading WgHook::_isVisible().

	WgSize		_sizeFromPolicy( WgSize specifiedSize, WgSizePolicy widthPolicy, WgSizePolicy heightPolicy ) const;

	bool			m_bVisible;
	WgBorders		m_padding;
};

/**
 * @brief	Super class for container widgets for layout of children.
 * 
 */

//____ WgPanel ________________________________________________________________

class WgPanel : public WgContainer
{
public:
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgPanelPtr	Cast( const WgObjectPtr& pObject );

	void		SetFocusGroup( bool bFocusGroup ) { m_bFocusGroup = bFocusGroup; }
	bool		IsFocusGroup() const { return m_bFocusGroup; }

	void		SetRadioGroup( bool bRadioGroup ) { m_bRadioGroup = bRadioGroup; }
	bool		IsRadioGroup() const { return m_bRadioGroup; }

	void		SetTooltipGroup( bool bTooltipGroup ) { m_bTooltipGroup = bTooltipGroup; }
	bool		IsTooltipGroup() const { return m_bTooltipGroup; }

	void		SetMaskOp( WgMaskOp operation );
	WgMaskOp	MaskOp() const { return m_maskOp; }

	inline WgPanelHookPtr	FirstHook() const { return static_cast<WgPanelHook*>(_firstHook()); }
	inline WgPanelHookPtr	LastHook() const { return static_cast<WgPanelHook*>(_lastHook()); }

	// Overloaded from WgIWidgets


protected:
	WgPanel();
	virtual ~WgPanel() {};

	bool			_isPanel() const;
	virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	virtual void	_onCloneContent( const WgWidget * _pOrg );


	bool		m_bFocusGroup;
	bool		m_bRadioGroup;
	bool		m_bTooltipGroup;	// All descendants belongs to the same tooltip group.
	WgMaskOp	m_maskOp;			// Specifies how container masks background.
};



#endif //WG_PANEL_DOT_H
