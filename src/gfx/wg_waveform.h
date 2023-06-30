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
#ifndef WAVEFORM_DOT_H
#define WAVEFORM_DOT_H
#pragma once

#include <wg_edgemap.h>
#include <wg_edgemapfactory.h>
#include <wg_edgemaptools.h>
#include <wg_color.h>

namespace wg
{
	class Waveform;
	typedef	StrongPtr<Waveform>	Waveform_p;
	typedef	WeakPtr<Waveform>	Waveform_wp;

	class Waveform : public Object
	{
	public:
		
		struct Blueprint
		{
			spx					bottomOutlineThickness = 1.f;
			HiColor				color = Color8::LightGrey;
			Gradient			gradient;								// Overrides color when set.
			WaveOrigo			origo = WaveOrigo::Top;
			HiColor				outlineColor = Color8::DarkGrey;
			Gradient			outlineGradient;						// Overrides outlineColor when set.
			SizeI				size;									// Mandatory
			spx					topOutlineThickness = 1.f;
		};
		

		//.____ Creation __________________________________________

		static Waveform_p	create( const Blueprint& blueprint, EdgemapFactory * pFactory = nullptr );


		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content __________________________________________________

		void setSamples( int sampleBegin, int sampleEnd, spx * pTopSamples, spx * pBottomSamples );
		void setSamples( int sampleBegin, int sampleEnd, float * pTopSamples, float * pBottomSamples );

		void setFlatTopLine( int sampleBegin, int sampleEnd, spx sample );
		void setFlatTopLine( int sampleBegin, int sampleEnd, float sample );

		void setFlatBottomLine( int sampleBegin, int sampleEnd, spx sample );
		void setFlatBottomLine( int sampleBegin, int sampleEnd, float sample );

		Edgemap_p	refresh();
				
	protected:

		Waveform( const Blueprint& bp, EdgemapFactory * pFactory );
		virtual ~Waveform();
		
		WaveOrigo	m_origo;
		
		HiColor		m_color;

		HiColor		m_outlineColor;
		spx			m_topOutlineThickness;
		spx			m_bottomOutlineThickness;

		Edgemap_p	m_pEdgemap;

		int			m_nbSamples;
		spx *		m_pSamples;				// Top/bottom interleaved.
		
		int			m_dirtBegin;
		int			m_dirtEnd;
	};

}

#endif //WAVEFORM_DOT_H
