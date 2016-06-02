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

#ifndef	WG_PRINTABLEFIELD_DOT_H
#define WG_PRINTABLEFIELD_DOT_H

#ifndef WG_FIELD_DOT_H
#	include <wg_field.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_PRINTER_DOT_H
#	include <wg_printer.h>
#endif

#ifndef WG_BASE_DOT_H
#	include <wg_base.h>
#endif

namespace wg 
{
	
	
	//____ PrintableHolder ___________________________________________________________
	
	struct PrintableHolder : public FieldHolder
	{
	};
	
	//____ PrintableField __________________________________________________________________
	
	class PrintableField : public Field
	{
		friend class Printer;
	public:
		PrintableField( PrintableHolder * pHolder );
		virtual ~PrintableField();
	
		virtual void		setStyle( const TextStyle_p& pStyle );
		virtual void		clearStyle();
		TextStyle_p			style() const { return m_pStyle; }
	
		virtual void		setPrinter( const Printer_p& pPrinter );
		virtual void		clearPrinter();
		Printer_p			printer() const { return m_pPrinter; }
	
		virtual void		setState( State state );
		inline State		state() const { return m_state; }
	
		virtual Size		preferredSize() const;	
		virtual int			matchingWidth( int height ) const;
		virtual int			matchingHeight( int width ) const;
		inline Size			size() const { return m_size; }
	
		virtual int			charAtPos( Coord pos ) const;
		virtual Rect		charRect( int charOfs ) const;
		virtual int			charLine( int charOfs ) const;
	
		virtual void		onRefresh();
	
		virtual String		tooltip() const;
	
		virtual void		onNewSize( const Size& size );	
		virtual void		onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip );
	
		virtual Rect		rectForRange( int ofs, int length ) const;
		
		virtual String		getString() const;
		
		virtual int			selectionBegin() const;
		virtual int			selectionEnd() const;

		Printer *			_printer() const { return m_pPrinter ? m_pPrinter.rawPtr() : Base::defaultPrinter().rawPtr(); }
		TextStyle *			_style() const { if( m_pStyle ) return m_pStyle.rawPtr(); return Base::defaultStyle().rawPtr(); }
		
	protected:
	
		Size				m_size;
	
		union 
		{
			void *			m_pPrinterData;
			int				m_printerData;
		};
	
		State				m_state;
		TextStyle_p			m_pStyle;
		CharBuffer			m_charBuffer;
		Printer_p			m_pPrinter;
	
	};
	
	struct EditState
	{
		bool 			bCaret;			// Set if caret should be displayed.
		bool			bShiftDown;		// Set if caret is in "selection mode" modifying the selection when moving.
		bool			bButtonDown;	// Set when mouse button was pressed inside field and still is down.
		int 			selectOfs;		// Selection is between selectOfs and caretOfs.
		int				caretOfs;		// End of selection and caret offset (if displaying)
		int				wantedOfs;		// Carets wanted offset in pixels when skipping between lines. -1 = none set.		
	};
	
	

} // namespace wg
#endif //WG_PRINTABLEFIELD_DOT_H
