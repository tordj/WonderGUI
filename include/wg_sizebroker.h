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


class WgSizeBroker
{
public:
	struct ItemData
	{
		int		pref;			// Preferred size for this item (input)
		int		min;			// Min size for this item (input)
		int		max;			// Max size for this item (input)
		int		weight;			// Weight for this item (input)
		int		size_out;		// Size for this item (output)
	};

	WgSizeBroker();
	WgSizeBroker( int(*fp)(ItemData*,int,int) );
	~WgSizeBroker() {}


	int Allocate( ItemData * pItems, int nItems, int totalSpace ) const { return m_function(pItems,nItems,totalSpace); }

	static const WgSizeBroker none;
//	static const WgSizeBroker even;
//	static const WgSizeBroker overlap;
//	static const WgSizeBroker weighted;


private:

	int(*m_function)(ItemData*,int,int);

};


#endif //WG_SIZEBROKER_DOT_H