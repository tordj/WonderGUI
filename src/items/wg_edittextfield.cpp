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
	
	EditTextField::EditTextField( EditTextHolder * pHolder ) : TextField(pHolder)
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;
		m_editMode = TextEditMode::Editable;
	}
	
	
	//____ clear() _________________________________________________________________
	
	void EditTextField::clear()
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;
	
		TextField::clear();
	}
	
	//____ set() ___________________________________________________________________
	
	void EditTextField::set( const CharSeq& seq )
	{
		TextField::set( seq );
		_caretToEnd();
	}
	
	void EditTextField::set( const CharBuffer * pBuffer )
	{
		TextField::set( pBuffer );
		_caretToEnd();
	}
	
	void EditTextField::set( const String& str )
	{
		TextField::set( str );
		_caretToEnd();
	}
	
	//____ append() ________________________________________________________________
	
	int EditTextField::append( const CharSeq& seq )
	{
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
				
		return TextField::append( seq );
	}
	
	//____ insert() ________________________________________________________________
	
	int EditTextField::insert( int ofs, const CharSeq& seq )
	{
		if( ofs < m_editState.caretOfs )
			m_editState.caretOfs += seq.length();
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
	
		return TextField::insert(ofs,seq);
	}
	
	//____ replace() _______________________________________________________________
	
	int EditTextField::replace( int ofs, int nDelete, const CharSeq& seq )
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
	
		return TextField::replace(ofs,nDelete,seq);
	}
	
	//____ erase() ________________________________________________________________
	
	int EditTextField::erase( int ofs, int len )
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
		
		return TextField::remove( ofs, len );
	}
	
	//____ setState() ______________________________________________________________
	
	void EditTextField::setState( State state )
	{
		TextField::setState(state);
	
		if( state.isFocused() && m_editMode == TextEditMode::Editable )
			m_editState.bCaret = true;
		else
			m_editState.bCaret = false;
	}
	
	
	//____ onRender() ______________________________________________________________
	
	void EditTextField::onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip )
	{
		_presenter()->renderField(this, pDevice, _canvas, _clip);
	}
	
	//____ _caretToEnd() __________________________________________________________
	
	void EditTextField::_caretToEnd()
	{
		int end = m_charBuffer.length();
		m_editState.caretOfs = end;
		m_editState.selectOfs = end;
		m_editState.wantedOfs = -1;	
	}

} // namespace wg
