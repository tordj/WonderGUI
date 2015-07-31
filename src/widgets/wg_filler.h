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
#ifndef WG_FILLER_DOT_H
#define WG_FILLER_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

namespace wg 
{
	
	class WgFiller;
	typedef	WgStrongPtr<WgFiller,WgWidget_p>		WgFiller_p;
	typedef	WgWeakPtr<WgFiller,WgWidget_wp>	WgFiller_wp;
	
	//____ WgFiller ____________________________________________________________
	/**
	 * @brief Simplest kind of widget.
	 *
	 * WgFiller is the simplest kind of widget that displays nothing but its skin
	 * and does nothing but posting the normal mouse messages that every widget does.
	 *
	 * It is typically used as backgrounds and for filling out empty spaces in
	 * organized layouts.
	 *
	 **/
	
	
	class WgFiller : public WgWidget
	{
	public:
		static WgFiller_p	create() { return WgFiller_p(new WgFiller()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgFiller_p	cast( const WgObject_p& pObject );
	
		void	setPreferredSize( const WgSize& size );
		WgSize	preferredSize() const;
	
	protected:
		WgFiller();
		virtual ~WgFiller();
		virtual WgWidget* _newOfMyType() const { return new WgFiller(); };
	
		void	_onCloneContent( const WgWidget * _pOrg );
	
	private:
		WgSize			m_preferredSize;
	};
	
	

} // namespace wg
#endif //WG_FILLER_DOT_H
