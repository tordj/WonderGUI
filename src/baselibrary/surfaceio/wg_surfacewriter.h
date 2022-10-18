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

#ifndef WG_SURFACEWRITER_DOT_H
#define WG_SURFACEWRITER_DOT_H
#pragma once

#include <ostream>

#include <wg_surfacefileheader.h>
#include <wg_surfacefactory.h>

namespace wg
{


	class SurfaceWriter;
	typedef	StrongPtr<SurfaceWriter>SurfaceWriter_p;
	typedef	WeakPtr<SurfaceWriter>	SurfaceWriter_wp;




class SurfaceWriter : public Object
{
public:

	struct SaveInfo
	{
		bool identity		= true;
		bool baggage 		= false;
		bool sampleMethod 	= false;
		bool scale			= true;
		bool buffered		= false;
		bool canvas			= false;
		bool dynamic		= false;
		bool tiling			= true;
		bool mipmap			= false;
	};
	
	struct Blueprint
	{
		SaveInfo			saveInfo;
	};

	//.____ Creation __________________________________________

	static SurfaceWriter_p	create( const Blueprint& blueprint );

	//.____ Identification _________________________________________________

	const TypeInfo&	typeInfo(void) const override;
	const static TypeInfo	TYPEINFO;
	
	void			writeSurfaceToStream( std::ostream& stream, Surface * pSurface, int extraDataSize = 0, char * pExtraData = nullptr );

protected:
	SurfaceWriter( const Blueprint& bp );
	virtual ~SurfaceWriter() {}
	
	SaveInfo		m_saveInfo;
	
};


//==============================================================================

} // namespace wg
#endif // WG_SURFACEREADER_DOT_H

