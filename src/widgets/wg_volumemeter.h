#ifndef WG_VOLUMEMETER_DOT_H
#define WG_VOLUMEMETER_DOT_H

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
	
	class VolumeMeter;
	typedef	WgStrongPtr<VolumeMeter,Widget_p>		VolumeMeter_p;
	typedef	WgWeakPtr<VolumeMeter,Widget_wp>	VolumeMeter_wp;
	
	//____ VolumeMeter ____________________________________________________________
	
	class VolumeMeter : public Widget
	{
	public:
		static VolumeMeter_p	create() { return VolumeMeter_p(new VolumeMeter()); }
		
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static VolumeMeter_p	cast( const Object_p& pObject );
		
		//____ Methods __________________________________________
		
		void			setLEDColors( Color bottomOn, Color middleOn, Color topOn, 
								   Color bottomOff, Color middleOff, Color topOff );
		void			setNbLEDs( int bottomSection, int middleSection, int topSection );
		void			setLEDSpacing( float spacing );
		void			setValue( float peak, float hold );
		void			setDirection( WgDirection direction );
			
		Size			preferredSize() const;
		
	protected:
		VolumeMeter();
		virtual ~VolumeMeter();
		virtual Widget* _newOfMyType() const { return new VolumeMeter(); };
		
		void			_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_onStateChanged( State oldState );
		void			_onCloneContent( const Widget * _pOrg );
		
		WgDirection		m_direction;
		Color			m_LEDColors[3][2];
		int				m_nSectionLEDs[3];
		int				m_nLEDs;
		float			m_LEDSpacing;
		
		float			m_peak;
		float			m_hold;
		
	};
	
	
	

} // namespace wg
#endif //WG_VOLUMEMETER_DOT_H
