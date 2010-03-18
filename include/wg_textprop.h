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

#ifndef WG_TEXTPROP_DOT_H
#define WG_TEXTPROP_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_EMITTER_DOT_H
#	include <wg_emitter.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

class WgChar;
class WgFont;
class WgGlyphSet;
class WgTextProp;
class WgTextPropPtr;
class WgTextPropHolder;
class WgTextPropManager;



//____ WgTextLink _____________________________________________________________

class WgTextLink : public WgEmitter, private WgRefCounted
{
	friend class WgSmartPtr<WgTextLink>;
protected:
	WgTextLink() { m_mode = WG_MODE_NORMAL; };

	WgMode 	m_mode;
};

typedef	WgSmartPtr<WgTextLink>	WgTextLinkPtr;



//____ WgTextPropPtr __________________________________________________________

class WgTextPropPtr
{
public:
	friend class WgChar;
	friend class WgText;
	friend class WgPen;

	WgTextPropPtr( WgTextPropHolder * pProp = 0 );
	explicit WgTextPropPtr( Uint16 hProp );
	WgTextPropPtr(const WgTextPropPtr& r);
	~WgTextPropPtr();


	WgTextPropPtr& operator=(const WgTextPropPtr& ref);
	WgTextPropPtr& operator=(int handle);

	inline bool operator==(const WgTextPropPtr& other) const { return m_hProp == other.m_hProp; }
	inline bool operator!=(const WgTextPropPtr& other) const { return m_hProp != other.m_hProp; }

	const WgTextProp & operator*() const;
	const WgTextProp * operator->() const;
	inline operator bool() const { return (m_hProp != 0); }

	inline	Uint16 GetHandle() const { return m_hProp; }

private:
	Uint16			m_hProp;
};


//____ WgTextProp _____________________________________________________________

class WgTextProp
{
	friend class WgChar;
	friend class WgTextTool;
	friend class WgGfxDevice;
	friend class WgTextPropManager;

public:

	WgTextProp();
	WgTextProp( const WgTextPropPtr& pProp );
//	~WgTextProp();

	WgTextPropPtr	Register() const;

	void			SetColor( WgColor col, WgMode mode = WG_MODE_ALL );
	void			SetStyle( WgFontStyle style, WgMode mode = WG_MODE_ALL );
	void			SetSize( int size, WgMode mode = WG_MODE_ALL );
	void			SetUnderlined( WgMode mode = WG_MODE_ALL );

	inline void		SetLink( const WgTextLinkPtr& pLink ) { m_pLink = pLink; }
	inline void		SetFont( WgFont * pFont ) { m_pFont = pFont; }
	bool			SetCharVisibility( Uint16 specialCharacter, bool bVisible );

	void			ClearColor( WgMode mode = WG_MODE_ALL );
	void			ClearStyle( WgMode mode = WG_MODE_ALL );
	void			ClearSize( WgMode mode = WG_MODE_ALL );
	void			ClearUnderlined( WgMode mode = WG_MODE_ALL );

	inline void		ClearLink() { m_pLink = 0; }
	inline void		ClearFont() { m_pFont = 0; }

	void			ClearAll();


	inline bool				IsUnderlined( WgMode mode = WG_MODE_NORMAL ) const { return m_modeProp[mode].m_bUnderlined; }
	inline bool				IsColored( WgMode mode = WG_MODE_NORMAL ) const { return m_modeProp[mode].m_bColored; }
	inline const WgColor&	GetColor( WgMode mode = WG_MODE_NORMAL ) const { return m_modeProp[mode].m_color; }
	inline WgFontStyle		GetStyle( WgMode mode = WG_MODE_NORMAL ) const { return (WgFontStyle) m_modeProp[mode].m_style; }
	inline int				GetSize( WgMode mode = WG_MODE_NORMAL ) const { return m_modeProp[mode].m_size; }
	bool					GetCharVisibility( Uint16 specialCharacter ) const;

	inline WgTextLinkPtr	GetLink() const { return m_pLink; }
	inline WgFont *		GetFont() const { return m_pFont; }

	void					AssertIntegrity() const;

	bool					IsEqual(WgMode mode0, WgMode mode1) const;
	bool					IsColorStatic() const;
	bool					IsStyleStatic() const;
	bool					IsSizeStatic() const;
	bool					IsUnderlineStatic() const;

	inline bool		CompareTo( const WgTextPropPtr& pProp ) const { return CompareTo( &(*pProp) ); }
	bool			CompareColorTo( const WgTextPropPtr& pProp ) const;
	bool			CompareStyleTo( const WgTextPropPtr& pProp ) const;
	bool			CompareSizeTo( const WgTextPropPtr& pProp ) const;
	bool			CompareUnderlineTo( const WgTextPropPtr& pProp ) const;

private:
	bool			CompareTo( const WgTextProp * pProp ) const;

	Uint8			CalculateChecksum() const;


	// Struct holding WgMode-specific parameters.

	struct WgModeProp
	{
		Uint16			m_style:5;		///< WgFontStyle for this mode, WG_STYLE_NORMAL is same as none and overriden if set higher in hierarchy.
		Uint16			m_size:11;		///< Size in pixels of font. 0 = none specified, use sized specified higher up in hierarchy.
		bool			m_bUnderlined;	///< Hierarchally cumulative, anyone in hierarchy set gives underlined.
		bool			m_bColored;		///< Set if color of this struct should be used.
		WgColor			m_color;		///< Color for character if m_bColored is set. Search order is character, text.
	};

	//____


	union
	{
		struct							///< Hierarchally cumulative, anyone in hierarchy set will set the flag.
		{
			Uint8 m_bShowLF						:1;
			Uint8 m_bShowTAB					:1;
			Uint8 m_bShowNoBreakSpace			:1;
			Uint8 m_bShowBreakPermitted			:1;
			Uint8 m_bShowHyphenBreakPermitted	:1;
			Uint8 m_bShowSpace					:1;
		};
		Uint8 m_visibilityFlags;
	};

	WgTextLinkPtr	m_pLink;			///< Hierarchally overrides.
	WgFont *		m_pFont;			///< Hierarchally overrides.

	WgModeProp		m_modeProp[WG_NB_MODES];
};

// CR, NON-BREAK SPACE, BREAK PERMITTED, HYPHEN BREAK PERMITTED, TAB.



#endif // WG_TEXTPROP_DOT_H


