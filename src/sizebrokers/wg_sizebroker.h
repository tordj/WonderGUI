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
#ifndef	WG_SIZEBROKER_DOT_H
#define	WG_SIZEBROKER_DOT_H

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

namespace wg 
{
	
	class SizeBrokerItem
	{
	public:	
		int		preferred;		// Preferred length for this item (input)
		int		min;			// Min length for this item (input)
		int		max;			// Max length for this item (input)
		float	weight;			// Weight for this item (input)
		int		output;			// Length or preferred length for this item (output)
	};
	
	
	class SizeBroker;
	typedef	WgStrongPtr<SizeBroker,Object_p>		SizeBroker_p;
	typedef	WgWeakPtr<SizeBroker,Object_wp>	SizeBroker_wp;
	
	class SizeBroker : public Object
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static SizeBroker_p	cast( const Object_p& pObject );
	    
		virtual int setItemLengths( SizeBrokerItem * pItems, int nItems, int totalLength ) const = 0;
		virtual int setPreferredLengths( SizeBrokerItem * pItems, int nItems ) const = 0;
		virtual bool mayAlterPreferredLengths() const = 0;
	
	protected:
		SizeBroker();
		virtual ~SizeBroker();
	};
	

} // namespace wg
#endif //WG_SIZEBROKER_DOT_H
