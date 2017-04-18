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
	
	//____ cast() _________________________________________________________________
	
	Panel_p Panel::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Panel_p( static_cast<Panel*>(pObject.rawPtr()) );
	
		return 0;
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
	
	void Panel::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace )
		{
			patches.sub( Rect(geo,clip) );
			return;
		}
		
		switch( m_maskOp )
		{
			case MaskOp::Recurse:
			{
				SlotWithGeo child;
				_firstSlotWithGeo( child );
	
				while(child.pSlot)
				{
					child.pSlot->pWidget->_maskPatches( patches, child.geo + geo.pos(), clip, blendMode );
					_nextSlotWithGeo( child );
				}
				break;
			}
			case MaskOp::Skip:
				break;
			case MaskOp::Mask:
				patches.sub( Rect(geo,clip) );
				break;
		}
	}


} // namespace wg
