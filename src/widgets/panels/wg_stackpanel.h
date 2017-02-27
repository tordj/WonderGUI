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

#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif

#ifndef WG_CHILDGROUP_DOT_H
#	include <wg_childgroup.h>
#endif


namespace wg 
{
	
	class StackPanel;
	typedef	StrongPtr<StackPanel,Panel_p>	StackPanel_p;
	typedef	WeakPtr<StackPanel,Panel_wp>	StackPanel_wp;
	




	//____ StackPanelSlot ____________________________________________________________
	
	class StackPanelSlot : public Slot
	{
	public:
		Origo			m_origo;
		SizePolicy		m_sizePolicy;		
	};


	//____ StackPanelChildren ________________________________________________________

	class StackPanelChildren : public ChildGroup<StackPanelSlot>
	{
	public:
		StackPanelChildren( SlotArray<StackPanelSlot> * pSlotArray );

		void		add( const Widget_p& pWidget );
		void		insert( const Widget_p& pWidget );

		void		setSizePolicy( int index, SizePolicy policy );
		SizePolicy	sizePolicy( int index ) const;

		void		setOrigo( int index, Origo origo );
		Origo		origo( int index ) const;
	};



	//____ StackPanel ___________________________________________________________
	
	class StackPanel : public Panel, protected SlotArrayHolder
	{
	public:
		static StackPanel_p	create() { return StackPanel_p(new StackPanel()); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static StackPanel_p	cast( const Object_p& pObject );

		//____ Interfaces ______________________________________

		StackPanelChildren	children;

		enum SizePolicy
		{
			DEFAULT,
			STRETCH,
			SCALE
		};
		
		// Overloaded from Widget
	
		int		matchingHeight( int width ) const;
		int		matchingWidth( int height ) const;
	
		Size	preferredSize() const;
	
	protected:
		StackPanel();
		virtual ~StackPanel();
		virtual Widget* _newOfMyType() const { return new StackPanel(); };
	
		// Overloaded from Widget
	
		void		_cloneContent( const Widget * _pOrg );
		void		_setSize( const Size& size );
	
		// Overloaded from Panel
	
		void		_firstChildWithGeo( WidgetWithGeo& package ) const;
		void		_nextChildWithGeo( WidgetWithGeo& package ) const;

		// Overloaded from SlotArrayHolder

		Object *	_object();
		const Object * _object() const;

		void		_didAddSlots( Slot * pSlot, int nb );
		void		_willRemoveSlots( Slot * pSlot, int nb );

		// Overloaded from WidgetHolder

		Coord		_childPos( void * pChildRef ) const;
		Size		_childSize( void * pChildRef ) const;

		void		_childRequestRender( void * pChildRef );
		void		_childRequestRender( void * pChildRef, const Rect& rect );
		void		_childRequestResize( void * pChildRef );

		Widget *	_prevChild( void * pChildRef ) const;
		Widget *	_nextChild( void * pChildRef ) const;

		// Internal to StackPanel
	
		void 	_refreshPreferredSize();
		void	_adaptChildrenToSize();
		void	_renderFromChildOnward( VectorHook * pHook );	
	
		Size	m_preferredSize;	

		SlotArray<StackPanelSlot>	m_children;
	};
	

} // namespace wg
#endif //WG_STACKPANEL_DOT_H
