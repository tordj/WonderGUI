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

//____ SetText() ______________________________________________________________

void Wg_Interface_TextHolder::SetText( const char * _pStr )
{
	m_pText->setText( _pStr );
	TextModified();
}

void Wg_Interface_TextHolder::SetText( const Uint16 * _pStr )
{
	m_pText->setText( _pStr );
	TextModified();
}

void Wg_Interface_TextHolder::SetText( const WgChar * _pStr )
{
	m_pText->setText( _pStr );
	TextModified();
}

void Wg_Interface_TextHolder::SetText( const WgText * _pText )
{
	m_pText->setText( _pText );
	TextModified();
}


//____ AddText() ______________________________________________________________
/*
void Wg_Interface_TextHolder::AddText( const char * pszText )
{
	m_pText->addText( pszText );
	TextModified();
}
*/
void Wg_Interface_TextHolder::AddText( const WgText * pText, Uint32 fromLine, Uint32 nLines )
{
	m_pText->addText( pText, fromLine, nLines );
	TextModified();
}

void Wg_Interface_TextHolder::AddText( const WgCharBuffer* charBuffer )
{
	m_pText->addText(charBuffer);
	TextModified();
}


//____ SetFormattedText() _____________________________________________________

Uint32 Wg_Interface_TextHolder::SetFormattedText( const char * pText, Uint32 nChar, const WgResDB * pResDB )
{
	Uint32 ret = m_pText->setFormattedText( pText, nChar, pResDB );
	TextModified();
	return ret;
}

Uint32 Wg_Interface_TextHolder::SetFormattedText( const Uint16 * pText, Uint32 nChar, const WgResDB * pResDB )
{
	Uint32 ret = m_pText->setFormattedText( pText, nChar, pResDB );
	TextModified();
	return ret;
}


//____ AddFormattedText() _____________________________________________________

Uint32 Wg_Interface_TextHolder::AddFormattedText( const char * pText, Uint32 nChar, const WgResDB * pResDB )
{
	Uint32 nb = m_pText->addFormattedText( pText, nChar, pResDB );
	TextModified();
	return nb;
}

//____ InsertFormattedText() __________________________________________________

Uint32 Wg_Interface_TextHolder::InsertFormattedText( Uint32 line, Uint32 col, const char * pText, Uint32 nChar, const WgResDB * pResDB )
{
	Uint32 nb = m_pText->insertFormattedText( line, col, pText, nChar, pResDB );
	TextModified();
	return nb;
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


//____ RemoveTextLines() ______________________________________________________________

void Wg_Interface_TextHolder::RemoveTextLines( Uint32 start, Uint32 nLines )
{
	m_pText->removeLines( start, nLines );
	TextModified();
}

void Wg_Interface_TextHolder::RemoveSelectedText()
{
	m_pText->removeSelectedText();
	TextModified();
}

//____ CloneInterface() _______________________________________________________

//DEPRECATED! To be removed once all widgets and items are gizmos.

void Wg_Interface_TextHolder::CloneInterface( Wg_Interface_TextHolder * _pClone )
{
}

//____ OnCloneContent() _______________________________________________________

void Wg_Interface_TextHolder::OnCloneContent( const Wg_Interface_TextHolder * pOrg )
{
}
