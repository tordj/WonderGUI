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

class WgLayer;
typedef	WgStrongPtr<WgLayer,WgContainerPtr>	WgLayerPtr;
typedef	WgWeakPtr<WgLayer,WgContainerPtr>	WgLayerWeakPtr;

class WgLayerHook;
typedef	WgHookTypePtr<WgLayerHook,WgHookPtr>	WgLayerHookPtr;


class WgLayerHook : public WgHook
{
	friend class WgLayer;
	friend class WgModalLayer;

public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgLayerHookPtr	Cast( const WgHookPtr& pInterface );

	WgCoord			Pos() const { return m_geo.Pos(); }
	WgSize			Size() const { 	return m_geo.Size(); }
	WgRect			Geo() const { return m_geo; }

	WgCoord			GlobalPos() const;
	WgRect			GlobalGeo() const;

	WgLayerHookPtr	Prev() const { return _prevLayerHook(); }
	WgLayerHookPtr	Next() const { return _nextLayerHook(); }
	WgLayerPtr		Parent() const;

protected:

	virtual WgLayerHook *	_prevLayerHook() const = 0;			// Iterate through all hooks except the base hook
	virtual WgLayerHook *	_nextLayerHook() const = 0;			// Iterate through all hooks except the base hook
	
	WgHook *		_prevHook() const;							// Iterates through all hooks, including the base hook
	WgHook *		_nextHook() const;							// Iterates through all hooks, including the base hook
	
	void			_requestRender();
	void			_requestRender( const WgRect& rect );

	WgRect		m_geo;
};


class WgLayer : public WgContainer
{
	friend class WgLayerHook;

public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLayerPtr	Cast( const WgObjectPtr& pObject );

	WgHookPtr			SetBaseWidget( const WgWidgetPtr& pWidget );
	WgWidgetPtr			BaseWidget();
	bool				RemoveBaseWidget();
	inline WgHookPtr	BaseHook() { return &m_baseHook; }

	inline WgLayerHookPtr	FirstLayerHook() const { return _firstLayerHook(); }
	inline WgLayerHookPtr	LastLayerHook() const { return _lastLayerHook(); }

	// Overloaded from WgWidget

	int					MatchingHeight( int width ) const;
	int					MatchingWidth( int height ) const;

	WgSize				PreferredSize() const;

protected:
	WgLayer();

	class _BaseHook : public WgHook
	{
		friend class WgLayer;
		friend class WgLayerHook;
		friend class WgModalLayer;
		friend class WgPopupLayer;

	public:

		// Standard Hook methods

		WgCoord		Pos() const { return m_pParent->Pos(); }
		WgSize		Size() const { 	return m_pParent->Size(); }
		WgRect		Geo() const { return m_pParent->Geo(); }

		WgCoord		GlobalPos() const { return m_pParent->GlobalPos(); }
		WgRect		GlobalGeo() const { return m_pParent->GlobalGeo(); }

	protected:

		void		_requestRender();
		void		_requestRender( const WgRect& rect );
		void		_requestResize();

		WgHook *		_prevHook() const { return 0; }
		WgHook *		_nextHook() const { return m_pParent->_firstLayerHook(); }
		WgContainer*	_parent() const { return m_pParent; }

		WgLayer * 	m_pParent;
	};

	_BaseHook		m_baseHook;

	virtual	void	_onRequestRender( const WgRect& rect, const WgLayerHook * pHook );	// rect is in our coordinate system.
	virtual WgLayerHook * _firstLayerHook() const = 0;
	virtual WgLayerHook * _lastLayerHook() const = 0;

	WgHook *			_firstHook() const;		// Fist Hook returned is for the base, then follows the LayerHooks.
	WgHook *			_lastHook() const;		//

	WgHook *		_firstHookWithGeo( WgRect& geo ) const;
	WgHook *		_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *		_lastHookWithGeo( WgRect& geo ) const;
	WgHook *		_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;


	virtual void	 _onBaseChanged();

	WgSize			m_size;

};


#endif //WG_LAYER_DOT_H
