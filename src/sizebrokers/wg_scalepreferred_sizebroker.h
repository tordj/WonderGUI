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
#ifndef	WG_SCALEPREFERRED_SIZEBROKER_DOT_H
#define	WG_SCALEPREFERRED_SIZEBROKER_DOT_H

#include <wg_sizebroker.h>

namespace wg 
{
	
	class ScalePreferredSizeBroker;
	typedef	StrongPtr<ScalePreferredSizeBroker,SizeBroker_p>		ScalePreferredSizeBroker_p;
	typedef	WeakPtr<ScalePreferredSizeBroker,SizeBroker_wp>	ScalePreferredSizeBroker_wp;
	
	class ScalePreferredSizeBroker : public SizeBroker
	{
	public:
		//.____ Creation __________________________________________

		static ScalePreferredSizeBroker_p	create() { return ScalePreferredSizeBroker_p(new ScalePreferredSizeBroker()); }
	
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ScalePreferredSizeBroker_p	cast( Object * pObject );
	
		//.____ Misc __________________________________________________

		int setItemLengths( SizeBrokerItem * pItems, int nItems, int totalLength ) const;
		int setPreferredLengths( SizeBrokerItem * pItems, int nItems ) const;
		bool mayAlterPreferredLengths() const;
	    
	protected:
		ScalePreferredSizeBroker() {};
	    virtual ~ScalePreferredSizeBroker() {};
	    
	};
	
	

} // namespace wg
#endif //WG_SCALEPREFERRED_SIZEBROKER_DOT_H
