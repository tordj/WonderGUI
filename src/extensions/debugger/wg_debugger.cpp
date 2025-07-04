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
#include <wg_debugger.h>

#include <wg_dummyinfopanel.h>

#include <objectinfopanels/wg_objectinfopanel.h>
#include <objectinfopanels/wg_widgetinfopanel.h>

#include <slotinfopanels/wg_staticslotinfopanel.h>


namespace wg
{

	const TypeInfo Debugger::TYPEINFO = { "Debugger", &Object::TYPEINFO };


	Widget_p factory( Object *)
	{
		return Widget_p();
	}

	//____ constructor _____________________________________________________________

	Debugger::Debugger()
	{
		m_objectInfoFactories[&Object::TYPEINFO] = [](const DebugPanel::Blueprint& panelBP, Object* pObject) { return (Widget_p) ObjectInfoPanel::create(panelBP, pObject); };

		m_objectInfoFactories[&Widget::TYPEINFO] = [](const DebugPanel::Blueprint& panelBP, Object* pObject) { return (Widget_p) WidgetInfoPanel::create(panelBP, (Widget*) pObject); };

		m_slotInfoFactories[&StaticSlot::TYPEINFO] = [](const DebugPanel::Blueprint& panelBP, StaticSlot* pSlot) { return (Widget_p) StaticSlotInfoPanel::create(panelBP, pSlot); };


	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Debugger::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ createDebugPanel() ____________________________________________________

	Widget_p Debugger::createDebugPanel( const DebugPanel::Blueprint& bp, const TypeInfo * pType, Object * pObject )
	{

		auto it = m_objectInfoFactories.find( pType );
		if( it == m_objectInfoFactories.end() )
		{
			return DummyInfoPanel::create(bp,pObject);
		}

		return it->second(bp,pObject);
	}

	//____ createDebugPanel() ____________________________________________________

	Widget_p Debugger::createDebugPanel( const DebugPanel::Blueprint& bp, const TypeInfo * pType, StaticSlot * pSlot )
	{

		auto it = m_slotInfoFactories.find( pType );
		if( it == m_slotInfoFactories.end() )
		{
			return DummyInfoPanel::create(bp,pSlot);
		}

		return it->second(bp,pSlot);
	}


} // namespace wg

