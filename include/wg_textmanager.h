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

#ifndef	WG_TEXTMANAGER_DOT_H
#define	WG_TEXTMANAGER_DOT_H

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgTextNode;
class WgText;
class WgFont;



class WgTextManager
{
friend class WgText;
friend class WgTextNode;

public:
	WgTextManager();
	virtual ~WgTextManager();

	bool SetScaleValue( float scale );

	bool SetAllowedSizes( int nSizes, float sizes[] );
	bool SetGrowFormula( float treshold, float ratio, float limit );
	bool SetShrinkFormula( float treshold, float ratio, float limit );
	bool SetSizeStepping( float stepping );

	enum Rounding
	{
		ROUND_NEAREST,
		ROUND_UP,
		ROUND_DOWN
	};

	void SetSizeRounding( Rounding rounding );

	inline float	GetGrowTreshold() const { return m_growTreshold; }
	inline float	GetGrowRatio() const { return m_growRatio; }
	inline float	GetGrowLimit() const { return m_growLimit; }

	inline float	GetShrinkTreshold() const { return m_shrinkTreshold; }
	inline float	GetShrinkRatio() const { return m_shrinkRatio; }
	inline float	GetShrinkLimit() const { return m_shrinkLimit; }

	inline float	GetScaleValue() const { return m_scale; }
	inline float	GetSizeStepping() const { return m_sizeStepping; }
	inline Rounding	GetSizeRounding() const { return m_sizeRounding; }

	inline float *	GetAllowedSizes() const { return m_pAllowedSizes; }

private:
	float	GetSize( WgTextNode * pNode, const WgFont * m_pFont, WgFontStyle style, int size );

	virtual WgTextNode *	NewNode( WgText * pText );
	WgChain<WgTextNode>	m_nodes;

	float		m_scale;

	float *		m_pAllowedSizes;

	float		m_growTreshold;
	float		m_growRatio;
	float		m_growLimit;

	float		m_shrinkTreshold;
	float		m_shrinkRatio;
	float		m_shrinkLimit;

	float		m_sizeStepping;

	Rounding	m_sizeRounding;

};


class WgTextNode : public WgLink
{

public:
	WgTextNode( WgTextManager * pManager, WgText * pText );
	virtual ~WgTextNode();

	LINK_METHODS( WgTextNode );

	inline float	GetSize( const WgFont * pFont, WgFontStyle style, int size ) { return m_pManager->GetSize( this, pFont, style, size ); }
	void	Refresh();

	inline WgTextManager *	GetManager() const { return m_pManager; }
	inline WgText *			GetText() const { return m_pText; }

private:
	WgTextManager	* m_pManager;
	WgText			* m_pText;
};


#endif //WG_TEXTMANAGER_DOT_H
