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
#ifndef WG_PIANOKEYBOARD_DOT_H
#define WG_PIANOKEYBOARD_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_skin.h>

#include <bitset>

namespace wg
{

	class PianoKeyboard;
	typedef	StrongPtr<PianoKeyboard>		PianoKeyboard_p;
	typedef	WeakPtr<PianoKeyboard>		PianoKeyboard_wp;

	//____ PianoKeyboard __________________________________________________________
	/**
	 * @brief	Widget displaying a piano keyboard.
	 *
	 * PianoKeyboard is a widget that displays an image.
	 *
	 **/

	class PianoKeyboard : public Widget
	{
	public:
		//.____ Creation __________________________________________

		static PianoKeyboard_p	create() { return PianoKeyboard_p(new PianoKeyboard()); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		Size		preferredSize() const override;
		Size		minSize() const override;
		Size		maxSize() const override;


		//.____ Appearance ____________________________________________________

		void		setLayout(int nbWhiteKeys, const std::bitset<7>& blackKeyPositions );

		void		setSurfaces(const Surface_p& pOddWhiteKeys, const Surface_p& pEvenWhiteKeys, const Surface_p& pBlackKeys, std::initializer_list<State> states);

		//.____ Control _______________________________________________________

		void		pressKey(int keyIdx);
		void		releaseKey(int keyIdx);

		bool		isKeyPressed(int keyIdx) const;

		void		setPressedKeys(std::initializer_list<int> pressedKeys);
		void		releaseAllKeys();

		void		setFlipKeyOnPress(bool bFlip);
		bool		flipKeyOnPress() const { return m_bFlipOnPress; }

		void		selectKey(int keyIdx);
		void		unselectKey(int keyIdx);

		bool		isKeySelected(int keyIdx) const;

		void		setSelectedKeys(std::initializer_list<int> selectedKeys);
		void		unselectAllKeys();

		int			userPressedKey() const { return m_bIsHoveredPressed ? m_hoveredKey : -1; }

	protected:
		PianoKeyboard();
		virtual ~PianoKeyboard();
		virtual Widget* _newOfMyType() const override { return new PianoKeyboard(); };

		void		_cloneContent(const Widget * _pOrg) override;
		void		_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		void		_receive(Msg * pMsg) override;
		bool		_alphaTest(const Coord& ofs) override;
//		void		_setState(State state) override;

		void		_requestRenderKey(int keyIdx);
		int			_markTestKey(const Coord& pos);
		Rect		_keyRect(int keyIdx);
		void		_setHoveredKey(int keyIdx, uint64_t timestamp);
		void		_setAPIPressed(int keyIdx, bool bPressed);
		void		_setHoverPress(bool bPressed, uint64_t timestamp);

	private:

		static const int c_maxKeys = 12 * 6;

		int				m_nbWhiteKeys = 0;
		std::bitset<7>	m_blackKeyPositions;
		int				m_nbKeys = 0;
		SizeI			m_keyboardSourceSize;		
		Size			m_preferredKeyboardSize;
		MU				m_blackKeyHeight = 0;
		int				m_blackKeySourceHeight = 0;		// In pixels

		Surface_p		m_pOddWhiteKeys;
		Surface_p		m_pEvenWhiteKeys;
		Surface_p		m_pBlackKeys;

		int				m_stateOfsY[StateEnum_Nb];	

		struct KeyInfo
		{
			State	state;
			int		keyPos;				// 0+, the key position from left to right, counting only white keys.
			bool	isBlack;			// key is the black key following the white key of keyPos.
			bool	isSetToPressed;		// Set when key has been pressed through API-call and should remain pressed after mouse release.
		};

		KeyInfo			m_keyInfo[c_maxKeys];

		int				m_hoveredKey		= -1;
		bool			m_bIsHoveredPressed = false;
		bool			m_bFlipOnPress		= false;


	};


} // namespace wg
#endif //WG_IMAGE_DOT_H
