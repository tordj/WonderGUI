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
#pragma once

#include <vector>

#include <wg_object.h>
#include <wg_togglebutton.h>

namespace wg 
{
	
	
	class ToggleGroup;
	typedef	StrongPtr<ToggleGroup>		ToggleGroup_p;
	typedef	WeakPtr<ToggleGroup>		ToggleGroup_wp;
	
	
	/**
	 * @brief Groups together StateButtons into RadioButtons.
	 *
	 * A ToggleGroup object groups together a number of RadioButtons, making
	 * them operate as mutually exclusive RadioButtons.
	 *
	 **/
	
	class	ToggleGroup : public Object
	{
	friend class ToggleButton;
	public:

		//.____ Creation __________________________________________

		static ToggleGroup_p	create() { return ToggleGroup_p(new ToggleGroup()); }
	
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ToggleGroup_p	cast( Object * pObject );	

		//.____ Behavior _____________________________________________
	
		inline bool			requireSelected() const { return m_bRequireSelected; } 	///< @brief Check if group requires a button to be selected at all time.
		void				setRequireSelected(bool bRequire);						///< @brief Set if group should require a button to always be selected.

		//.____ Misc _______________________________________________
	
		void				add( ToggleButton * pToggleButton );				///< @brief Add widget to this group.
		bool				remove( ToggleButton * pToggleButton );			///< @brief Remove a widget from this group.
		void				clear();												///< @brief Remove all widgets from this group.
		
		ToggleButton_p		get( int index );										///< @brief Get widget at specified index in group.
		int					size() const;											///< @brief Get number of widgets in group.
			
		ToggleButton_p		selected() const;										///< @brief Get the selected widget.
	
	
	protected:
		ToggleGroup();
		virtual ~ToggleGroup();
	
		void			_remove( ToggleButton * pButton );			// Callback for statebuttons destructor. Notifying ToggleGroup.
		void			_select( ToggleButton * pButton );			// Callback for statebuttons select. Notifying ToggleGroup.
		bool			_unselect( ToggleButton * pButton );		// Callback for statebuttons unselect. Notifying ToggleGroup.
	
	private:
	
		bool						m_bRequireSelected;
		ToggleButton *				m_pSelected;
		std::vector<ToggleButton*> m_entries;
	};
	
	
	
	

} // namespace wg
#endif //	ToggleButton_DOT_H
