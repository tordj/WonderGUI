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

#include <vector>

#ifndef WG_SIZECAPSULE_DOT_H
#define WG_SIZECAPSULE_DOT_H

#ifndef WG_CAPSULE_DOT_H
#	include <wg_capsule.h>
#endif

#ifndef WG_SIZESCALER_DOT_H
#	include <wg_sizescaler.h>
#endif

namespace wg 
{
	
	class SizeCapsule;
	typedef	WgStrongPtr<SizeCapsule,Capsule_p>		SizeCapsule_p;
	typedef	WgWeakPtr<SizeCapsule,Capsule_p>		SizeCapsule_wp;
	
	class SizeCapsule : public Capsule, public Scalable
	{
	public:
		static SizeCapsule_p	create() { return SizeCapsule_p(new SizeCapsule()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static SizeCapsule_p	cast( const Object_p& pObject );
	
		void	setSizes( Size min, Size preferred, Size max );
	
		void	setPreferredSize( Size size );
		void	setMinSize( Size size );
		void	setMaxSize( Size size );
	
		Size	preferredSize() const;
		Size	minSize() const;
		Size	maxSize() const;
	
		int		matchingHeight( int width ) const;
		int		matchingWidth( int height ) const;
	
	protected:
		SizeCapsule();
		virtual ~SizeCapsule();
		virtual Widget* _newOfMyType() const { return new SizeCapsule(); };
	
	private:
		void	_onScaleChanged();
	
		Size			m_min;
		Size			m_max;
		Size			m_preferred;
	};
	

} // namespace wg
#endif //WG_SIZECAPSULE_DOT_H
