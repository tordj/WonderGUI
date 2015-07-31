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
#ifndef	WG_MODVALUEFIELD_DOT_H
#define WG_MODVALUEFIELD_DOT_H

#ifndef WG_VALUEFIELD_DOT_H
#	include <wg_valuefield.h>
#endif

namespace wg 
{
	
	
	class WgModValueField;
	
	//____ WgModValueHolder ___________________________________________________________
	
	class WgModValueHolder : public WgValueHolder
	{
	public:
		virtual void		_onValueModified( WgModValueField * pField ) = 0;
	};
	
	
	//____ WgModValueField ____________________________________________________________
	
	class WgModValueField : public WgValueField
	{
	public:
		WgModValueField( WgModValueHolder * pHolder ) : WgValueField(pHolder), m_minValue(INT64_MIN), m_maxValue(INT64_MAX) {}
		~WgModValueField() {}
	
		// ModValue methods
	
		void				clear();
		bool				set( Sint64 value, int scale );
		
		bool				setRange( Sint64 min, Sint64 max );
		inline Sint64		min() const { return m_minValue; }
		inline Sint64		max() const { return m_maxValue; }
	
		void				onValueModified() { static_cast<WgModValueHolder*>(m_pHolder)->_onValueModified(this); }
	
	protected:
		Sint64				m_minValue;
		Sint64				m_maxValue;
	};
	
	

} // namespace wg
#endif //WG_MODVALUEFIELD_DOT_H
