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
#ifndef WG_LINEEDITOR_DOT_H
#define WG_LINEEDITOR_DOT_H


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
	
	class WgLineEditor;
	typedef	WgStrongPtr<WgLineEditor,WgWidget_p>		WgLineEditor_p;
	typedef	WgWeakPtr<WgLineEditor,WgWidget_wp>	WgLineEditor_wp;
	
	//____ WgLineEditor ____________________________________________________________
	
	class WgLineEditor : public WgWidget, protected WgLegacyTextHolder
	{
	public:
		static WgLineEditor_p	create() { return WgLineEditor_p(new WgLineEditor()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgLineEditor_p	cast( const WgObject_p& pObject );
	
		//____ Interfaces ______________________________________
	
		WgEditText		text;
	
		//____ Methods __________________________________________
	
		inline void	SetPasswordMode( bool on_off ) { m_bPasswordMode = on_off; };
		inline bool PasswordMode() {return m_bPasswordMode;};
		inline Uint16 PasswordGlyph() const				 { return m_pwGlyph; };
		void		SetPasswordGlyph( Uint16 glyph );
	
		int			InsertTextAtCursor( const WgCharSeq& str );
		bool		InsertCharAtCursor( Uint16 c );
	
		virtual void			setEditMode(WgTextEditMode mode);
		virtual WgTextEditMode	editMode() const { return m_text.editMode(); }
	
		WgSize		preferredSize() const;
		bool		IsAutoEllipsisDefault() const { return false; };
	
	
	protected:
		WgLineEditor();
		virtual ~WgLineEditor();
		virtual WgWidget* _newOfMyType() const { return new WgLineEditor(); };
	
		bool	_isEditable() const { return m_text.isEditable(); }
		bool	_isSelectable() const { return m_text.isSelectable(); }
	
		void	_onMsg( const WgMsg_p& pMsg );
		void	_onStateChanged( WgState oldState );
		void	_onCloneContent( const WgWidget * _pOrg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void	_onNewSize( const WgSize& size );
		void	_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin );
	
		WgObject * 		_object() { return this; };
		void			_onFieldDirty( WgField * pField );
		void 			_onFieldResize( WgField * pField );
	
	private:
	
		void	_adjustViewOfs();
	
		WgLegacyTextField			m_text;
		WgRouteId			m_tickRouteId;
	
		bool				m_bResetCursorOnFocus;
		bool				m_bPasswordMode;
		Uint16				m_pwGlyph;
		int					m_viewOfs;
	};
	
	

} // namespace wg
#endif //WG_LINEEDITOR_DOT_H
