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

#ifndef	WG_GIZMO_RADIOBUTTON_DOT_H
#define	WG_GIZMO_RADIOBUTTON_DOT_H

#ifndef	WG_GIZMO_CHECKBOX_DOT_H
#	include <wg_gizmo_checkbox.h>
#endif

// Radiobutton is almost exactly the same as Checkbox so it inherits from it now
class	WgGizmoRadiobutton : public WgGizmoCheckbox
{

public:
	WgGizmoRadiobutton();
	~WgGizmoRadiobutton();

	virtual const char * Type() const;
	static const char * GetMyType();
	
	bool	SetState( bool state );

	inline void	AllowUnchecking( bool bAllow ) {m_bAllowUnchecking = bAllow; }	///< Should we allow this radiobutton to be unchecked without any other being checked?
	inline bool AllowUnchecking( ) const { return m_bAllowUnchecking; }

private:

	void	_onCloneContent( const WgGizmo * _pOrg );
	
	bool			m_bAllowUnchecking;

};




#endif //	WgGizmoRadiobutton_DOT_H
