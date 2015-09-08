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
	
	class Patches;
	
	class Panel;
	typedef	WgStrongPtr<Panel,Container_p>		Panel_p;
	typedef	WgWeakPtr<Panel,Container_wp>	Panel_wp;
	
	class PanelHook;
	typedef	HookTypePtr<PanelHook,Hook_p>	PanelHook_p;
	
	
	//____ PanelHook ____________________________________________________________
	
	class PanelHook : public Hook
	{
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static PanelHook_p	cast( const Hook_p& pInterface );
	
		PanelHook_p	prev() const { return static_cast<PanelHook*>(_prevHook()); }
		PanelHook_p	next() const { return static_cast<PanelHook*>(_nextHook()); }
		Panel_p		parent() const;
	
		virtual bool	setVisible( bool bVisible );
		bool			isVisible() { return m_bVisible; }
	
		virtual bool	setPadding( Border padding );
		Border		padding() const { return m_padding; }
	
	protected:
		PanelHook() : m_bVisible(true) {}
		 virtual ~PanelHook() {};
	
		Size		_paddedPreferredSize() const;
		Size		_paddedMinSize() const;
		Size		_paddedMaxSize() const;
		int			_paddedMatchingWidth( int paddedHeight ) const;
		int			_paddedMatchingHeight( int paddedWidth ) const;
		bool		_isVisible() const { return m_bVisible; }				// Overloading Hook::_isVisible().
	
		Size		_sizeFromPolicy( Size specifiedSize, SizePolicy widthPolicy, SizePolicy heightPolicy ) const;
	
		bool			m_bVisible;
		Border		m_padding;
	};
	
	/**
	 * @brief	Base class for layout widgets.
	 *
	 * Base class for layout widgets.
	 */
	
	//____ Panel ________________________________________________________________
	
	class Panel : public Container
	{
	public:
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static Panel_p	cast( const Object_p& pObject );
		
		void		setTooltipGroup( bool bTooltipGroup ) { m_bTooltipGroup = bTooltipGroup; }
		bool		isTooltipGroup() const { return m_bTooltipGroup; }
	
		void		setMaskOp( WgMaskOp operation );
		WgMaskOp	maskOp() const { return m_maskOp; }
	
		inline PanelHook_p	firstHook() const { return static_cast<PanelHook*>(_firstHook()); }
		inline PanelHook_p	lastHook() const { return static_cast<PanelHook*>(_lastHook()); }
	
		// Overloaded from IWidgets
	
	
	protected:
		Panel();
		virtual ~Panel() {};
	
		bool			_isPanel() const;
		virtual void	_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, WgBlendMode blendMode );
		virtual void	_onCloneContent( const Widget * _pOrg );
	
	
		bool		m_bTooltipGroup;	// All descendants belongs to the same tooltip group.
		WgMaskOp	m_maskOp;			// Specifies how container masks background.
	};
	
	
	

} // namespace wg
#endif //WG_PANEL_DOT_H
