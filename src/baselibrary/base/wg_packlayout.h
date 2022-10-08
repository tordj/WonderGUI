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
#ifndef	WG_PACKLAYOUT_DOT_H
#define	WG_PACKLAYOUT_DOT_H
#pragma once

#include <wg_pointers.h>
#include <wg_types.h>

#include <vector>
#include <functional>

namespace wg
{


	class PackLayout;
	typedef	StrongPtr<PackLayout>	PackLayout_p;
	typedef	WeakPtr<PackLayout>		PackLayout_wp;

	class PackLayout : public Object
	{
	public:

		enum class WantedSize
		{
			Default,
			MaxDefault,
			MaxDefaultTimesWeight,
			DefaultTimesWeight,
		};

		enum class StartSize
		{
			Default,
			MaxDefault,
			DefaultTimesWeight,
			MaxDefaultTimesWeight,
			DefaultAsFraction
		};

		enum class Factor
		{
			Zero,								// Don't shrink at all.
			One,								// Shrink all by same amount.
			Margin,								// Shrink all by same relative amount towards min size, reaching min size at the same time.
			Size,								// Shrink all by same percentage.

			Weight,								// Shrink all by their weight, relative to total weight.
			InverseWeight,						// 

			SizeTimesWeight,					// Shrink all relative to their size multiplied by weight.
			SizeTimesInverseWeight,				// Shrink all relative to their size multiplied by weight.

			MarginTimesWeight,					// Shrink all by 
			MarginTimesInverseWeight,

			List1,								// Shrink all by their relative value in list 1.
			List2,								// Shrink all by their relative value in list 2.
			List3,								// Shrink all by their relative value in list 3.

			Ordered								// Columns will grow and shrink in order.
		};

		enum class SizeSource
		{
			Item,
			Weight,
			List1,					// Item sizes expressed in pts.
			List2,
			List3
		};

		enum class HideSetting
		{
			Never,					// Never hide an item.
			Restrained,				// Start hiding items (in reverse priority order) when none can shrink anymore.
			Aggressive				// Hide an item as soon as it hits its minSize limit.
		};


		struct Item
		{
			spx		def;			// Default length for this item.
			spx		min;			// Min length for this item.
			spx		max;			// Max length for this item.
			int		weight;			// Weight for this item. 16 binals.
		};

		struct Blueprint
		{
			WantedSize			wantedSize = WantedSize::Default;
			StartSize			startSize = StartSize::Default;
			Factor				expandFactor = Factor::Zero;
			Factor				shrinkFactor = Factor::Zero;
			HideSetting			hideSetting = HideSetting::Never;

			std::vector<float>	list1;
			std::vector<float>	list2;
			std::vector<float>	list3;

			SizeSource			minSizeSource = SizeSource::Item;
			SizeSource			defaultSizeSource = SizeSource::Item;
			SizeSource			maxSizeSource = SizeSource::Item;

			std::vector<int>	expandOrder;
			std::vector<int>	shrinkOrder;

		};


		//.____ Creation __________________________________________

		static PackLayout_p	create(const Blueprint& blueprint);


		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Misc __________________________________________

		virtual spx getItemSizes(spx* pOutput, spx availableSpace, int scale, int nItems, const Item* pItems);
		virtual spx getWantedSizes(spx* pOutput, int scale, int nItems, const Item* pItems);
		virtual bool doesCalcWantedLength();

	protected:
		PackLayout(const Blueprint& blueprint);
		virtual ~PackLayout();

		std::tuple<const spx*, int>	_defaultSizes(  const Item* pItems, const spx * pWeightSPX );
		std::tuple<const spx*, int>	_minSizes( const Item* pItems, const spx* pWeightSPX );
		std::tuple<const spx*, int>	_maxSizes( const Item* pItems, const spx* pWeightSPX );
		
		void						_updateSpxCache(int scale);
		void				_convertList(std::vector<int>& to, const std::vector<float>& from);

		uint64_t			_getItemFactors(uint64_t* pOutput, Factor factor, int nItems, const Item* pItems, const spx * pSizes, const spx* pWeightSpx);

		WantedSize			m_wantedSize;
		StartSize			m_startSize;
		HideSetting			m_hideSetting;
		Factor				m_shrinkFactor;
		Factor				m_expandFactor;

		std::vector<int>	m_list1;			// 16.16 format
		std::vector<int>	m_list2;
		std::vector<int>	m_list3;

		int					m_cacheScale = 0;
		std::vector<spx>	m_list1_spxCache;
		std::vector<spx>	m_list2_spxCache;
		std::vector<spx>	m_list3_spxCache;

		std::vector<int>	m_expandOrder;
		std::vector<int>	m_shrinkOrder;


		SizeSource			m_minSizeSource;
		SizeSource			m_defaultSizeSource;
		SizeSource			m_maxSizeSource;

		bool				m_bCalcWantedLength = false;
		bool				m_bUseSpxWeight = false;

	};


} // namespace wg
#endif //WG_PACKLAYOUT_DOT_H
