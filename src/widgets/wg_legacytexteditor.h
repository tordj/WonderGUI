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

#ifndef	WG_LEGACYTEXTEDITOR_DOT_H
#define	WG_LEGACYTEXTEDITOR_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_LEGACYEDITTEXT_DOT_H
#	include <wg_legacyedittext.h>
#endif

#ifndef WG_CARETINSTANCE_DOT_H
#	include <wg_caretinstance.h>
#endif

namespace wg 
{
	
	class LegacyTextEditor;
	typedef	StrongPtr<LegacyTextEditor,Widget_p>		LegacyTextEditor_p;
	typedef	WeakPtr<LegacyTextEditor,Widget_wp>	LegacyTextEditor_wp;
	
	class LegacyTextEditor:public Widget, protected LegacyTextHolder
	{
	public:
		static LegacyTextEditor_p	create() { return LegacyTextEditor_p(new LegacyTextEditor()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static LegacyTextEditor_p	cast( const Object_p& pObject );
	
		//____ Interfaces ______________________________________
	
		LegacyEditText		text;
	
		//____ Methods __________________________________________
	
		inline void		setMaxLines( int nLines ) { m_maxLines = nLines; }
		inline int		maxLines() { return m_maxLines; }
	
		int		insertTextAtCursor( const CharSeq& str );
		bool	insertCharAtCursor( uint16_t c );
	
		virtual void			setEditMode(TextEditMode mode);
		virtual TextEditMode	editMode() const { return m_text.editMode(); }
	
		PointerStyle		pointerStyle() const;
		String			tooltipString() const;
	
		int		matchingHeight( int width ) const;
		Size	preferredSize() const;
		bool	isAutoEllipsisDefault() const { return true; };
	
		bool	isEditable() const { return m_text.isEditable(); }
		bool	isSelectable() const { return m_text.isSelectable(); }
	
	protected:
		LegacyTextEditor();
		virtual ~LegacyTextEditor();
		virtual Widget* _newOfMyType() const { return new LegacyTextEditor(); };
	
		void	_onCloneContent( const Widget * _pOrg );
		void	_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void	_onNewSize( const Size& size );
		void	_onRefresh();
		void	_onMsg( const Msg_p& pMsg );
		void	_onStateChanged( State oldState );
		void	_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
	
		Object * _object() { return this; }
		void	_onFieldDirty( Field * pField );
		void	_onFieldDirty( Field * pField, const Rect& rect );
		void	_onFieldResize( Field * pField );
	private:
	
		bool	_insertCharAtCursor( uint16_t c );
	
	
		LegacyTextField			m_text;
		bool				m_bHasFocus;
		int					m_maxLines;
		bool				m_bResetCursorOnFocus;
		RouteId			m_tickRouteId;
	};
	
	
	
	
	

} // namespace wg
#endif // WG_LEGACYTEXTEDITOR_DOT_H
