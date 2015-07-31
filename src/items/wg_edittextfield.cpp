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

#include <wg_edittextfield.h>
#include <wg_charseq.h>

namespace wg 
{
	
	
	//____ Constructor _____________________________________________________________
	
	WgEditTextField::WgEditTextField( WgEditTextHolder * pHolder ) : WgTextField(pHolder)
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;
		m_editMode = WG_TEXT_EDITABLE;
	}
	
	
	//____ clear() _________________________________________________________________
	
	void WgEditTextField::clear()
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;
	
		WgTextField::clear();
	}
	
	//____ set() ___________________________________________________________________
	
	void WgEditTextField::set( const WgCharSeq& seq )
	{
		WgTextField::set( seq );
		_cursorToEnd();
	}
	
	void WgEditTextField::set( const WgCharBuffer * pBuffer )
	{
		WgTextField::set( pBuffer );
		_cursorToEnd();
	}
	
	void WgEditTextField::set( const WgString& str )
	{
		WgTextField::set( str );
		_cursorToEnd();
	}
	
	//____ append() ________________________________________________________________
	
	int WgEditTextField::append( const WgCharSeq& seq )
	{
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
				
		return WgTextField::append( seq );
	}
	
	//____ insert() ________________________________________________________________
	
	int WgEditTextField::insert( int ofs, const WgCharSeq& seq )
	{
		if( ofs < m_editState.caretOfs )
			m_editState.caretOfs += seq.length();
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
	
		return WgTextField::insert(ofs,seq);
	}
	
	//____ replace() _______________________________________________________________
	
	int WgEditTextField::replace( int ofs, int nDelete, const WgCharSeq& seq )
	{
		int caretOfs = m_editState.caretOfs;
		if( caretOfs > ofs )
		{
			if( caretOfs < ofs + nDelete )
				caretOfs = ofs;
			else
				caretOfs = caretOfs - nDelete + seq.length();
		}
		m_editState.caretOfs = caretOfs;
		m_editState.selectOfs = caretOfs;
		m_editState.wantedOfs = -1;
	
		return WgTextField::replace(ofs,nDelete,seq);
	}
	
	//____ delete() ________________________________________________________________
	
	int WgEditTextField::remove( int ofs, int len )
	{
		int caretOfs = m_editState.caretOfs;
		if( caretOfs > ofs )
		{
			if( caretOfs < ofs + len )
				caretOfs = ofs;
			else
				caretOfs -= len;
		}
		m_editState.caretOfs = caretOfs;
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
		
		return WgTextField::remove( ofs, len );
	}
	
	//____ setState() ______________________________________________________________
	
	void WgEditTextField::setState( WgState state )
	{
		WgTextField::setState(state);
	
		if( state.isFocused() && m_editMode == WG_TEXT_EDITABLE )
			m_editState.bCaret = true;
		else
			m_editState.bCaret = false;
	}
	
	
	//____ onRender() ______________________________________________________________
	
	void WgEditTextField::onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip )
	{
		_presenter()->renderField(this, pDevice, _canvas, _clip);
	}
	
	//____ _cursorToEnd() __________________________________________________________
	
	void WgEditTextField::_cursorToEnd()
	{
		int end = m_charBuffer.length();
		m_editState.caretOfs = end;
		m_editState.selectOfs = end;
		m_editState.wantedOfs = -1;	
	}

} // namespace wg
