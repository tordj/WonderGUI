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
#pragma once

#include <wg_capsule.h>

//TODO: Need to support skin padding.

namespace wg
{

	class SizeCapsule;
	typedef	StrongPtr<SizeCapsule>		SizeCapsule_p;
	typedef	WeakPtr<SizeCapsule>		SizeCapsule_wp;

	/**
	* @brief	A widget that controls the size of a single child.
	*
	* The SizeCapsule is used to change the size of a widget. This is done either by imposing
	* limits on its minimum and maximum size or by setting a specific preferred size.
	*
	* Width and height can be controlled separately
	* The SizeCapsule overrides the min, max and preferred size reported by its child, thus
	* affecting its geometry. Exactly how the geometry is affected depends on the parent widget.
	*
	*/

	class SizeCapsule : public Capsule
	{
	public:
		//.____ Creation __________________________________________

		static SizeCapsule_p	create() { return SizeCapsule_p(new SizeCapsule()); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];

		//.____ Geometry _________________________________________________

		bool	setSizes( Size min, Size preferred, Size max );

		void	setPreferredSize( Size size );
		void	setMinSize( Size size );
		void	setMaxSize( Size size );


	protected:
		SizeCapsule();
		virtual ~SizeCapsule();
		virtual Widget* _newOfMyType() const override { return new SizeCapsule(); };

		SizeI	_preferredSize() const override;
		SizeI	_minSize() const override;
		SizeI	_maxSize() const override;

		int		_matchingHeight(int width) const override;
		int		_matchingWidth(int height) const override;

	private:

		SizeI			m_min;
		SizeI			m_max = { INT_MAX,INT_MAX };
		SizeI			m_preferred = { -1, -1 };
	};


} // namespace wg
#endif //WG_SIZECAPSULE_DOT_H
