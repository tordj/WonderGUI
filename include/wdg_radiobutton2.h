/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@swipnet.se].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------
	
  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@swipnet.se] for details.

=========================================================================*/

#ifndef	WDG_RADIOBUTTON2_DOT_H
#define	WDG_RADIOBUTTON2_DOT_H


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_RADIOBUTTON_DOT_H
#	include <wg_gizmo_radiobutton.h>
#endif

class Wdg_RadioButton2 : public Wdg_Widget<WgGizmoRadiobutton>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_RadioButton2,WgGizmoRadiobutton);
	virtual ~Wdg_RadioButton2() {};

	static void cbCheck( void * pObj ) {((Wdg_RadioButton2*)pObj)->Check();};
	static void cbUncheck( void * pObj ) {((Wdg_RadioButton2*)pObj)->Uncheck();};
	static void cbChangeState( void * pObj ) {((Wdg_RadioButton2*)pObj)->ChangeState();};
	static void cbSetState( void * pObj, bool state ) {((Wdg_RadioButton2*)pObj)->SetState(state);};


};

/*
#ifndef	WDG_CHECKBOX2_DOT_H
#	include <wdg_checkbox2.h>
#endif

// RadioButton2 is almost exactly the same as CheckBox2 so it inherits from it now
class	Wdg_RadioButton2 : public Wdg_CheckBox2
{

public:
	WIDGET_CONSTRUCTORS(Wdg_RadioButton2,Wdg_CheckBox2);
	~Wdg_RadioButton2();

	virtual const char * Type() const;
	static const char * GetMyType();
	
	bool	SetState( bool state );

	inline void	AllowUnchecking( bool bAllow ) {m_bAllowUnchecking = bAllow; }	///< Should we allow this radiobutton to be unchecked without any other being checked?
	inline bool AllowUnchecking( ) const { return m_bAllowUnchecking; }

protected:
	WgWidget * NewWidgetOfMyType() const;
private:
	void Init();

	void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
	
	bool			m_bAllowUnchecking;

};
*/



#endif //	WDG_RADIOBUTTON2_DOT_H
