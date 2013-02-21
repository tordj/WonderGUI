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
#ifndef WG_LAYER_DOT_H
#define WG_LAYER_DOT_H

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif


class WgLayerHook : public WgHook
{
	friend class WgLayer;
public:
	WgCoord			Pos() const { return m_geo.Pos(); }
	WgSize			Size() const { 	return m_geo.Size(); }
	WgRect			Geo() const { return m_geo; }

	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgLayerHook *	Prev() const { return static_cast<WgLayerHook*>(_prevHook()); }
	WgLayerHook *	Next() const { return static_cast<WgLayerHook*>(_nextHook()); }
	WgLayer*		Parent() const;

protected:

	void		_requestRender();
	void		_requestRender( const WgRect& rect );
	WgWidgetHolder* _holder() const { return _parent(); }

	WgRect		m_geo;
};


class WgLayer : public WgContainer
{
	friend WgLayerHook;

public:
	bool			IsLayer() const;
	WgLayer *		CastToLayer();
	const WgLayer *	CastToLayer() const;

	WgHook *		SetBase( WgWidget * pWidget );
	WgWidget *		Base();
	bool			DeleteBase();
	WgWidget *		ReleaseBase();


	inline WgLayerHook *	FirstHook() const { return static_cast<WgLayerHook*>(_firstHook()); }
	inline WgLayerHook *	LastHook() const { return static_cast<WgLayerHook*>(_lastHook()); }

	// Overloaded from WgWidget

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			PreferredSize() const;

protected:
	WgLayer();

	class BaseHook : public WgHook
	{
		friend class WgLayer;

	public:

		const char *Type( void ) const;
		static const char * ClassType();

		// Standard Hook methods

		WgCoord		Pos() const { return m_pParent->Pos(); }
		WgSize		Size() const { 	return m_pParent->Size(); }
		WgRect		Geo() const { return m_pParent->Geo(); }

		WgCoord		ScreenPos() const { return m_pParent->ScreenPos(); }
		WgRect		ScreenGeo() const { return m_pParent->ScreenGeo(); }

		WgLayer *		Parent() const { return m_pParent; }

	protected:
		void		_requestRender();
		void		_requestRender( const WgRect& rect );
		void		_requestResize();

		WgHook *	_prevHook() const { return 0; }
		WgHook *	_nextHook() const { return m_pParent->_firstLayerHook(); }
		WgContainer* _parent() const { return m_pParent; }
		WgWidgetHolder* _holder() const { return m_pParent; }

		WgLayer * 	m_pParent;
	};

	virtual	void	_onRequestRender( const WgRect& rect, const WgLayerHook * pHook );	// rect is in our coordinate system.
	virtual WgLayerHook * _firstLayerHook() const = 0;
	virtual void	 _onBaseChanged();


	BaseHook		m_baseHook;
	WgSize			m_size;

};


#endif //WG_LAYER_DOT_H
