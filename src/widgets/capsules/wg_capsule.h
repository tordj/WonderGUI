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

#ifndef WG_CAPSULE_DOT_H
#define WG_CAPSULE_DOT_H

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

#ifndef WG_CHILDENTRY_DOT_H
#	include <wg_childentry.h>
#endif

#ifndef WG_SLOT_DOT_H
#	include <wg_slot.h>
#endif


namespace wg 
{
	
	class Capsule;
	typedef	StrongPtr<Capsule,Container_p>	Capsule_p;
	typedef	WeakPtr<Capsule,Container_p>	Capsule_wp;
	
	
	//____ Capsule ______________________________________________________________
	
	class Capsule : public Container, protected ChildEntryHolder
	{
	
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Capsule_p	cast( const Object_p& pObject );

		//____ Interfaces ______________________________________

		ChildEntry<Slot,Capsule> child;



		// Overloaded from Widget
	
		int				matchingHeight( int width ) const;
		int				matchingWidth( int height ) const;
	
		Size			preferredSize() const;
	
	protected:
		Capsule();
		virtual ~Capsule();

		// Overloaded from ChildHolder

		Coord		_childPos( void * pChildRef ) const;
		Size		_childSize( void * pChildRef ) const;

		void		_childRequestRender( void * pChildRef );
		void		_childRequestRender( void * pChildRef, const Rect& rect );
		void		_childRequestResize( void * pChildRef );

		Widget *	_prevChild( void * pChildRef ) const;
		Widget *	_nextChild( void * pChildRef ) const;

		// Overloaded from SlotCanHolder

		Object *	_object();
		const Object * _object() const;

		void		_setWidget( Slot * pSlot, Widget * pWidget );

		// Overloaded from Container

		Widget *	_firstChild() const;
		Widget *	_lastChild() const;

		void		_firstChildWithGeo( WidgetWithGeo& package ) const;
		void		_nextChildWithGeo( WidgetWithGeo& package ) const;
		
		//
	
		void		_collectPatches( Patches& container, const Rect& geo, const Rect& clip );
		void		_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode );
		void		_cloneContent( const Widget * _pOrg );
		void		_setSize( const Size& size );
	
	
	
		Slot		m_child;
	
	};
	

} // namespace wg
#endif //WG_CAPSULE_DOT_H
