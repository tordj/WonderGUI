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
	
	class StackPanelSlot : public PanelSlot
	{
	public:
		StackPanelSlot() : origo(Origo::Center), sizePolicy(SizePolicy2D::Default) {}
	
		Origo			origo;
		SizePolicy2D	sizePolicy;		
	};


	class StackPanelChildren;
	typedef	StrongInterfacePtr<StackPanelChildren,Interface_p>	StackPanelChildren_p;
	typedef	WeakInterfacePtr<StackPanelChildren,Interface_wp>	StackPanelChildren_wp;

	//____ StackPanelChildren ________________________________________________________

	class StackPanelChildren : public PanelChildren<StackPanelSlot,StackPanel>
	{
	public:
		StackPanelChildren( SlotArray<StackPanelSlot> * pSlotArray, StackPanel * pHolder ) : PanelChildren<StackPanelSlot,StackPanel>(pSlotArray,pHolder) {}

		inline StackPanelChildren_p	ptr() { return StackPanelChildren_p(_object(),this); }


		void		add( const Widget_p& pWidget );
		bool		insert( int index, const Widget_p& pWidget );
		bool		remove( int index );

		void		setSizePolicy( int index, SizePolicy2D policy );
		SizePolicy2D sizePolicy( int index ) const;

		void		setOrigo( int index, Origo origo );
		Origo		origo( int index ) const;
	};



	//____ StackPanel ___________________________________________________________
	
	/**
	*/

	class StackPanel : public Panel, protected PanelSlotsHolder
	{
		friend class StackPanelChildren;

	public:

		//.____ Creation __________________________________________

		static StackPanel_p	create() { return StackPanel_p(new StackPanel()); }

		//.____ Components _______________________________________

		StackPanelChildren	children;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static StackPanel_p	cast( const Object_p& pObject );

		//.____ Geometry ____________________________________________
	
		int			matchingHeight( int width ) const;
		int			matchingWidth( int height ) const;
	
		Size		preferredSize() const;

		//.____ Hierarchy _________________________________________________

		bool		removeChild( const Widget_p& pWidget );
		bool		clear();
	
	protected:
		StackPanel();
		virtual ~StackPanel();
		virtual Widget* _newOfMyType() const { return new StackPanel(); };
	
		// Overloaded from Widget
	
		void		_cloneContent( const Widget * _pOrg );
		void		_setSize( const Size& size );

		// Overloaded from Container

		Widget *	_firstChild() const;
		Widget *	_lastChild() const;
				
		void		_firstChildWithGeo( WidgetWithGeo& package ) const;
		void		_nextChildWithGeo( WidgetWithGeo& package ) const;

		// Overloaded from PanelSlotsHolder

		void		_didAddSlots( Slot * pSlot, int nb );
		void		_willRemoveSlots( Slot * pSlot, int nb );
		void		_hideSlots( PanelSlot *, int nb );
		void		_unhideSlots( PanelSlot *, int nb );
		void		_repadSlots( PanelSlot *, int nb, Border padding );


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
//		void	_renderFromChildOnward( VectorHook * pHook );	

		void	_hideChildren( StackPanelSlot * pSlot, int nb );
		void	_unhideChildren( StackPanelSlot * pSlot, int nb );


		Rect	_childGeo( const StackPanelSlot * pSlot ) const;
	
		Size	m_preferredSize;	

		SlotArray<StackPanelSlot>	m_children;
	};
	

} // namespace wg
#endif //WG_STACKPANEL_DOT_H
