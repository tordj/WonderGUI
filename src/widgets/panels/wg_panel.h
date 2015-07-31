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

namespace wg 
{
	
	class WgPatches;
	
	class WgPanel;
	typedef	WgStrongPtr<WgPanel,WgContainer_p>		WgPanel_p;
	typedef	WgWeakPtr<WgPanel,WgContainer_wp>	WgPanel_wp;
	
	class WgPanelHook;
	typedef	WgHookTypePtr<WgPanelHook,WgHook_p>	WgPanelHook_p;
	
	
	//____ WgPanelHook ____________________________________________________________
	
	class WgPanelHook : public WgHook
	{
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WgPanelHook_p	cast( const WgHook_p& pInterface );
	
		WgPanelHook_p	prev() const { return static_cast<WgPanelHook*>(_prevHook()); }
		WgPanelHook_p	next() const { return static_cast<WgPanelHook*>(_nextHook()); }
		WgPanel_p		parent() const;
	
		virtual bool	setVisible( bool bVisible );
		bool			isVisible() { return m_bVisible; }
	
		virtual bool	setPadding( WgBorder padding );
		WgBorder		padding() const { return m_padding; }
	
	protected:
		WgPanelHook() : m_bVisible(true) {}
		 virtual ~WgPanelHook() {};
	
		WgSize		_paddedPreferredSize() const;
		WgSize		_paddedMinSize() const;
		WgSize		_paddedMaxSize() const;
		int			_paddedMatchingWidth( int paddedHeight ) const;
		int			_paddedMatchingHeight( int paddedWidth ) const;
		bool		_isVisible() const { return m_bVisible; }				// Overloading WgHook::_isVisible().
	
		WgSize		_sizeFromPolicy( WgSize specifiedSize, WgSizePolicy widthPolicy, WgSizePolicy heightPolicy ) const;
	
		bool			m_bVisible;
		WgBorder		m_padding;
	};
	
	/**
	 * @brief	Base class for layout widgets.
	 *
	 * Base class for layout widgets.
	 */
	
	//____ WgPanel ________________________________________________________________
	
	class WgPanel : public WgContainer
	{
	public:
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgPanel_p	cast( const WgObject_p& pObject );
	
		void		setFocusGroup( bool bFocusGroup ) { m_bFocusGroup = bFocusGroup; }
		bool		isFocusGroup() const { return m_bFocusGroup; }
	
		void		setTooltipGroup( bool bTooltipGroup ) { m_bTooltipGroup = bTooltipGroup; }
		bool		isTooltipGroup() const { return m_bTooltipGroup; }
	
		void		setMaskOp( WgMaskOp operation );
		WgMaskOp	maskOp() const { return m_maskOp; }
	
		inline WgPanelHook_p	firstHook() const { return static_cast<WgPanelHook*>(_firstHook()); }
		inline WgPanelHook_p	lastHook() const { return static_cast<WgPanelHook*>(_lastHook()); }
	
		// Overloaded from WgIWidgets
	
	
	protected:
		WgPanel();
		virtual ~WgPanel() {};
	
		bool			_isPanel() const;
		virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
		virtual void	_onCloneContent( const WgWidget * _pOrg );
	
	
		bool		m_bFocusGroup;
		bool		m_bTooltipGroup;	// All descendants belongs to the same tooltip group.
		WgMaskOp	m_maskOp;			// Specifies how container masks background.
	};
	
	
	

} // namespace wg
#endif //WG_PANEL_DOT_H
