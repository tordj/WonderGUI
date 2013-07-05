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

#ifndef	WG_RADIOBUTTON_DOT_H
#define	WG_RADIOBUTTON_DOT_H

#ifndef	WG_CHECKBOX_DOT_H
#	include <wg_checkbox.h>
#endif

// Radiobutton is almost exactly the same as Checkbox so it inherits from it now
class	WgRadioButton : public WgCheckBox
{

public:
	WgRadioButton();
	~WgRadioButton();

	virtual const char * Type() const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgRadioButton(); };
	
	inline void	AllowUnselecting( bool bAllow ) {m_bAllowUnselecting = bAllow; }	///< Should we allow this radiobutton to be unchecked without any other being checked?
	inline bool AllowUnselecting( ) const { return m_bAllowUnselecting; }

private:

	void			_onCloneContent( const WgWidget * _pOrg );
	WgContainer *	_findRadioGroup();
	void			_unselectRecursively( WgContainer * pParent );
	void			_onStateChanged( WgState oldState, WgState newState );
	
	
	bool			m_bAllowUnselecting;

};




#endif //	WgRadioButton_DOT_H
