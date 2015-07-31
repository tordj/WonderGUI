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

#ifndef	WG_TEXTEDITOR_DOT_H
#define	WG_TEXTEDITOR_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_EDITTEXT_DOT_H
#	include <wg_edittext.h>
#endif

#ifndef WG_CARETINSTANCE_DOT_H
#	include <wg_caretinstance.h>
#endif

namespace wg 
{
	
	class TextEditor;
	typedef	WgStrongPtr<TextEditor,Widget_p>		TextEditor_p;
	typedef	WgWeakPtr<TextEditor,Widget_wp>	TextEditor_wp;
	
	class TextEditor:public Widget, protected LegacyTextHolder
	{
	public:
		static TextEditor_p	create() { return TextEditor_p(new TextEditor()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static TextEditor_p	cast( const Object_p& pObject );
	
		//____ Interfaces ______________________________________
	
		EditText		text;
	
		//____ Methods __________________________________________
	
		inline void		setMaxLines( int nLines ) { m_maxLines = nLines; }
		inline int		maxLines() { return m_maxLines; }
	
		int		insertTextAtCursor( const CharSeq& str );
		bool	insertCharAtCursor( Uint16 c );
	
		virtual void			setEditMode(TextEditMode mode);
		virtual TextEditMode	editMode() const { return m_text.editMode(); }
	
		WgPointerStyle		pointerStyle() const;
		String			tooltipString() const;
	
		int		matchingHeight( int width ) const;
		Size	preferredSize() const;
		bool	isAutoEllipsisDefault() const { return true; };
	
		bool	isEditable() const { return m_text.isEditable(); }
		bool	isSelectable() const { return m_text.isSelectable(); }
	
	protected:
		TextEditor();
		virtual ~TextEditor();
		virtual Widget* _newOfMyType() const { return new TextEditor(); };
	
		void	_onCloneContent( const Widget * _pOrg );
		void	_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void	_onNewSize( const Size& size );
		void	_onRefresh();
		void	_onMsg( const Msg_p& pMsg );
		void	_onStateChanged( State oldState );
		void	_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
	
		Object * _object() { return this; }
		void	_onFieldDirty( Field * pField );
		void	_onFieldResize( Field * pField );
	private:
	
		bool	_insertCharAtCursor( Uint16 c );
	
	
		LegacyTextField			m_text;
		bool				m_bHasFocus;
		int					m_maxLines;
		bool				m_bResetCursorOnFocus;
		WgRouteId			m_tickRouteId;
	};
	
	
	
	
	

} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
