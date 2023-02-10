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

#include <wg_util.h>
#include <wg_geo.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_patches.h>
#include <wg_base.h>
#include <wg_state.h>

#include <algorithm>

namespace wg
{

	//____ markTestStretchRect() __________________________________________________

	bool Util::markTestStretchRect( CoordSPX ofs, Surface * pSurface, const RectSPX& source, const RectSPX& area, int opacityTreshold )
	{
		// Sanity check & shortcuts.
		if( !pSurface || !area.contains(ofs) || source.isEmpty() || area.isEmpty() || opacityTreshold > 4096 )
			return false;

		if( pSurface->isOpaque() || opacityTreshold <= 0 )
			return true;

		// Make cordinates relative area.

		ofs.x -= area.x;
		ofs.y -= area.y;

		// Convert offset in area to offset in bitmap.

		CoordSPX sourceOfs;

		sourceOfs.x = (int) (ofs.x/((double)area.w) * source.w);
		sourceOfs.y = (int) (ofs.y/((double)area.h) * source.h);

		// Do alpha test

		int alpha = pSurface->alpha( sourceOfs );

		return (alpha >= opacityTreshold);
	}

	//____ markTestTileRect() __________________________________________________

	bool Util::markTestTileRect(CoordSPX ofs, Surface* pSurface, const RectSPX& area, int scale, int opacityTreshold)
	{
		// Sanity check & shortcuts.
		if (!pSurface || !area.contains(ofs) || area.isEmpty() || opacityTreshold > 4096)
			return false;

		if (pSurface->isOpaque() || opacityTreshold <= 0)
			return true;

		// Make cordinates relative area.

		ofs.x -= area.x;
		ofs.y -= area.y;

		// Convert offset in area to offset in bitmap.

		SizeSPX tileOnScreen = pSurface->pixelSize() * 64 * scale / pSurface->scale();

		CoordSPX sourceOfs = ofs * pSurface->scale() / scale;

		SizeI size = pSurface->pixelSize()*64;

		sourceOfs.x %= size.w;
		sourceOfs.y %= size.h;

		// Do alpha test

		int alpha = pSurface->alpha(sourceOfs);

		return (alpha >= opacityTreshold);
	}

	bool Util::markTestNinePatch(CoordSPX ofs, Surface* pSurface, const NinePatch& patch, const RectSPX& _dest, int scale, int opacityTreshold)
	{
		const BorderSPX destFrame = align(ptsToSpx(patch.frame, scale));
		const BorderSPX sourceFrame = align(ptsToSpx(patch.frame,pSurface->scale()));
		const RectSPX _source = align(ptsToSpx(patch.block, pSurface->scale()));



		// Sanity check & shortcuts.

		if (sourceFrame.isEmpty() && patch.rigidPartXSections == YSections::None && patch.rigidPartYSections == XSections::None )
			return markTestStretchRect(ofs, pSurface, _source, _dest, opacityTreshold);

		if (!pSurface || !_dest.contains(ofs) || _source.isEmpty() || _dest.isEmpty() || opacityTreshold > 4096)
			return false;

		if (pSurface->isOpaque() || opacityTreshold <= 0)
			return true;

		//

		RectSPX source;
		RectSPX dest;

		XSections markedSectionX;
		YSections markedSectionY;

		if (ofs.x < _dest.x + destFrame.left)
		{
			// left section
			markedSectionX = XSections::Left; 

			source.x = _source.x;
			source.w = sourceFrame.left;
			dest.x = _dest.x;
			dest.w = destFrame.left;
		}
		else if (ofs.x < _dest.x + _dest.w - destFrame.right)
		{
			// mid section
			markedSectionX = XSections::Center;

			source.x = _source.x + sourceFrame.left;
			source.w = _source.w - sourceFrame.width();
			dest.x = _dest.x + destFrame.left;
			dest.w = _dest.w - destFrame.width();
		}
		else
		{
			// right section
			markedSectionX = XSections::Right;

			source.x = _source.x + _source.w - sourceFrame.right;
			source.w = sourceFrame.right;
			dest.x = _dest.x + _dest.w - destFrame.right;
			dest.w = destFrame.right;
		}

		if (ofs.y < _dest.y + destFrame.top)
		{
			// top section
			markedSectionY = YSections::Top;

			source.y = _source.y;
			source.h = sourceFrame.top;
			dest.y = _dest.y;
			dest.h = destFrame.top;
		}
		else if (ofs.y < _dest.y + _dest.h - destFrame.bottom)
		{
			// mid section
			markedSectionY = YSections::Center;

			source.y = _source.y + sourceFrame.top;
			source.h = _source.h - sourceFrame.height();
			dest.y = _dest.y + destFrame.top;
			dest.h = _dest.h - destFrame.height();
		}
		else
		{
			// bottom section
			markedSectionY = YSections::Bottom;

			source.y = _source.y + _source.h - sourceFrame.bottom;
			source.h = sourceFrame.bottom;
			dest.y = _dest.y + _dest.h - destFrame.bottom;
			dest.h = destFrame.bottom;
		}

		// Make coordinates relative area.

		ofs.x -= dest.x;
		ofs.y -= dest.y;


		// Convert offset in area to offset in bitmap.
/*
		TJFIX!!!

RectI sourceOfs;

		if (markedSectionX == XSections::Center && (patch.rigidPartXSections & markedSectionY) != YSections::None)
		{
			int leftSrcLen = patch.rigidPartXOfs;
			int rightSrcLen = source.w - patch.rigidPartXOfs - patch.rigidPartXLength;

			int totalSrcLen = leftSrcLen + rightSrcLen;

			MU midDstLen = std::min(MU(patch.rigidPartXLength), dest.w);

			MU leftDstLen = (dest.w - midDstLen) * leftSrcLen / totalSrcLen;
			MU rightDstLen = dest.w - midDstLen - leftDstLen;

			if (ofs.x < leftDstLen)
				sourceOfs.x = int(ofs.x.qpix / double(leftDstLen.qpix) * leftSrcLen);
			else if (ofs.x < leftDstLen + midDstLen)
				sourceOfs.x = int(ofs.x - leftDstLen) + leftSrcLen;
			else
				sourceOfs.x = int((ofs.x.qpix - leftDstLen.qpix - midDstLen.qpix) / double(rightDstLen.qpix) * rightSrcLen) + leftSrcLen + patch.rigidPartXLength;
		}
		else
			sourceOfs.x = (int)(ofs.x.qpix / ((double)dest.w.qpix) * source.w);
	
		
		if (markedSectionY == YSections::Center && (patch.rigidPartYSections & markedSectionX) != XSections::None)
		{
			int topSrcLen = patch.rigidPartYOfs;
			int bottomSrcLen = source.h - patch.rigidPartYOfs - patch.rigidPartYLength;

			int totalSrcLen = topSrcLen + bottomSrcLen;

			MU midDstLen = std::min(MU(patch.rigidPartYLength), dest.h);

			MU topDstLen = (dest.h - midDstLen) * topSrcLen / totalSrcLen;
			MU bottomDstLen = dest.h - midDstLen - topDstLen;

			if (ofs.y < topDstLen)
				sourceOfs.y = int( ofs.y.qpix / double(topDstLen.qpix) * topSrcLen);
			else if (ofs.y < topDstLen + midDstLen)
				sourceOfs.y = int(ofs.y - topDstLen) + topSrcLen;
			else
				sourceOfs.y = int((ofs.y.qpix - topDstLen.qpix - midDstLen.qpix) / double(bottomDstLen.qpix) * bottomSrcLen) + topSrcLen + patch.rigidPartYLength;
		}
		else
			sourceOfs.y = (int)(ofs.y.qpix / ((double)dest.h.qpix) * source.h);

		// Do alpha test

		int alpha = pSurface->alpha(source.x + sourceOfs.x, source.y + sourceOfs.y);

		return (alpha >= opacityTreshold);
*/
		return true;
	}




	//____ sizeFromConstraint() __________________________________________________________

	spx Util::sizeFromConstraint( spx defaultSize, spx specifiedSize, SizeConstraint policy )
	{
		switch( policy )
		{
			case SizeConstraint::None:
				return defaultSize;
			case SizeConstraint::Equal:
				return specifiedSize;
			case SizeConstraint::LessOrEqual:
				if( defaultSize > specifiedSize )
					return specifiedSize;
			case SizeConstraint::GreaterOrEqual:
				if( defaultSize < specifiedSize )
					return specifiedSize;
		}
		return defaultSize;
	}

	//____ bestStateIndexMatch() ___________________________________________________

	int Util::bestStateIndexMatch(int wantedStateIndex, Bitmask<uint32_t> availableStateIndices)
	{
		static uint32_t mask[State::IndexAmount] = {	1,
											2+1,
											4+1,
											8 + 4 + 2 + 1,
											16 + 4 + 1,
											32 + 16 + 8 + 4 + 2 + 1,
											64 + 1,
											128 + 64 + 2 + 1,
											256 + 64 + 4 + 1,
											512 + 256 + 128 + 64 + 8 + 4 + 2 + 1,
											1024 + 256 + 64 + 16 + 4 + 1,
											2048 + 1024 + 512 + 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1,
											4096 + 4 + 1,
											8192 + 4096 + 8 + 4 + 2 + 1,
											16384 + 4096 + 256 + 64 + 4 + 1,
											32768 + 16384 + 8192 + 4096 + 512 + 256 + 128 + 64 + 8 + 4 + 2 + 1,
											65536 + 1,
											65536*2 + 65536 + 64 + 1
										};

		int candidates = uint32_t(availableStateIndices) & mask[wantedStateIndex];

		return mostSignificantBit(candidates);
	}

	//____ Checksum8::add() ________________________________________________________

	void Util::Checksum8::add( const void * pData, uint32_t nBytes )
	{
		uint32_t x = remainder;

		for( uint32_t i = 0 ; i < nBytes ; i++ )
			x = ((x << 8) + ((uint8_t*)pData)[i])%dividend;

		remainder = x;
	}


	//____ placementToOfs() ________________________________________________________

	CoordSPX Util::placementToOfs( Placement placement, SizeSPX base )
	{
		switch( placement )
		{
			default:
			case Placement::NorthWest:
				return CoordSPX();

			case Placement::North:
				return CoordSPX( align(base.w/2),0 );

			case Placement::NorthEast:
				return CoordSPX( base.w,0 );

			case Placement::East:
				return CoordSPX( base.w, align(base.h/2) );

			case Placement::SouthEast:
				return CoordSPX( base.w, base.h );

			case Placement::South:
				return CoordSPX( align(base.w/2), base.h );

			case Placement::SouthWest:
				return CoordSPX( 0, base.h );

			case Placement::West:
				return CoordSPX( 0, align(base.h/2) );

			case Placement::Center:
				return CoordSPX( align(base.w/2), align(base.h/2) );
		}
	}

	//____ placementToRect() ________________________________________________________

	RectSPX Util::placementToRect( Placement placement, SizeSPX base, SizeSPX rect )
	{
		switch( placement )
		{
			default:
			case Placement::NorthWest:
				return RectSPX(0,0, rect);

			case Placement::North:
				return RectSPX( align(base.w/2 - rect.w/2), 0, rect );

			case Placement::NorthEast:
				return RectSPX( base.w - rect.w, 0, rect );

			case Placement::East:
				return RectSPX( base.w - rect.w, align(base.h/2 - rect.h/2), rect );

			case Placement::SouthEast:
				return RectSPX( base.w - rect.w, base.h - rect.h, rect );

			case Placement::South:
				return RectSPX( align(base.w/2 - rect.w/2), base.h - rect.h, rect );

			case Placement::SouthWest:
				return RectSPX( 0, base.h - rect.h, rect );

			case Placement::West:
				return RectSPX( 0, align(base.h/2 - rect.h/2), rect );

			case Placement::Center:
				return RectSPX( align(base.w/2 - rect.w/2), align(base.h/2 - rect.h/2), rect );
		}
	}

	//____ scaleToFit() _______________________________________________________

	SizeSPX Util::scaleToFit(SizeSPX object, SizeSPX boundaries)
	{
		float wScale = object.w / (float)boundaries.w;
		float hScale = object.h / (float)boundaries.h;

		float useScale = std::max(wScale, hScale);

		return object / useScale;
	}

	//____ patchesToClipList() ____________________________________________________________________

	Util::ClipPopData Util::patchesToClipList( GfxDevice * pDevice, const RectSPX& clip, const PatchesSPX& patches )
	{
		int nOldRects 				= pDevice->clipListSize();
		const RectSPX * pOldRects 	= pDevice->clipList();

		int nRects = patches.size();
		const RectSPX * pRects = patches.begin();

		int allocSize = nRects * sizeof(RectSPX);

		RectSPX * pNewRects = (RectSPX*) Base::memStackAlloc(allocSize);
		int nNewRects = 0;

		for( int i = 0 ; i < nRects ; i++ )
		{
			pNewRects[nNewRects] = RectSPX::overlap(pRects[i], clip);
			if( !pNewRects[nNewRects].isEmpty() )
				nNewRects++;
		}

		pDevice->setClipList(nNewRects, pNewRects);
		return { nOldRects, pOldRects, allocSize };
	}

	Util::ClipPopData Util::patchesToClipList( GfxDevice * pDevice, const PatchesSPX& patches )
	{
		int nOldRects 				= pDevice->clipListSize();
		const RectI * pOldRects 	= pDevice->clipList();

		int nRects = patches.size();
		const RectSPX * pRects = patches.begin();

		int allocSize = nRects * sizeof(RectSPX);

		RectSPX * pNewRects = (RectSPX*) Base::memStackAlloc(allocSize);

		for( int i = 0 ; i < nRects ; i++ )
			pNewRects[i] = pRects[i];

		pDevice->setClipList(nRects, pNewRects);
		return { nOldRects, pOldRects, allocSize };
	}

	//____ limitClipList() ____________________________________________________________________

	Util::ClipPopData Util::limitClipList( GfxDevice * pDevice, const RectSPX& _clip )
	{
		RectSPX clip = _clip;

		if( clip.contains(pDevice->clipBounds()))
			return { 0, nullptr, 0 };

		int nRects 				= pDevice->clipListSize();
		const RectSPX * pRects 	= pDevice->clipList();
		int allocSize = nRects * sizeof(RectSPX);

		RectSPX * pNewRects = (RectSPX*) Base::memStackAlloc(allocSize);
		int nNewRects = 0;

		for( int i = 0 ; i < nRects ; i++ )
		{
			if (clip.isOverlapping(pRects[i]))
				pNewRects[nNewRects++] = RectSPX::overlap(pRects[i], clip);
		}

		pDevice->setClipList(nNewRects, pNewRects);
		return { nRects, pRects, allocSize };
	}

	//____ pushClipList() __________________________________________________________________

	Util::ClipPopData Util::pushClipList(GfxDevice * pDevice)
	{
		return { pDevice->clipListSize(), pDevice->clipList(), 0 };
	}


	//____ popClipList() ____________________________________________________________________

	void Util::popClipList( GfxDevice * pDevice, const ClipPopData& popData )
	{
		if( popData.bInitialized )
		{
			pDevice->setClipList(popData.nRects, popData.pRects);
			Base::memStackRelease(popData.reservedMem);
		}
	}

} // namespace wg
