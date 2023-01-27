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

#ifndef    WG2_TEXTDISPLAY_DOT_H
#define    WG2_TEXTDISPLAY_DOT_H


#ifndef WG2_WIDGET_DOT_H
#    include <wg2_widget.h>
#endif

#ifndef    WG2_INTERFACE_EDITTEXT_DOT_H
#    include <wg2_interface_edittext.h>
#endif

#ifndef    WG2_TEXT_DOT_H
#    include <wg2_text.h>
#endif

#ifndef WG2_CURSORINSTANCE_DOT_H
#    include <wg2_cursorinstance.h>
#endif


class WgTextDisplay:public WgWidget, public WgInterfaceEditText
{
public:
    WgTextDisplay();
    virtual ~WgTextDisplay();
    virtual const char * Type() const;
    static const char * GetClass();
    virtual WgWidget * NewOfMyType() const { return new WgTextDisplay(); };



    //____ Methods __________________________________________

    inline void       SetMaxLines( int nLines ) { m_maxLines = nLines; }
    inline int        MaxLines() { return m_maxLines; }

    Uint32    InsertTextAtCursor( const wg::CharSeq& str );
    bool      InsertCharAtCursor( Uint16 c );

    void        GoBOL();
    void        GoEOL();
    void        GoBOF();
    void        GoEOF();

    virtual void            SetEditMode(WgTextEditMode mode);
    virtual WgTextEditMode  GetEditMode() const { return m_text.GetEditMode(); }

    WgPointerStyle        GetPointerStyle() const;
//    wg::String            GetTooltipString() const;

    virtual bool TempIsInputField() const { return IsEditable(); }
    virtual Wg_Interface_TextHolder* TempGetText() { return this; }

    int     MatchingPixelHeight( int width ) const;
    WgSize  PreferredPixelSize() const;
    bool    IsAutoEllipsisDefault() const { return true; };

    bool    IsEditable() const { return m_text.IsEditable(); }
    bool    IsSelectable() const { return m_text.IsSelectable(); }

    void    SelectAll() { m_text.selectAll(); }

    void    ChangeText(const wg::CharSeq& kStr);
    void    ChangeText(std::string kStr);
    void    ChangeText(const char* kStr);

protected:

    void    _onCloneContent( const WgWidget * _pOrg );
    void    _onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window );
    void    _onNewSize( const WgSize& size );
    void    _onRefresh();
    void    _onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
    bool    _onAlphaTest( const WgCoord& ofs );
    void    _setState(WgState state);
    void    _onEnable();
    void    _onDisable();
    void    _onGotInputFocus();
    void    _onLostInputFocus();
    void    _setScale( int scale );

    void    _bringCursorInView();


    WgText                m_text;
    bool                  m_bHasFocus;
    int                   m_maxLines;
    bool                  m_bResetCursorOnFocus;
private:
    void    _textModified();
    bool    _insertCharAtCursor( Uint16 c );

    wg::String            m_kStr;
};

#endif // WG2_TEXTDISPLAY_DOT_H
