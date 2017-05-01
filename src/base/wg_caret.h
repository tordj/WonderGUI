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

#ifndef	WG_CARET_DOT_H
#define WG_CARET_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_pointers.h>

namespace wg 
{
	
	class GfxDevice;
	
	class Caret;
	typedef	StrongPtr<Caret,Object_p>		Caret_p;
	typedef	WeakPtr<Caret,Object_wp>		Caret_wp;
	
	
	enum class CaretMode
	{
		Insert,
		Overwrite,
		Eol
	};
	
	
	class Caret : public Object
	{
	public:
		//.____ Creation __________________________________________

		static Caret_p	create() { return Caret_p(new Caret()); }

		//.____ Identification __________________________________________
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Caret_p		cast( const Object_p& pObject );

		//.____ Control ________________________________________________________
	
		virtual bool		setCycleLength( int millisec );
		inline int			cycleLength() const { return m_cycleLength; }
		virtual bool		restartCycle();
		virtual bool		setMode( CaretMode mode );
		inline CaretMode	mode() const { return m_mode; }


		//.____ Internal ___________________________________________________________

		virtual int			eolWidth( const Size& eolCell ) const;
		virtual Rect		dirtyRect( Rect cell ) const;
		virtual bool		tick( int millisec );
		inline bool			needToRender() const { return m_bNeedToRender; }
		virtual void		render( GfxDevice * pDevice, Rect cell, const Rect& clip );

	
	protected:
		Caret();

		bool		_updateNeedToRender( int oldTicks, int newTicks );

		
		CaretMode	m_mode;
		int			m_ticks;
		int			m_cycleLength;
		bool		m_bNeedToRender;
	
	};
	

} // namespace wg
#endif //WG_CARET_DOT_H
