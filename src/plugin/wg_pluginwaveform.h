/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free Ioware; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Ioware Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef	WG_PLUGINWAVEFORM_DOT_H
#define	WG_PLUGINWAVEFORM_DOT_H
#pragma once


#include <wg_waveform.h>

typedef void* wg_obj;
typedef struct wg_waveformBP_struct wg_waveformBP;

namespace wg
{

	class PluginWaveform;
	typedef	StrongPtr<PluginWaveform>	PluginWaveform_p;
	typedef	WeakPtr<PluginWaveform>	    PluginWaveform_wp;

	//____ PluginWaveform _____________________________________________________________

	class PluginWaveform : public Waveform
	{
		friend class PluginWaveformFactory;

	public:

		//.____ Creation __________________________________________

		static PluginWaveform_p	create( wg_obj object );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance ____________________________________________________

		HiColor		color(int segment) const override;
		Gradient	gradient(int segment) const override;


		//.____ Content _______________________________________________________

		bool 		importSamples(	WaveOrigo origo, const spx* pSource, int edgeBegin, int edgeEnd,
									int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0) override;

		bool 		importSamples(	WaveOrigo origo, const float* pSource, int edgeBegin, int edgeEnd,
									int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0) override;

		bool 		exportSamples(	WaveOrigo origo, spx* pDestination, int edgeBegin, int edgeEnd,
									int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0) override;

		bool 		exportSamples(	WaveOrigo origo, float* pDestination, int edgeBegin, int edgeEnd,
									int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0) override;

		//.____ Misc _________________________________________________________
		 
		inline wg_obj		cObject() { return m_cWaveform; }

	private:
		PluginWaveform( wg_obj object );
		~PluginWaveform();

		wg_obj		m_cWaveform;

	};
} // namespace wg
#endif //WG_PLUGINWAVEFORM_DOT_H

