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


//____ Constructor _____________________________________________________________

WgEditTextField::WgEditTextField( WgEditTextHolder * pHolder ) : WgTextField(pHolder)
{
	m_editState.caretOfs = 0;
	m_editState.selectOfs = 0;
	m_editState.wantedOfs = -1;
	m_editMode = WG_TEXT_EDITABLE;
}


//____ Clear() _________________________________________________________________

void WgEditTextField::Clear()
{
	m_editState.caretOfs = 0;
	m_editState.selectOfs = 0;
	m_editState.wantedOfs = -1;

	WgTextField::Clear();
}

//____ Set() ___________________________________________________________________

void WgEditTextField::Set( const WgCharSeq& seq )
{
	WgTextField::Set( seq );
	_cursorToEnd();
}

void WgEditTextField::Set( const WgCharBuffer * pBuffer )
{
	WgTextField::Set( pBuffer );
	_cursorToEnd();
}

void WgEditTextField::Set( const WgString& str )
{
	WgTextField::Set( str );
	_cursorToEnd();
}

//____ Append() ________________________________________________________________

int WgEditTextField::Append( const WgCharSeq& seq )
{
	m_editState.selectOfs = m_editState.caretOfs;
	m_editState.wantedOfs = -1;
			
	return WgTextField::Append( seq );
}

//____ Insert() ________________________________________________________________

int WgEditTextField::Insert( int ofs, const WgCharSeq& seq )
{
	if( ofs < m_editState.caretOfs )
		m_editState.caretOfs += seq.Length();
	m_editState.selectOfs = m_editState.caretOfs;
	m_editState.wantedOfs = -1;

	return WgTextField::Insert(ofs,seq);
}

//____ Replace() _______________________________________________________________

int WgEditTextField::Replace( int ofs, int nDelete, const WgCharSeq& seq )
{
	int caretOfs = m_editState.caretOfs;
	if( caretOfs > ofs )
	{
		if( caretOfs < ofs + nDelete )
			caretOfs = ofs;
		else
			caretOfs = caretOfs - nDelete + seq.Length();
	}
	m_editState.caretOfs = caretOfs;
	m_editState.selectOfs = caretOfs;
	m_editState.wantedOfs = -1;

	return WgTextField::Replace(ofs,nDelete,seq);
}

//____ Delete() ________________________________________________________________

int WgEditTextField::Delete( int ofs, int len )
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
	
	return WgTextField::Delete( ofs, len );
}

//____ SetState() ______________________________________________________________

void WgEditTextField::SetState( WgState state )
{
	WgTextField::SetState(state);

	if( state.IsFocused() && m_editMode == WG_TEXT_EDITABLE )
		m_editState.bCaret = true;
	else
		m_editState.bCaret = false;
}


//____ OnRender() ______________________________________________________________

void WgEditTextField::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip )
{
	_presenter()->RenderField(this, pDevice, &m_editState, _canvas, _clip);
}

//____ _cursorToEnd() __________________________________________________________

void WgEditTextField::_cursorToEnd()
{
	int end = m_charBuffer.Length();
	m_editState.caretOfs = end;
	m_editState.selectOfs = end;
	m_editState.wantedOfs = -1;	
}
