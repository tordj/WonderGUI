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

#ifndef	WG_TEXTFIELD_DOT_H
#define WG_TEXTFIELD_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_PRESENTABLEFIELD_DOT_H
#	include <wg_presentablefield.h>
#endif


#ifndef WG_BASE_DOT_H
#	include <wg_base.h>
#endif

class WgString;
class WgCharSeq;
class WgCharBuffer;

//____ WgTextHolder ___________________________________________________________

struct WgTextHolder : public WgPresentableHolder
{
};

//____ WgTextField __________________________________________________________________

class WgTextField : public WgPresentableField
{
public:
	WgTextField( WgTextHolder * pHolder );

	virtual void		clear();

	virtual void		set( const WgCharSeq& seq );
	virtual void		set( const WgCharBuffer * buffer );
	virtual void		set( const WgString& str );

	virtual int			append( const WgCharSeq& seq );
	virtual int			insert( int ofs, const WgCharSeq& seq );
	virtual int			replace( int ofs, int nDelete, const WgCharSeq& seq );
	virtual int			remove( int ofs, int len );


	inline int			length() const { return m_charBuffer.length(); }
	inline bool			isEmpty() const { return length()==0?true:false; }
	WgTextLink_p		getMarkedLink() const;

protected:

};

#endif //WG_TEXTFIELD_DOT_H
