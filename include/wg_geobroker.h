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
#ifndef	WG_GEOBROKER_DOT_H
#define	WG_GEOBROKER_DOT_H


class WgGeoBroker
{
public:
	struct ItemData
	{
		int		def;			// Default space for this item (input)
		int		min;			// Min space for this item (input)
		int		max;			// Max space for this item (input)
		int		weight;			// Weight for this item (input)
		int		ofs_out;		// Offset for this item (output)
		int		size_out;		// Size for this item (output)
	};

	WgGeoBroker();
	WgGeoBroker( int(*fp)(ItemData*,int,int) );
	~WgGeoBroker() {}


	int Allocate( ItemData * pItems, int nItems, int totalSpace ) const { return m_function(pItems,nItems,totalSpace); }

	static const WgGeoBroker none;
//	static const WgGeoBroker even;
//	static const WgGeoBroker overlap;
//	static const WgGeoBroker weighted;


private:

	int(*m_function)(ItemData*,int,int);

};


#endif //WG_GEOBROKER_DOT_H