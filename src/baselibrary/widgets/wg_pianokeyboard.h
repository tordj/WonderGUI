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

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{

			Object_p		baggage;
			Surface_p		blackKeys;
			bool			dropTarget = false;
			bool			enabled = true;
			Surface_p		evenWhiteKeys;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			bool			keyFlipOnPress = false;
			std::bitset<7>	keyLayout = 0b1011011;
			std::initializer_list<State>	keyStates;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			int				nbWhiteKeys = 52;
			Surface_p		oddWhiteKeys;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			int				scale = -1;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;

		};

		//.____ Creation __________________________________________

		static PianoKeyboard_p	create() { return PianoKeyboard_p(new PianoKeyboard()); }
		static PianoKeyboard_p	create( const Blueprint& blueprint ) { return PianoKeyboard_p(new PianoKeyboard(blueprint)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance ____________________________________________________

		void		setLayout(int nbWhiteKeys, const std::bitset<7>& blackKeyPositions );

		void		setSurfaces(const Surface_p& pOddWhiteKeys, const Surface_p& pEvenWhiteKeys, const Surface_p& pBlackKeys, const std::initializer_list<State>& states);

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

		//.____ Internal ______________________________________________________

		SizeSPX		_defaultSize(int scale = -1) const override;
		SizeSPX		_minSize(int scale = -1) const override;
		SizeSPX		_maxSize(int scale = -1) const override;


	protected:
		PianoKeyboard();
		PianoKeyboard(const Blueprint& blueprint);
		virtual ~PianoKeyboard();

		void		_render(GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window) override;
		void		_receive(Msg * pMsg) override;
		bool		_alphaTest(const CoordSPX& ofs) override;
//		void		_setState(State state) override;

		void		_requestRenderKey(int keyIdx);
		int			_markTestKey(const CoordSPX& pos);
		RectSPX		_keyRect(int keyIdx);
		void		_setHoveredKey(int keyIdx, uint64_t timestamp);
		void		_setAPIPressed(int keyIdx, bool bPressed);
		void		_setHoverPress(bool bPressed, uint64_t timestamp);

	private:

		static const int c_maxKeys = 12 * 6;

		int				m_nbWhiteKeys = 0;
		std::bitset<7>	m_blackKeyPositions;
		int				m_nbKeys = 0;
		SizeSPX			m_keyboardSourceSize;
		Size			m_defaultKeyboardSize;
		float			m_blackKeyHeight = 0;			// Relative to canvas content height.
		spx				m_blackKeySourceHeight = 0;

		Surface_p		m_pOddWhiteKeys;
		Surface_p		m_pEvenWhiteKeys;
		Surface_p		m_pBlackKeys;

		int				m_stateOfsY[State::IndexAmount];	

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
