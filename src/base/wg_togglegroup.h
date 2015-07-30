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
typedef	WgStrongPtr<WgToggleGroup,WgObject_p>		WgToggleGroup_p;
typedef	WgWeakPtr<WgToggleGroup,WgObject_wp>		WgToggleGroup_wp;


/**
 * @brief Groups together StateButtons into RadioButtons.
 *
 * A WgToggleGroup object groups together a number of RadioButtons, making
 * them operate as mutually exclusive RadioButtons.
 *
 **/

class	WgToggleGroup : public WgObject
{
friend class WgToggleButton;
public:
	static WgToggleGroup_p	create() { return WgToggleGroup_p(new WgToggleGroup()); }

	bool				isInstanceOf( const char * pClassName ) const;
	const char *		className( void ) const;
	static const char	CLASSNAME[];
	static WgToggleGroup_p	cast( const WgObject_p& pObject );	


	inline bool			requireSelected() const { return m_bRequireSelected; } 	///< @brief Check if group requires a button to be selected at all time.
	void				setRequireSelected(bool bRequire);						///< @brief Set if group should require a button to always be selected.

	void				add( const WgToggleButton_p& pToggleButton );			///< @brief Add widget to this group.
	bool				remove( const WgToggleButton_p& pToggleButton );		///< @brief Remove a widget from this group.
	void				clear();												///< @brief Remove all widgets from this group.
	
	WgToggleButton_p	get( int index );										///< @brief Get widget at specified index in group.
	int					size() const;											///< @brief Get number of widgets in group.
		
	WgToggleButton_p	selected() const;										///< @brief Get the selected widget.


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
