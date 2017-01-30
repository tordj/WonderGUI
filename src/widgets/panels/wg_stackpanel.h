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

#ifndef WG_STACKPANEL_DOT_H
#define WG_STACKPANEL_DOT_H

#ifndef WG_VECTORPANEL_DOT_H
#	include <wg_vectorpanel.h>
#endif

namespace wg 
{
	
	class StackPanel;
	typedef	StrongPtr<StackPanel,VectorPanel_p>		StackPanel_p;
	typedef	WeakPtr<StackPanel,VectorPanel_wp>	StackPanel_wp;
	
	class StackHook;
	typedef	HookTypePtr<StackHook,VectorHook_p>		StackHook_p;
	
	//____ StackHook ____________________________________________________________
	
	class StackHook : public VectorHook
	{
		friend class StackPanel;
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static StackHook_p	cast( const Hook_p& pInterface );
	
		enum SizePolicy
		{
			DEFAULT,
			STRETCH,
			SCALE
		};
	
		void			setSizePolicy( SizePolicy policy );
		SizePolicy		getSizePolicy() const { return m_sizePolicy; }
			
		void			setOrigo( Origo origo );
		Origo	origo() const { return m_origo; }
	
		StackHook_p	prev() const { return _prev(); }
		StackHook_p 	next() const { return _next(); }
	
		StackPanel_p 	parent() const { return m_pParent; }
	
	protected:
		PROTECTED_LINK_METHODS( StackHook );
	
		StackHook( StackPanel * pParent );
	
		Container * _parent() const;
	
		Rect			_getGeo( const Rect& parentGeo ) const;
	
		StackPanel *	m_pParent;
		
		Origo	m_origo;
		SizePolicy		m_sizePolicy;
		
	};
	
	//____ StackPanel ___________________________________________________________
	
	class StackPanel : public VectorPanel
	{
	friend class StackHook;	
		
	public:
		static StackPanel_p	create() { return StackPanel_p(new StackPanel()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static StackPanel_p	cast( const Object_p& pObject );
		
		inline StackHook_p addWidget( const Widget_p& pWidget ) { return static_cast<StackHook*>(VectorPanel::_addWidget(pWidget.rawPtr())); }
		inline StackHook_p insertWidget( const Widget_p& pWidget, const Widget_p& pSibling ) { return static_cast<StackHook*>(VectorPanel::_insertWidget(pWidget.rawPtr(),pSibling.rawPtr())); }
		
		// Overloaded from Widget
	
		int		matchingHeight( int width ) const;
		int		matchingWidth( int height ) const;
	
		Size	preferredSize() const;
	
	protected:
		StackPanel();
		virtual ~StackPanel();
		virtual Widget* _newOfMyType() const { return new StackPanel(); };
	
		// Overloaded from Widget
	
		void	_cloneContent( const Widget * _pOrg );
		void	_setSize( const Size& size );
	
		// Overloaded from Panel
	
		Hook *	_firstHookWithGeo( Rect& writeGeo ) const;
		Hook *	_nextHookWithGeo( Rect& writeGeo, Hook * pHook ) const;
	
		Hook *	_lastHookWithGeo( Rect& writeGeo ) const;
		Hook *	_prevHookWithGeo( Rect& writeGeo, Hook * pHook ) const;
	
		// Overloaded from VectorPanel
	
		Rect	_hookGeo( const VectorHook * pHook );
		void	_requestResizeRequested( VectorHook * pHook );
		void	_renderRequested( VectorHook * pHook );
		void	_renderRequested( VectorHook * pHook, const Rect& rect );
		void	_onWidgetAppeared( VectorHook * pInserted );				// so parent can update geometry and possibly request render.
		void	_onWidgetDisappeared( VectorHook * pToBeRemoved );		// so parent can update geometry and possibly request render.
		void	_onWidgetsReordered();
		void	_refreshAllWidgets();
		VectorHook * _newHook();
	
		// Internal to StackPanel
	
		void 	_refreshPreferredSize();
		void	_adaptChildrenToSize();
		void	_renderFromChildOnward( VectorHook * pHook );
	
		inline StackHook *	_firstHook() const { return static_cast<StackHook*>(m_hooks.first()); }
		inline StackHook *	_lastHook() const { return static_cast<StackHook*>(m_hooks.last()); }
	
	
		Size	m_preferredSize;	
	};
	
	
	

} // namespace wg
#endif //WG_STACKPANEL_DOT_H
