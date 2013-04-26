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
#ifndef	WG_MULTIRULE_SIZEBROKER_DOT_H
#define	WG_MULTIRULE_SIZEBROKER_DOT_H

#ifndef	WG_SIZEBROKER_DOT_H
#	include <wg_sizebroker.h>
#endif

class WgMultiRuleSizeBroker : public WgSizeBroker
{
public:
	WgMultiRuleSizeBroker();
	virtual ~WgMultiRuleSizeBroker();
    
	int SetItemLengths( WgSizeBrokerItem * pItems, int nItems, int totalLength ) const;
	int SetPreferredLengths( WgSizeBrokerItem * pItems, int nItems ) const;
	bool MayAlterPreferredLengths() const;

protected:
};


#endif //WG_MULTIRULE_SIZEBROKER_DOT_H