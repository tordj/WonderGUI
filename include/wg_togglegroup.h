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

#ifndef	WG_TOGGLEGROUP_DOT_H
#define	WG_TOGGLEGROUP_DOT_H

#include <vector>

#ifndef	WG_OBJECT_DOT_H
#	include <wg_object.h>
#endif

#ifndef WG_TOGGLEBUTTON_DOT_H
#	include <wg_togglebutton.h>
#endif


class WgToggleGroup;
typedef	WgStrongPtr<WgToggleGroup,WgObjectPtr>		WgToggleGroupPtr;
typedef	WgWeakPtr<WgToggleGroup,WgObjectWeakPtr>		WgToggleGroupWeakPtr;


class	WgToggleGroup : public WgObject
{
friend class WgToggleButton;
public:
	static WgToggleGroupPtr	Create() { return WgToggleGroupPtr(new WgToggleGroup()); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgToggleGroupPtr	Cast( const WgObjectPtr& pObject );	


	inline bool			RequireSelected() const { return m_bRequireSelected; }
	void				SetRequireSelected(bool bRequire);

	void				Add( const WgToggleButtonPtr& pToggleButton );
	bool				Remove( const WgToggleButtonPtr& pToggleButton );
	void				Clear();
	
	WgToggleButtonPtr	Get( int index );
	int					Size() const;
		
	WgToggleButtonPtr	Selected() const;


protected:
	WgToggleGroup();
	virtual ~WgToggleGroup();

	void			_remove( WgToggleButton * pButton );			// Callback for statebuttons destructor. Notifying ToggleGroup.
	void			_select( WgToggleButton * pButton );			// Callback for statebuttons select. Notifying ToggleGroup.
	bool			_unselect( WgToggleButton * pButton );		// Callback for statebuttons unselect. Notifying ToggleGroup.

private:

	bool						m_bRequireSelected;
	WgToggleButton *				m_pSelected;
	std::vector<WgToggleButton*> m_entries;
};




#endif //	WgToggleButton_DOT_H
