
#ifndef WG_SIMPLEVOLUMEMETER_DOT_H
#define WG_SIMPLEVOLUMEMETER_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

namespace wg 
{
	
	class SimpleVolumeMeter;
	typedef	StrongPtr<SimpleVolumeMeter,Widget_p>		SimpleVolumeMeter_p;
	typedef	WeakPtr<SimpleVolumeMeter,Widget_wp>		SimpleVolumeMeter_wp;
	
	//____ SimpleVolumeMeter ____________________________________________________________
	
	class SimpleVolumeMeter : public Widget
	{
	public:
		static SimpleVolumeMeter_p	create() { return SimpleVolumeMeter_p(new SimpleVolumeMeter()); }
		
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static SimpleVolumeMeter_p	cast( const Object_p& pObject );
		
		//____ Methods __________________________________________
	
		void			setColors( Color bottom, Color middle, Color top );
		void			setSections( float bottomFraction, float topFraction );
		void			setHoldHeight( float fraction );
		
		void			setValue( float peak, float hold );
		void			setValue( float leftPeak, float leftHold, float rightPeak, float rightHold );
		
		
		
		Size			preferredSize() const;
		
	protected:
		SimpleVolumeMeter();
		virtual ~SimpleVolumeMeter();
		virtual Widget* _newOfMyType() const { return new SimpleVolumeMeter(); };
		
		void			_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_onCloneContent( const Widget * _pOrg );
		void			_onNewSize( const Size& size );
		void			_onStateChanged( State oldState );
		void			_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
		
		void			_updateSectionPixelHeight();
		void			_renderPeak( GfxDevice * pDevice, int nb, const Rect& _rect, const Rect& _clip );
		void			_renderHold( GfxDevice * pDevice, int nb, const Rect& _rect, const Rect& _clip );
		
		
		Color			m_sectionColors[3];
		float			m_sectionHeight[3];
		int				m_sectionPixelHeight[3];
		float			m_holdHeight;
		bool			m_bStereo;
		float			m_peak[2];
		float			m_hold[2];
		
	};
	
	
	

} // namespace wg
#endif //WG_SIMPLEVOLUMEMETER_DOT_H
