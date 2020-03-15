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

#include <vector>
#include <wg_panel.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_gfxdevice.h>
#include <wg_slot.h>
#include <wg_internal.h>

namespace wg
{
	const char Panel::CLASSNAME[] = {"Panel"};

	//____ Constructor _____________________________________________________________

	Panel::Panel() : m_bTooltipGroup(false), m_maskOp(MaskOp::Recurse)
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool Panel::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Container::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * Panel::className( void ) const
	{
		return CLASSNAME;
	}

	//____ _isPanel() ______________________________________________________________

	bool Panel::_isPanel() const
	{
		return true;
	}

	//____ setMaskOp() _____________________________________________________________

	void Panel::setMaskOp( MaskOp operation )
	{
		if( operation != m_maskOp )
		{
			m_maskOp = operation;
			_requestRender();
		}
	}

	//____ _cloneContent() _______________________________________________________

	void Panel::_cloneContent( const Widget * _pOrg )
	{
		const Panel * pOrg = static_cast<const Panel*>(_pOrg);

		m_bTooltipGroup 	= pOrg->m_bTooltipGroup;
		m_maskOp 			= pOrg->m_maskOp;

		Container::_cloneContent( pOrg );
	}


	//____ _maskPatches() __________________________________________________________

	void Panel::_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		//TODO: Should m_maskOp be moved to Container instead? Could remove many versions of _maskPatches()...
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace )
		{
			patches.sub( RectI(geo,clip) );
			return;
		}

		switch( m_maskOp )
		{
			case MaskOp::Recurse:
			{
				RectI	myClip(geo, clip);				// Need to limit clip to our geo. Otherwise children outside might mask what they shouldn't (for panels where children can go outside parent).
				SlotWithGeo child;
				_firstSlotWithGeo( child );

				while(child.pSlot)
				{
					OO(child.pSlot->_widget())->_maskPatches( patches, child.geo + geo.pos(), myClip, blendMode );
					_nextSlotWithGeo( child );
				}
				break;
			}
			case MaskOp::Skip:
				break;
			case MaskOp::Mask:
				patches.sub( RectI(geo,clip) );
				break;
		}
	}


} // namespace wg
