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

#ifndef	WG_SIZESCALER_DOT_H
#define	WG_SIZESCALER_DOT_H

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

namespace wg 
{
	
	class SizeScaler;
	
	typedef	StrongPtr<SizeScaler,Object_p>	SizeScaler_p;
	
	
	class Scalable
	{
		friend class SizeScaler;
	protected:
		Scalable() {}
		virtual ~Scalable();
		virtual void _onScaleChanged() = 0;
		
		SizeScaler_p m_pScaler;
	};
	
	
	class SizeScaler : public Object
	{
		friend class Scalable;
	public:
		static SizeScaler_p create();
		static SizeScaler_p create( float scale );
		static SizeScaler_p create( float scaleX, float scaleY );
	
		virtual ~SizeScaler() {}
	
		void	setScale( float scale );
		void	setScale( float scaleX, float scaleY );
		void	setScaleX( float scaleX );
		void	setScaleY( float scaleY );
	
		inline float	scaleX() const { return m_scaleX; }
		inline float	scaleY() const { return m_scaleY; }
	
		void	addItem( Scalable * pScalable );
		void	removeItem( Scalable * pScalable );
		void	removeAllItems();
	
	private:
		SizeScaler( float scaleX, float scaleY );
		void	_removeDeadItem( Scalable * pScalable );
	
		class Node : public Link
		{
		public:
			LINK_METHODS(Node);
			Scalable * m_pScalable;
		};
	
		Chain<Node>	m_nodes;
	
		float	m_scaleX;
		float	m_scaleY;
	};
	
	

} // namespace wg
#endif //WG_SIZESCALER_DOT_H
