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
#include <wg_interface_textholder.h>


//____ Constructor ____________________________________________________________

Wg_Interface_TextHolder::Wg_Interface_TextHolder()
{
	m_pText		= 0;
}


//____ SetTextMode() __________________________________________________________

void Wg_Interface_TextHolder::SetTextMode( WgMode _mode )
{
	if( _mode != m_pText->mode() )
	{
		m_pText->setMode(_mode);
		TextModified();
	}
}

//____ SetTextWrap() __________________________________________________________
bool Wg_Interface_TextHolder::SetTextWrap( bool bWrap )
{
	if( bWrap != m_pText->IsWrap() )
	{
		m_pText->SetWrap(bWrap);
		TextModified();
	}
	return true;
}

//____ SetTextAlignment() _____________________________________________________

void Wg_Interface_TextHolder::SetTextAlignment( const WgOrigo& origo )
{
	if( origo != m_pText->alignment() )
	{
		m_pText->setAlignment(origo);
		TextModified();
	}
}

//____ SetTextTintMode() _____________________________________________________

void Wg_Interface_TextHolder::SetTextTintMode( WgTintMode mode )
{
	if( mode != m_pText->tintMode() )
	{
		m_pText->setTintMode(mode);
		TextModified();
	}
}


//____ Wg_Interface_TextHolder::ClearText() ___________________________________

void Wg_Interface_TextHolder::ClearText()
{
	if( m_pText )
	{
		m_pText->clear();
		TextModified();
	}
}


void Wg_Interface_TextHolder::SetText( const WgCharSeq& seq )
{
	m_pText->setText( seq );
	TextModified();
}

void Wg_Interface_TextHolder::SetText( const WgCharBuffer * pBuffer )
{
	m_pText->setText( pBuffer );
	TextModified();
}

void Wg_Interface_TextHolder::SetText( const WgString& str )
{
	m_pText->setText( str );
	TextModified();
}

void Wg_Interface_TextHolder::SetText( const WgText * pText )
{
	m_pText->setText( pText );
	TextModified();
}

int Wg_Interface_TextHolder::AddText( const WgCharSeq& seq )
{
	int nAdded = m_pText->addText( seq );
	TextModified();
	return nAdded;
}

int Wg_Interface_TextHolder::InsertText( int ofs, const WgCharSeq& seq )
{
	int nAdded = m_pText->insertText( ofs, seq );
	TextModified();
	return nAdded;
}

int Wg_Interface_TextHolder::ReplaceText( int ofs, int nDelete, const WgCharSeq& seq )
{
	int nAdded = m_pText->replaceText( ofs, nDelete, seq );
	TextModified();
	return nAdded;
}

int Wg_Interface_TextHolder::DeleteText( int ofs, int len )
{
	int nDeleted = m_pText->deleteText( ofs, len );
	TextModified();
	return nDeleted;
}

void Wg_Interface_TextHolder::DeleteSelectedText()
{
	m_pText->deleteSelectedText();
	TextModified();
}

int Wg_Interface_TextHolder::AddChar( const WgChar& character )
{
	int nAdded = m_pText->addChar( character );
	TextModified();
	return nAdded;
}

int Wg_Interface_TextHolder::InsertChar( int ofs, const WgChar& character )
{
	int nAdded = m_pText->insertChar( ofs, character );
	TextModified();
	return nAdded;
}

int Wg_Interface_TextHolder::ReplaceChar( int ofs, const WgChar& character )
{
	int nAdded = m_pText->replaceChar( ofs, character );
	TextModified();
	return nAdded;
}

int Wg_Interface_TextHolder::DeleteChar( int ofs )
{
	int nDeleted = m_pText->deleteChar( ofs );
	TextModified();
	return nDeleted;
}



//____ SetLineSpaceAdjustment() __________________________________________________

void Wg_Interface_TextHolder::SetLineSpaceAdjustment( Sint8 adjustment )
{
	m_pText->setLineSpaceAdjustment( adjustment );
	TextModified();
}

//____ GetLineSpaceAdjustment() __________________________________________________

Sint8 Wg_Interface_TextHolder::GetLineSpaceAdjustment() const
{
	return m_pText->lineSpaceAdjustment();
}


//____ CloneInterface() _______________________________________________________

//DEPRECATED! To be removed once all widgets and items are gizmos.

void Wg_Interface_TextHolder::CloneInterface( Wg_Interface_TextHolder * _pClone )
{
}

//____ _onCloneContent() _______________________________________________________

void Wg_Interface_TextHolder::_onCloneContent( const Wg_Interface_TextHolder * pOrg )
{
}
