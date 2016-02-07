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
#ifndef	WG_VALUEFIELD_DOT_H
#define WG_VALUEFIELD_DOT_H

#ifndef WG_PRESENTABLEFIELD_DOT_H
#	include <wg_presentablefield.h>
#endif

#ifndef WG_VALUEFORMATTER_DOT_H
#	include <wg_valueformatter.h>
#endif

namespace wg 
{
	
	
	//____ ValueHolder ___________________________________________________________
	
	class ValueHolder : public PresentableHolder
	{
	public:
	};
	
	//____ ValueField ____________________________________________________________
	
	class ValueField : public PresentableField
	{
	public:
		ValueField(ValueHolder * pHolder);
	
		void				setFormatter( const ValueFormatter_p& pFormatter );
		void				clearFormatter();
		inline ValueFormatter_p	formatter() const { return m_pFormatter; }
	
		virtual void		clear();
		virtual bool		set( int64_t value, int scale );
		
		void				onRefresh();
		inline int64_t		value() const { return m_value; }
		inline int			scale() const { return m_scale; }
	
	protected:
		void				_regenText();
	
		int64_t					m_value;
		int						m_scale;
		
		ValueFormatter_p		m_pFormatter;
	};
	
	

} // namespace wg
#endif //WG_VALUEFIELD_DOT_H
