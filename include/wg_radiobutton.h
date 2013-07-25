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

class WgRadioButton;
typedef	WgSmartPtr<WgRadioButton,WgCheckBoxPtr>	WgRadioButtonPtr;
typedef	WgWeakPtr<WgRadioButton,WgCheckBoxWeakPtr>	WgRadioButtonWeakPtr;


class	WgRadioButton : public WgCheckBox
{

public:
	static WgRadioButtonPtr	Create() { return WgRadioButtonPtr(new WgRadioButton()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgRadioButtonPtr	Cast( const WgObjectPtr& pObject );
	
	inline void	AllowUnselecting( bool bAllow ) {m_bAllowUnselecting = bAllow; }	///< Should we allow this radiobutton to be unchecked without any other being checked?
	inline bool AllowUnselecting( ) const { return m_bAllowUnselecting; }

protected:
	WgRadioButton();
	virtual ~WgRadioButton();
	virtual WgWidget* _newOfMyType() const { return new WgRadioButton(); };


private:

	void			_onCloneContent( const WgWidget * _pOrg );
	WgContainer *	_findRadioGroup();
	void			_unselectRecursively( WgContainer * pParent );
	void			_onStateChanged( WgState oldState, WgState newState );
	
	
	bool			m_bAllowUnselecting;

};




#endif //	WgRadioButton_DOT_H
