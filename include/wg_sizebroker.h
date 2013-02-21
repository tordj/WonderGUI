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
		int		preferred;		// Preferred length for this item (input)
		int		min;			// Min length for this item (input)
		int		max;			// Max length for this item (input)
		int		weight;			// Weight for this item (input)
		int		length;			// Length for this item (output)
	};

	WgSizeBroker();
	virtual ~WgSizeBroker();

	virtual int SetItemLengths( ItemData * pItems, int nItems, int totalLength ) const;
	virtual int PreferredLength( const ItemData * pItems, int nItems ) const;

private:

};


#endif //WG_SIZEBROKER_DOT_H