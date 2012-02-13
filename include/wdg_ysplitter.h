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

#ifndef	WDG_YSPLITTER_DOT_H
#define	WDG_YSPLITTER_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

class Wdg_YSplitter:public WgWidget, public WgEmitter
{
	public:
		WIDGET_CONSTRUCTORS(Wdg_YSplitter,WgWidget)
		virtual ~Wdg_YSplitter();
		virtual const char * Type() const;
		static const char * GetMyType();

		void	Setup( WgWidget * pTopPane, WgWidget * pSplitHandle, WgWidget * pBottomPane, float topFraction );

		void	Start();
		void	Stop();

		WgWidget* GetTopPane() const { return m_pTopPane; }
		WgWidget* GetBottomPane() const { return m_pBottomPane; }
		WgWidget* GetHandle() const { return m_pHandle; }
		
		void	SetTopFraction(float fraction) { m_topFraction = fraction; UpdatePanes(); }
		float	GetTopFraction() const { return m_topFraction; }


	protected:
		WgWidget * NewWidgetOfMyType() const;

		WgEmitter * GetEmitter() { return this; }

	private:	
		void Init();
		bool DoMyOwnMarkTest( int _x, int _y );
		void DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo );

		void	UpdatePanes();
		void	TopPaneOnly();
		void	BottomPaneOnly();
		void	OnHandlePressed();
		void	OnHandleDragged( int ofs );

		static void cbHandlePressed( void * pMe ) { ((Wdg_YSplitter*)pMe)->OnHandlePressed(); }
		static void cbHandleDragged( void * pMe, int ofs ) { ((Wdg_YSplitter*)pMe)->OnHandleDragged(ofs); }

		WgWidget *	m_pTopPane;
		WgWidget *	m_pBottomPane;
		WgWidget *	m_pHandle;
		float		m_topFraction;		
		int			m_pressPos;
};

#endif //	WDG_YSPLITTER_DOT_H
