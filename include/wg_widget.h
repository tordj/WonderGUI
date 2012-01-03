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

#ifndef	WG_WIDGET_DOT_H
#define	WG_WIDGET_DOT_H

#ifndef WG_TYPES_DOT_H
#	include	<wg_types.h>
#endif
#ifndef WG_RECTCHAIN_DOT_H
#	include	<wg_rectchain.h>
#endif
#ifndef WG_EMITTER_DOT_H
#	include	<wg_emitter.h>
#endif
#ifndef WG_INPUT_DOT_H
#	include	<wg_input.h>
#endif
#ifndef WG_ORIGO_DOT_H
#	include	<wg_origo.h>
#endif

#ifndef WG_SIGNALS_DOT_H
#	include <wg_signals.h>
#endif

#ifndef	WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_STRING_DOT_H
#	include <wg_string.h>
#endif


class	WgWidget;
class	WgChar;
class	WgGizmo;
class	WgHook;
class	Wg_Interface_TextHolder;

//____ Callback Macros ________________________________________________________
/*
	These macros makes it quicker and easier to write the callback methods.
*/


#define BEGIN_CALLBACKS(_class_) \
		typedef _class_ CB_CLASS;

#define CALLBACK_0P( _method_ ) \
		static void cb##_method_( void * pObj ) {((CB_CLASS *)pObj)->_method_();};
#define CALLBACK_1P( _method_, _p1_ ) \
		static void cb##_method_( void * pObj, _p1_ p1 ) {((CB_CLASS *)pObj)->_method_(p1);};
#define CALLBACK_2P( _method_, _p1_, _p2_ ) \
		static void cb##_method_( void * pObj, _p1_ p1, _p2_ p2 ) {((CB_CLASS *)pObj)->_method_(p1,p2);};
#define CALLBACK_3P( _method_, _p1_, _p2_, _p3_ ) \
		static void cb##_method_( void * pObj, _p1_ p1, _p2_ p2, _p3_ p3 ) {((CB_CLASS *)pObj)->_method_(p1,p2,p3);};


//____ Constructor Macros _____________________________________________________
//
// These macros saves us from some repetitive typing when making new widgets.

#define WIDGET_CONSTRUCTORS(_class_,_parentclass_) \
	_class_( WgWidget* pParent = 0 ) : _parentclass_(pParent) { Init(); }; \
	_class_( const WgRect& geometry, WgWidget * pParent = 0 ) : _parentclass_(geometry, pParent) { Init(); }; \
	_class_( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : _parentclass_( origo, geometry, pParent ) { Init(); } \
	_class_( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : _parentclass_(upperLeft,x1, y1, lowerRight, x2, y2, pParent) { Init(); } \


//____ Class: WgWidget __________________________________________________________

class	WgWidget
{
 	friend class Wdg_Root;
	friend class WgInput;
	friend class Wdg_Shader;
	friend class WgWidgetHook;
	friend class WgWidgetContainer;

	public:
		WgWidget( WgWidget* pParent = 0 );
		WgWidget( const WgRect& geometry, WgWidget * pParent = 0 );
		WgWidget( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 );
		WgWidget( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 );
		virtual ~WgWidget();

		//____ Callbacks ______________________________________

		static void cbMove			(void * pWdg, int x, int y) { ((WgWidget*)pWdg)->Move(x,y); }
		static void cbMoveX			(void * pWdg, int x )		{ ((WgWidget*)pWdg)->MoveX(x); }
		static void cbMoveY			(void * pWdg, int y )		{ ((WgWidget*)pWdg)->MoveY(y); }

		static void cbResize		(void * pWdg, int x, int y)	{ ((WgWidget*)pWdg)->Resize(x,y); }
		static void cbResizeWidth	(void * pWdg, int x )		{ ((WgWidget*)pWdg)->ResizeWidth(x); }
		static void cbResizeHeight	(void * pWdg, int y )		{ ((WgWidget*)pWdg)->ResizeHeight(y); }

		static void cbSetSize		(void * pWdg, int x, int y)	{ ((WgWidget*)pWdg)->SetSize(x,y); }
		static void cbSetWidth		(void * pWdg, int x )		{ ((WgWidget*)pWdg)->SetWidth(x); }
		static void cbSetHeight		(void * pWdg, int y )		{ ((WgWidget*)pWdg)->SetHeight(y); }

		static void cbSetPos		(void * pWdg, int x, int y)	{ ((WgWidget*)pWdg)->SetSize(x,y); }
		static void cbSetPosX		(void * pWdg, int x )		{ ((WgWidget*)pWdg)->SetPosX(x); }
		static void cbSetPosY		(void * pWdg, int y )		{ ((WgWidget*)pWdg)->SetPosY(y); }

		static void cbTop			(void * pWdg)				{ ((WgWidget*)pWdg)->Top(); }
		static void cbBottom		(void * pWdg)				{ ((WgWidget*)pWdg)->Bottom(); }
		static void cbUp			(void * pWdg)				{ ((WgWidget*)pWdg)->Up(); }
		static void cbDown			(void * pWdg)				{ ((WgWidget*)pWdg)->Down(); }

		static void cbModal			(void * pWdg)				{ ((WgWidget*)pWdg)->Modal(); }
		static void cbUnModal		(void * pWdg)				{ ((WgWidget*)pWdg)->UnModal(); }

		static void cbEnable		(void * pWdg)				{ ((WgWidget*)pWdg)->Enable(); }
		static void cbDisable		(void * pWdg)				{ ((WgWidget*)pWdg)->Disable(); }

		static void cbEnableBranch	(void * pWdg)				{ ((WgWidget*)pWdg)->EnableBranch(); }
		static void cbDisableBranch	(void * pWdg)				{ ((WgWidget*)pWdg)->DisableBranch(); }

		static void	cbGrabInputFocus(void * pWdg)				{ ((WgWidget*)pWdg)->GrabInputFocus(); }


		/** Interception rules are used by Intercept() for telling what to do on
			 various action sources */

		enum  InterceptionRules
		{
  			PASS,
  			BLOCK,
  			INTERCEPT_PASS,
  			INTERCEPT_BLOCK,
		};

		/** Identifies sources of user actions for Intercept() to act upon */

		enum	ActionSource
		{
			POINTER			= 0x001,		// Blocking this also blocks all button actions. You also must intercept it to be able to intercept any button actions.
			BUTTON1			= 0x002,
			BUTTON2			= 0x004,
			BUTTON3			= 0x008,
			BUTTON4			= 0x010,
			BUTTON5			= 0x020,
			KEYBOARD		= 0x040,		// Blocking doesn't affect keys down with mouse button click.
			WHEEL1			= 0x080,
			WHEEL2			= 0x100,
			WHEEL3			= 0x200
		};

		/** */

		enum	PointerMask
		{
			POINTER_TRANSPARENT,				///< Widget is completely transparent to the pointer.
			POINTER_OPAQUE,						///< Widget is completely opaque to the pointer.
			POINTER_SOURCE_ALPHA,				///< Alpha value of pixel decides.
			POINTER_SOFT_OPAQUE,				///< Widget is opaque to the pointer, but doesn't block for underlying widgets.
		};

		/** */

		enum	ChildGeoPolicy
		{
			CONSTRAINED,
			CLIPPED,
			PUSHED,
//			FLOATING			NOT IMPLEMENTED YET!
		};

		virtual const char *Type( void ) const;
		static const char * GetMyType();

		inline Sint32		Id() const { return (Sint32)m_id; }
		virtual void		SetId( Sint32 id ) { m_id = id; }
		inline Sint64		Id64() const { return m_id; }
		virtual void		SetId64( Sint64 id ) { m_id = id; }

		inline Uint8		Layer() const { return m_layer; }
		inline void			SetLayer( Uint8 layer ) { m_layer = layer; }


		// added by patrik for custom tooltip delay
		inline Sint32		TooltipDelay() const { return m_tooltipDelay; }
		inline void			SetTooltipDelay( Sint32 tooltipDelay ) { m_tooltipDelay = tooltipDelay; }

		virtual bool		IsInputField() const { return false; }
		virtual Wg_Interface_TextHolder* GetText() { return 0; }

		virtual WgString	GetTooltipString() const	{ return 0; }
		virtual WgItem*		GetTooltipItem() const { return 0; }

		virtual void			SetPointerStyle( WgPointerStyle style )		{ m_pointerStyle = style; }
		virtual WgPointerStyle	GetPointerStyle() const					{ return m_pointerStyle; }

		virtual bool		SetParent( WgWidget * _pParent, WgWidget * pBelow = 0 );	      // Set parent widget.
		inline WgWidget*	Parent() const 		{return m_pParent;}
		inline WgWidget*	FirstChild() const 	{return m_pFirstChild;}
		inline WgWidget*	LastChild() const 	{return m_pLastChild;}
		inline WgWidget*	PrevSibling() const	{return m_pPrevSibling;}
		inline WgWidget*	NextSibling() const	{return m_pNextSibling;}
		bool				IsChildOf(WgWidget* pOther);
		
		WgWidget *			GetNext() const;
		WgWidget *			GetPrev() const;
		WgWidget *			GetLastOffspring() const;


		WgWidget*			Root();											///< Returns the root of this widgets branch. Not necessarily a Wdg_Root object!
		virtual	bool		IsRootWidget() const { return false; }			///< Returns true if this widget is a real root widget (of the Wdg_Root class).

		virtual bool		SetGeometry( WgOrigo _origo, const WgRect& _geometry, bool bEmit = true );
		virtual bool		SetGeometry( WgOrigo _topLeft, Sint32 x1, Sint32 y1, WgOrigo _bottomRight, Sint32 x2, Sint32 y2, bool bEmit = true );
		virtual bool		SetGeometry( const WgRect& _geometry, bool bEmit = true );
		inline const WgRect&	Geometry() const { return m_geo; };
		WgRect				ScreenGeometry() const;

		virtual bool		SetOrigo( WgOrigo _origo );														// Set Widgets Origos.
		virtual bool		SetOrigo( WgOrigo _topLeft, WgOrigo _bottomRight );	// Set Widgets Origos.
		inline WgOrigo		OrigoTopLeft() const {return m_origo1;};
		inline WgOrigo		OrigoBottomRight() const {return m_origo2;};

		virtual bool		CloneGeometry( const WgWidget* pCloneFrom );

		virtual bool	Move( int _x, int _y );             // Move current Origo-related position (pixels).
		virtual bool	MoveX( int _x );
		virtual bool 	MoveY( int _y );

		virtual bool	Resize( int _w, int _h, bool bAllowMoveChildren = false );			// Modify current width & height (pixels).
		virtual bool	ResizeWidth( int _w, bool bAllowMoveChildren = false );              // Modify current width.
		virtual bool	ResizeHeight( int _h, bool bAllowMoveChildren = false );             // Modify current height;

		virtual bool	SetSize( int _w, int _h, bool bAllowMoveChildren = false );    // Set new size (in pixels).
		virtual bool	SetWidth( int _w, bool bAllowMoveChildren = false );
		virtual bool	SetHeight( int _h, bool bAllowMoveChildren = false );

		virtual bool	SetPos( int _x, int _y );			// Set to Origo-related offset (in pixels).
		virtual bool	SetPosX( int _x );
		virtual bool	SetPosY( int _y );

		virtual void	SetChildGeoPolicy( ChildGeoPolicy policy );
		virtual ChildGeoPolicy	GetChildGeoPolicy() const { return m_childGeoPolicy; }

//		virtual void	SetConstrainChildren( bool bConstrain );
//		virtual bool	GetConstrainChildren() const { return m_bConstrainChildren; }

		virtual void	Enable();
		virtual void	Disable();
		virtual void	HideBranch();
		virtual void	ShowBranch();
		inline  void	SetEnabled(bool bEnable) { bEnable ? Enable() : Disable(); }
		inline  bool	IsEnabled() { return m_bEnabled; }
		inline  bool	IsSetToHide() { return m_bHidden; }
		bool			IsHidden();
		virtual void	EnableBranch();
		virtual void	DisableBranch();

		inline void		Refresh() { DoMyOwnRefresh(); };	///< Refresh "external" dependencies, such redrawing if Surface content of Font has changed.
		void			RefreshBranch();

		virtual void	Modal();							// Turn widget modal (on top of parent and siblings,
															// blocking them from receiving actions).
		virtual void	UnModal();                      	// Turn widget non-modal.
		inline bool		IsModal() const { return m_bModal; }

		virtual void	SetPointerMask( PointerMask mode );
		PointerMask		GetPointerMask() const { return m_pointerMask; }

		virtual bool	MinSize( int _w, int _h );			// Minimum size for this widget.
		virtual bool	MaxSize( int _w, int _h );			// Maximum size for this widget.

		inline  bool	MinSize( WgSize size ) { return MinSize(size.w, size.h); }
		inline  bool	MaxSize( WgSize size ) { return MaxSize(size.w, size.h); }
		inline  WgSize  GetMinSize() const { return WgSize(MinWidth(), MinHeight()); }
		inline  WgSize  GetMaxSize() const { return WgSize(MaxWidth(), MaxHeight()); }
		inline  WgSize  GetMinSizeUser() const { return WgSize(MinWidthUser(), MinHeightUser()); }
		inline  WgSize  GetMaxSizeUser() const { return WgSize(MaxWidthUser(), MaxHeightUser()); }

		inline	bool	SetMinWidth( int _w ) { return MinSize( _w, m_sizeLimit_user.minH); }
		inline	bool	SetMinHeight( int _h ) { return MinSize( m_sizeLimit_user.minW, _h ); }
		inline	bool	SetMaxWidth( int _w ) { return MaxSize( _w, m_sizeLimit_user.maxH); }
		inline	bool	SetMaxHeight( int _h ) { return MaxSize(m_sizeLimit_user.maxW, _h); }


		virtual bool	Top();								// Put us ontop all our silbings (except modal ones).
		virtual bool	Bottom();							// Put us below all our siblings.
		virtual bool	Up();								// Move us one step up among siblings.
		virtual bool	Down();								// Move us one step down among siblings.

		bool			MoveAbove( WgWidget * pSibling );
		bool			MoveBelow( WgWidget * pSibling );

		bool			HasInputFocus();
		bool			GrabInputFocus();
		bool			RemoveInputFocus();

		WgWidget * 		FindOccupant( int x, int y, bool bCheckModal, WgWidget * bestSoFar = 0, WgWidget ** wpBlockingModal = 0 );	// Return widget (me, child or 0) occupying position
																									//  at x, y in PARENTS local cordinate system.
		bool				Intercept( ActionSource _actionSource, InterceptionRules _rules );

		InterceptionRules	GetInterceptionRules( ActionSource _actionSource );
                                                     

		void			Local2abs( int * _pX, int * _pY ) const;		// Cordinate from local cordsys to global
		void			Abs2local( int * _pX, int * _pY ) const; 		// Cordinate from global to local cordsys
		inline int		PosX() const	{ return m_x1; }		// Origo-relative offset for top-left corner.
		inline int		PosY() const	{ return m_y1; }		// Origo-relative offset for top-left corner.
		inline int		PosX2() const	{ return m_x2; }		// Origo-relative offset for bottom-right corner.
		inline int		PosY2() const	{ return m_y2; }		// Origo-relative offset for bottom-right corner.
		inline int		Width() const	{ return m_geo.w; }
		inline int		Height() const	{ return m_geo.h; }
		inline int		MinWidth() const	{ return m_sizeLimit.minW; }
		inline int		MinHeight() const	{ return m_sizeLimit.minH; }
		inline int		MaxWidth() const	{ return m_sizeLimit.maxW; }
		inline int		MaxHeight() const	{ return m_sizeLimit.maxH; }
		inline int		MinWidthUser() const	{ return m_sizeLimit_user.minW; }
		inline int		MinHeightUser() const	{ return m_sizeLimit_user.minH; }
		inline int		MaxWidthUser() const	{ return m_sizeLimit_user.maxW; }
		inline int		MaxHeightUser() const	{ return m_sizeLimit_user.maxH; }

		WgWidget *		Clone();
		WgWidget *		CloneBranch();

		// Not meant to be public, but had to... :(

		bool			PathFinder( const WgWidget * _goal, char * _pMapEntry ) const;
		WgWidget * 		PathFollower( const char * _pMap ) const;


		inline void		ForceOpaque() { m_bOpaque = true; }

		virtual WgGizmo * GetGizmo() { return 0; }
		virtual const WgGizmo * GetGizmo() const { return 0; }
		virtual WgHook * GetHook() const { return 0; }

		virtual WgEmitter * GetEmitter() = 0;



	inline void AddCallbackW( WgSignal::Signal_void sig, void(*fp)(void*), void * pObj ) {	if( !m_bDestroyed ) GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_void signal ) { if( !m_bDestroyed ) GetEmitter()->Emit(signal); }																					
	inline int	RemoveCallbackW( WgSignal::Signal_void signal, void(*fp)(void*), void * pObj ) { return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }
	
	inline void AddCallbackW( WgSignal::Signal_bool sig, void(*fp)(void*,bool), void * pObj ) { if( !m_bDestroyed ) GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_bool signal, bool p1 ) { if( !m_bDestroyed ) GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,bool), void * pObj ) {	return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_int sig, void(*fp)(void*,int), void * pObj ) { if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_int signal, int p1 ) { if( !m_bDestroyed )GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,int), void * pObj ) {	return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_int64 sig, void(*fp)(void*,Sint64), void * pObj ) { if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_int64 signal, Sint64 p1 ) { if( !m_bDestroyed )GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,Sint64), void * pObj ) {	return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_float sig, void(*fp)(void*,float), void * pObj ) { if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_float signal, float p1 ) { if( !m_bDestroyed )GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,float), void * pObj ) {	return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_voidptr sig, void(*fp)(void*,const void *), void * pObj ) { if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_voidptr signal, const void * p1 ) { if( !m_bDestroyed )GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,const void *), void * pObj ) {	return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_charptr sig, void(*fp)(void*,const char *), void * pObj ) { if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_charptr signal, const char * p1 ) { if( !m_bDestroyed )GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,const char *), void * pObj ) {	return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_item sig, void(*fp)(void*,WgItem *), void * pObj ) { if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_item signal, WgItem * p1 ) { if( !m_bDestroyed )GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,WgItem *), void * pObj ) {	return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_menuitem sig, void(*fp)(void*,WgMenuItem *), void * pObj ) { if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_menuitem signal, WgMenuItem * p1 ) { if( !m_bDestroyed )GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,WgMenuItem *), void * pObj ) {	return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_treeentry sig, void(*fp)(void*,WgTreeEntry *), void * pObj ) { if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_treeentry signal, WgTreeEntry * p1 ) { if( !m_bDestroyed )GetEmitter()->Emit(signal,p1); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,WgTreeEntry *), void * pObj ) { return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_bool_bool sig, void(*fp)(void*,bool,bool), void * pObj ) {	if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_bool_bool signal, bool p1, bool p2 )	{ if( !m_bDestroyed )GetEmitter()->Emit(signal,p1,p2); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,bool,bool), void * pObj ) { return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_int_int sig, void(*fp)(void*,int,int), void * pObj ) {	if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_int_int signal, int p1, int p2 )	{ if( !m_bDestroyed )GetEmitter()->Emit(signal,p1,p2); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,int,int), void * pObj ) { return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }

	inline void AddCallbackW( WgSignal::Signal_float_float sig, void(*fp)(void*,float,float), void * pObj ) {	if( !m_bDestroyed )GetEmitter()->AddCallbackUnsafe( sig, (void*) fp, pObj ); }
	inline void EmitW( WgSignal::Signal_float_float signal, float p1, float p2 )	{ if( !m_bDestroyed )GetEmitter()->Emit(signal,p1,p2); }
	inline int	RemoveCallbackW( WgSignal::Signal signal, void(*fp)(void*,float,float), void * pObj ) { return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, (void*) fp, pObj ):0; }


	inline int	RemoveCallbackW( WgSignal::Signal signal, void * pCallback, void * pObj ) { return (!m_bDestroyed)?GetEmitter()->RemoveCallback( signal, pCallback, pObj ):0; } 
	inline int	RemoveCallbacksW( WgSignal::Signal signal ) { return (!m_bDestroyed)?GetEmitter()->RemoveCallbacks( signal ):0; }
	inline int	RemoveCallbacksW( void * pObj ) { return (!m_bDestroyed)?GetEmitter()->RemoveCallbacks( pObj ):0; }
	inline int	RemoveCallbacksW() { return (!m_bDestroyed)?GetEmitter()->RemoveCallbacks():0; }





	protected:

		Sint64		m_id;
		Sint32		m_tooltipDelay;
		Uint8		m_layer;

		WgPointerStyle	m_pointerStyle;

		// Only to be manipulated by Class::WgWidget itself.
		// Read-only for derived WgWidget-classes!

		WgWidget	* m_pPrevSibling;
		WgWidget	* m_pNextSibling;
		WgWidget	* m_pParent;
		WgWidget	* m_pFirstChild;
		WgWidget	* m_pLastChild;

		WgOrigo		m_origo1;					// Origo for X1, Y1.
		WgOrigo		m_origo2;					// Origo for X2, Y2.
		Sint32		m_x1, m_y1, m_x2, m_y2;		// Offsets from origo for X1,Y1,X2,Y2.
		WgRect		m_geo;						// Widget geometry. I.e. position, width, height in parent cordinates (calculated).

		bool		m_bEnabled;
		bool		m_bHidden;

		ChildGeoPolicy	m_childGeoPolicy;

//		bool		m_bConstrainChildren;		// Forces children to stay inside.
		bool		m_bMoveChildren;


		bool		m_bDestroyed;


		// To be set accordingly by derived classes. Initialised to default values.

		WgMinMax2D	m_sizeLimit_self;			// Sizelimits defined by widget itself (high prio).

		PointerMask	m_pointerMask;				// Under what circumstances do we accept mouse marking/clicking?

		bool		m_bOpaque;					// Is widget completely without transparent/masked pixels?

		bool		m_bRendersAll;				// Widget doesn't handle dirty rectangles, doMyOwnRender() always
												//  updates all of it, possibly causing massive overdraws.

		bool		m_bRenderOne;				// Set if overhead for each rendered dirty rectangle is so high that
												// it's prefered to just render one rectangle even if it means an
												// extra big rectangle and possibly overdraws (not to be set if
												// bRendersAll is set).

    // Interface for derived classes.

		void		RefreshSizeLimit( void );

		inline void	RequestRender() { m_dirtyRects.Clear(); RequestRender( 0, 0, m_geo.w, m_geo.h, m_pFirstChild, true ); };
		inline void RequestRender( const WgRect& rect ) { RequestRender( rect.x, rect.y, rect.w, rect.h, m_pFirstChild, true ); };
		void		RequestRender( int _x, int _y, int _w, int _h,						// In local cordinate system.
									WgWidget * _pMaskBranch = 0, bool _bParentToo = true );
		inline void	ClearRenderQueue( void ) { m_dirtyRects.Clear(); };		// Only necessary for Size(), other
																			// functions can use it as a slight
																			// optimization before doing a
																			// RequestRender() that fills it all.


private:
		void 		Init();						// Common stuff for all Constructors.

		bool		_setParent( WgWidget * _pNewParent, WgWidget * _pBelow, bool bEmit );


		void		removeFromInputRecursively( WgInput * pInput );

		WgInput*	GetInputObj();

		bool		m_bModal;
		bool		m_bRenderedHere;			// Have we rendered since last move/size/parent/top/bottom/up/down?

		WgMinMax2D	m_sizeLimit;				// SizeLimit for user, self and tree combined.
		WgMinMax2D	m_sizeLimit_user;			// User defined sizelimits (low prio).
		WgMinMax2D	m_sizeLimit_tree;			// Sizelimits because of child-hierarchy (mid prio).

    	Uint16     	m_actionFilterBlock;		// What actions to block.
    	Uint16     	m_actionFilterCopy;			// What actions to copy.

		void	MaskAgainstBranch( WgRectChain * _pDirtObj, int _ofsX, int _ofsY );
		void	AdoptionFocusUpdate( WgWidget * pNewParent );

		// WgWidgets own update routine.

		void	UpdateX( const WgUpdateInfo& updateInfo );

		// Callback for WgInput when widget receives actions.

		void	ActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );

		// Callback for WgInput when widget gains/loses input focus.

		void	GotInputFocus( void );
		void	LostInputFocus( void );


		// Functions related to resizing (and indirectly moving/parenting etc).

		void	RefreshTreeSizeLimit( void );	// Calculates minWidth & minHeight for szLimit_tree;
		bool	UpdateGeometry(bool bPreferResize, bool bMoveChildren = false, bool bEmit = true );
//		void	AnchorRefresh();				// Called when parent moves/resizes.

		// Common subfunctions for Parent/Up/Down/Top/Bottom/Modal

 		bool	Disconnect(); 											// Disconnect from parent and siblings.
		bool	Connect( WgWidget * pParent, WgWidget * pPutBelow );	// Connect to new parent (at specified sibling position).

		// Subfunctions for WgWidget::Clone()

		void	CloneContent( WgWidget * _pClone, WgWidget * _pCloneRoot, WgWidget * _pTreeRoot );
		//void	BuildCloneBranch( WgWidget ** _wpAddr, WgWidget * _pCloneParent );
		WgWidget* BuildCloneBranch( const WgWidget * pSrc, WgWidget * pDstParent );

		// Data only used by widget-render.cpp

		WgRectChain m_dirtyRects;

		// Subfunctions for WgWidget::Render().

		virtual void	RenderRecursively(Uint8 _layer, const WgRect& clip );
		void	PreRenderMasking( int _ofsX, int _ofsY );
		void	ClearBranchFromDirt( void );
		void	FreezeBranch( int _ofsX, int _ofsY );
		void	PushDirt( int _ofsX, int _ofsY );
		void	PushDirtSub( WgRectChain * _pDirtObj, int _ofsX, int _ofsY );
		void	AddDirtToNonOpaqueCovering( WgRectChain * _pDirtObj, int _ofsX, int _ofsY );
		bool	MaskAgainstFrozenBranch( WgRectChain * _pDirtObj, int _ofsX, int _ofsY );
		void	CollectDirtyRectangles( WgRectChain * _pDirtObj );
		void	PushDirtSubCovering( WgRectChain * _pDirtObj, int _ofsX, int _ofsY );
		void	ClipDirtAgainstVisibleGeometry( const WgRect& visible );


protected:

		// The following methods can be provided by derived widget classes

		// Called when geometry has been changed, so widget can for example
		// send special signals resulting from resizing.

		virtual void DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo );

		// Called by user when he suspects that one or more of your "external
		// dependencies" (Surfaces, Fonts etc) have been modified. Widget should
		// then check all dependencies and possibly update them and request render.

		virtual void DoMyOwnRefresh( void );

		// Called once for each update if registered with root widget. Used for periodic
		// or timer based updates.

		virtual void DoMyOwnUpdate( const WgUpdateInfo& _updateInfo );

		// Called when mouse actions affecting this widget have been performed.

		virtual void DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );

		// Graphical update. Called once for each dirty rectangle (or just once if
		// bRenderOne or bRenderAll is set). Inparameters are in real screen cordinates.
		// pWindow is the widgets position and size while pClip (which is totally inside
		// pWindow) is the area to be updated.

 		virtual void DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer );

		// Called during cloning. Should copy and modify widgets own parameters accordingly.

		virtual	void DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot,
																 const WgWidget * _pBranchRoot );

		// Only called if bOpaqueForMouse == false. Should return true if a click
		// on ( _x, _y ) (in widgets local cordinate system) should be interpreted
		// as a click on this widget.

		virtual	bool DoMyOwnMarkTest( int _x, int _y ) = 0;


		// Called when widget is enabled or disabled.

		virtual void DoMyOwnDisOrEnable( void );

		// Called when widget gets or loses input focus.

		virtual void DoMyOwnInputFocusChange( bool _bFocus ) {};


		// Slightly clumsy solution to a cloning-related problem.
		// Just put:
		// WgWidget * NewOfMyType( int number ) { return new Wdg_Fill[number]; };
		// as a protected method in the header file.

		virtual WgWidget * NewOfMyType() const = 0;

};


#endif	// WG_WIDGET_DOT_H
