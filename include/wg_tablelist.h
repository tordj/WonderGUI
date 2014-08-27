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

#ifndef	WG_TABLELIST_DOT_H
#define	WG_TABLELIST_DOT_H

#ifndef WG_LIST_DOT_H
#	include <wg_list.h>
#endif

#ifndef WG_SIZEBROKER_DOT_H
#	include <wg_sizebroker.h>
#endif

class WgTableList;
typedef	WgStrongPtr<WgTableList,WgListPtr>		WgTableListPtr;
typedef	WgWeakPtr<WgTableList,WgListWeakPtr>	WgTableListWeakPtr;

class WgTableListHook;
typedef	WgHookTypePtr<WgTableListHook,WgListHookPtr>	WgTableListHookPtr;


class WgTableListHook : public WgTableListHook
{
	friend class WgTableList;
	friend class WgHookArray<WgTableListHook>;
public:
	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgTableListHookPtr	Cast( const WgHookPtr& pInterface );

	WgCoord			Pos() const;
	WgSize			Size() const;
	WgRect			Geo() const;
	WgCoord			GlobalPos() const;
	WgRect			GlobalGeo() const;

	WgTableListHookPtr	Prev() const { return static_cast<WgTableListHook*>(_prevHook()); }
	WgTableListHookPtr	Next() const { return static_cast<WgTableListHook*>(_nextHook()); }
	WgTableListPtr		Parent() const { return m_pParent; }

protected:
	WgTableListHook() {};

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;

	WgContainer *	_parent() const;
	
	WgTableList *	m_pParent;

	int				m_prefLength;		// Preferred length of this widget.
	int				m_prefBreadth;		// Preferred breadth of this widget.

};


//____ WgTableList ____________________________________________________________

class WgTableList : public WgList
{
	friend class WgTableListHook;
public:
	static WgTableListPtr	Create() { return WgTableListPtr(new WgTableList()); }
	
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgTableListPtr	Cast( const WgObjectPtr& pObject );

	void					SetOrientation( WgOrientation orientation );
	WgOrientation			Orientation() const { return m_bHorizontal?WG_HORIZONTAL:WG_VERTICAL; }

	WgSize					PreferredSize() const;
	int						HeightForWidth( int width ) const;
	int						WidthForHeight( int height ) const;

	void					SortRows();
	void					SetSortColumn( int column );
	int						GetSortColumn() const { return m_sortColumn; }

	bool					Clear();

	class Header
	{
		friend class WgTableList;
	public:
		WgIconField			arrow;

		void				SetSkin( const WgSkinPtr& pSkin );
		inline WgSkinPtr	Skin() const { return m_pSkin; }

		inline WgIIconPtr	Arrow() { return WgIIconPtr(m_pHolder, &arrow); }
	private:
		void				_fieldModified( WgIconField * pField );

		WgTableList *		m_pHolder;
		WgSkinPtr			m_pSkin;
	};

	class Columns
	{
	public:
		int Size() const;
		
		int	Add( int nb = 1 );
		int Insert( int index, int nb = 1 );
		void Remove( int index, int nb = 1 );

		void			SetSizeBroker( const WgSizeBrokerPtr& pBroker );
		WgSizeBrokerPtr	SizeBroker() const { return m_pSizeBroker; }
		
	private:
		WgSizeBrokerPtr	m_pSizeBroker;
		
	};
	
	class Rows
	{
	public:
		int Size() const;

		int	Add( int nb = 1 );
		int Insert( int index, int nb = 1 );
		void Remove( int index, int nb = 1 );
		
	};

	class Column
	{
	public:
		class Header
		{
		public:
			WgIconField			icon;
			WgTextField			label;

			inline WgIIconPtr		Icon() { return WgIIconPtr(m_pHolder, &icon); }
			inline WgIModifTextPtr	Label() { return WgIModifTextPtr(m_pHolder, &label); }

			bool	SetWeight( float weight );
			float	Weight() { return m_weight; }

		private:
			void				_fieldModified( WgTextField * pText );
			void				_fieldModified( WgIconField * pField );

			WgTableList *		m_pHolder;
			float				m_weight;
			WgSize				m_preferredSize;
			WgState				m_state;
			bool				m_bPressed;			
		};
		
		class Hooks
		{
		public:
			
		};
		
		class Widgets
		{
		public:	
		};
		
		Header		header;
		Hooks		hooks;
		Widgets		widgets;
		
		void		SetVisible( bool bVisible );
		bool		SetWidget( int row );
		WgWidgetPtr	RemoveWidget( int row );

	};

	class Row
	{
	public:
		class Hooks
		{
		public:
			
		};
		
		class Widgets
		{
		public:	
		};
		
		Hooks	hooks;
		Widget	widgets;
	};


	Header			header;
	Columns			columns;
	Rows			rows;

protected:
	WgTableList();
	virtual ~WgTableList();
	WgWidget*		_newOfMyType() const { return new WgTableList(); };

	bool			m_bHorizontal;
	int				m_sortColumn;



	WgHookArray<WgTableListHook>	m_hooks;
};




#endif //WG_TABLELIST_DOT_H