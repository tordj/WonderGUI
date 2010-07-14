#ifndef	WG_BASECLASS_TEXT_DOT_H
#define WG_BASECLASS_TEXT_DOT_H

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_FONTSET_DOT_H
#	include <wg_fontset.h>
#endif

class WgWidget;

//____ Wdg_Baseclass_Text _____________________________________________________

class Wdg_Baseclass_Text
{
	public:
		void	SetTextObj( WgText * pText );
		void	SetTextAlignment( WgOrigo origo );
		bool	SetFonts( WgFontSet * pFont );
			
		inline WgOrigo 		TextAlignment() const;
		inline WgFontSet * 	Fonts() const;
		inline WgText *		TextObj() const;
	
protected:

		virtual void DoMyOwnRefresh( void );
		virtual void DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot,
									 const WgWidget * _pBranchRoot );
		virtual void DoMyOwnDisOrEnable( void ); 


		WgFontSet *	m_pFonts;
		WgText *	m_pText;			// Must always exist!
		WgOrigo		m_textOrigo;

	private:
		void	Init();
		void	ApplyFont();
};


//____ inline methods __________________________________________________________


WgOrigo WgTextWidget::TextAlignment() const
{
	return m_textOrigo;
}

WgFontSet * WgTextWidget::Fonts() const
{
	return m_pFonts;
}

WgText* WgTextWidget::TextObj() const
{
	return m_pText;
}


#endif // WDG_BASECLASS_TEXT_DOT_H
