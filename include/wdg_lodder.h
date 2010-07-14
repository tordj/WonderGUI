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

#ifndef	WDG_LODDER_DOT_H
#define	WDG_LODDER_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

class Wdg_Lodder:public WgWidget
{
	public:
		WIDGET_CONSTRUCTORS(Wdg_Lodder,WgWidget)
		virtual ~Wdg_Lodder();
		virtual const char * Type() const;
		static const char * GetMyType();

		class Lod : private WgLink
		{
			friend class WgChain<Lod>;
		public:
			WgWidget *	pWidget;
			WgSize		minSize;

			LINK_METHODS( Lod );
		};

		void		AddChildrenAsLODs();
		const Lod*	GetLOD(Uint32 iLod) const;
		Uint32		GetLODCount() const { return m_lodChain.size(); }
		bool		IsLOD(WgWidget* pWidget) const;

	protected:
		WgWidget * NewOfMyType() const;

		bool	AddLOD( WgWidget * pWidget );
		bool	RemoveLOD( WgWidget * pWidget );
		void	Clear();

		void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );

	private:	
		void	Init();
		bool	DoMyOwnMarkTest( int _x, int _y );
		void	DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo );

		void	SelectLod( const WgRect& r );

		WgChain<Lod>	m_lodChain;

		WgWidget *		m_pCurrentLod;

};

#endif //	WDG_LODDER_DOT_H
