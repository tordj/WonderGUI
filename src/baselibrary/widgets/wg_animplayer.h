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

#ifndef	WG_ANIMPLAYER_DOT_H
#define	WG_ANIMPLAYER_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_canimframes.h>

namespace wg
{

	class AnimPlayer;
	typedef	StrongPtr<AnimPlayer>		AnimPlayer_p;
	typedef	WeakPtr<AnimPlayer>	AnimPlayer_wp;

	//____ AnimPlayer ____________________________________________________________
	/**
	* @brief A widget for playing simple animations.
	*
	**/

	class AnimPlayer : public Widget, protected CAnimFrames::Holder
	{
	public:

		//.____ Creation __________________________________________

		static AnimPlayer_p	create() { return AnimPlayer_p(new AnimPlayer()); }

		//.____ Components _______________________________________

		CAnimFrames		frames;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control __________________________________________

		void			setPlayMode(PlayMode mode);
		PlayMode		playMode() const { return m_playMode; }

		bool			setPlayPos( int ticks );						/// Position in ticks for next update.
		bool			setPlayPosFraction( float fraction );			/// Position in fractions of duration.
		int				playPos() const { return m_playPos; }										/// Returns play position in ticks.

		bool			rewind( int ticks );
		bool			fastForward( int ticks );

		int				cycleDuration() const { return m_cycleDuration; }								/// Returns duration of one cycle of animation.

		void			setSpeed( float speed );
		float			speed() const { return m_speed; }

		bool			play();
		void			stop();
		bool			isPlaying() const { return m_bPlaying; };

		//.____ Behavior ______________________________________________________

		inline void		setMarkAlpha(int markAlpha);
		inline int		markAlpha() const;


	protected:
		AnimPlayer();
		virtual ~AnimPlayer();
		virtual Widget* _newOfMyType() const override { return new AnimPlayer(); };

		void			_cloneContent( const Widget * _pOrg ) override;

		SizeSPX			_preferredSize(int scale = -1) const override;

		void			_update(int microPassed, int64_t microsecTimestamp) override;
		void			_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void			_refresh() override;
		bool			_alphaTest( const CoordSPX& ofs ) override;
		void			_setState( State state ) override;

		bool			_playPosUpdated();
		int				_playPosToTimestamp(int playPos) const;

		void			_didSetAnimFrameSize(CAnimFrames* pComponent) override;
		void			_didSetAnimSurface(CAnimFrames* pComponent) override;
		void			_didAddEntries(AnimFrame* pEntry, int nb) override;
		void			_didMoveEntries(AnimFrame* pFrom, AnimFrame* pTo, int nb) override;
		void			_willEraseEntries(AnimFrame* pEntry, int nb) override;
		Object*			_object() override;


	private:

		PlayMode	m_playMode	= PlayMode::Forward;
		int			m_playPos	= 0;
		int			m_cycleDuration = 0;
		int			m_frameTimestamp = -1;		// Timestamp for last rendered frame.

		bool		m_bPlaying	= false;
		float		m_speed		= 1.f;

		int			m_animMarkAlpha = 1;
	};

	void AnimPlayer::setMarkAlpha(int markAlpha)
	{
		m_animMarkAlpha = markAlpha;
	}

	int AnimPlayer::markAlpha() const
	{
		return m_animMarkAlpha;
	}

} // namespace wg
#endif //	WG_ANIMPLAYER_DOT_H
