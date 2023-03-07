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

#ifndef WG_SURFACEREADER_DOT_H
#define WG_SURFACEREADER_DOT_H
#pragma once

#include <istream>

#include <wg_surfacefileheader.h>
#include <wg_surfacefactory.h>

namespace wg
{


	class SurfaceReader;
	typedef	StrongPtr<SurfaceReader>SurfaceReader_p;
	typedef	WeakPtr<SurfaceReader>	SurfaceReader_wp;


class SurfaceReader : public Object
{
public:

	struct Blueprint
	{
		SurfaceFactory_p	factory;
	};

	//.____ Creation __________________________________________

	static SurfaceReader_p	create( const Blueprint& blueprint );

	//.____ Identification _________________________________________________

	const TypeInfo&		typeInfo(void) const override;
	const static TypeInfo	TYPEINFO;

	//.____ Misc _________________________________________________________________
	
	Surface_p		readSurfaceFromStream( std::istream& stream );
	Surface_p		readSurfaceFromStream(std::istream& stream, const Surface::Blueprint& extraFlags );

	Surface_p		readSurfaceFromBlob(const Blob* pBlob);
	Surface_p		readSurfaceFromBlob( const Blob * pBlob, const Surface::Blueprint& extraFlags );

	Surface_p		readSurfaceFromMemory(const char* pData);
	Surface_p		readSurfaceFromMemory( const char * pData, const Surface::Blueprint& extraFlags );
	
protected:
	SurfaceReader( const Blueprint& bp );
	virtual ~SurfaceReader() {}
	
	Surface::Blueprint	_blueprintFromHeader( const SurfaceFileHeader * pHeader );
	
	int				_addFlagsFromOtherBlueprint(Surface::Blueprint& dest, const Surface::Blueprint& extraFlags);

	Surface_p		_readSurfFromStream(std::istream& stream, const Surface::Blueprint& extraFlags);
	Surface_p		_readQOIFromStream(std::istream& stream, const Surface::Blueprint& extraFlags);


	SurfaceFactory_p	m_pFactory;
};


//==============================================================================

} // namespace wg
#endif // WG_SURFACEREADER_DOT_H



