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
#ifndef WG_FILLER_DOT_H
#define WG_FILLER_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_skin.h>

namespace wg 
{
	
	class Filler;
	typedef	StrongPtr<Filler,Widget_p>		Filler_p;
	typedef	WeakPtr<Filler,Widget_wp>	Filler_wp;
	
	//____ Filler ____________________________________________________________
	/**
	 * @brief Simplest kind of widget.
	 *
	 * Filler is the simplest kind of widget that displays nothing but its skin
	 * and does nothing but posting the normal mouse messages that every widget does.
	 *
	 * It is typically used as backgrounds and for filling out empty spaces in
	 * organized layouts.
	 *
	 **/
	
	
	class Filler : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static Filler_p	create() { return Filler_p(new Filler()); }
	
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Filler_p		cast( Object * pObject );

		//.____ Geometry ____________________________________________
	
		void	setPreferredSize( const Size& size );
		Size	preferredSize() const;
	
	protected:
		Filler();
		virtual ~Filler();
		virtual Widget* _newOfMyType() const { return new Filler(); };
	
		void	_cloneContent( const Widget * _pOrg );
	
	private:
		Size			m_preferredSize;
	};
	
	

} // namespace wg
#endif //WG_FILLER_DOT_H
