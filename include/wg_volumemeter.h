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

//____ WgVolumeMeter ____________________________________________________________

class WgVolumeMeter : public WgWidget
{
public:
	WgVolumeMeter();
	virtual ~WgVolumeMeter();
	
	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgVolumeMeter(); };
	
	//____ Methods __________________________________________
	
	void			SetLEDColors( WgColor bottomOn, WgColor middleOn, WgColor topOn, 
							   WgColor bottomOff, WgColor middleOff, WgColor topOff );
	void			SetNbLEDs( int bottomSection, int middleSection, int topSection );
	void			SetLEDSpacing( float spacing );
	void			SetValue( float peak, float hold );
	void			SetDirection( WgDirection direction );
		
	WgSize			PreferredSize() const;
	
protected:
	
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onStateChanged( WgState oldState, WgState newState );
	void			_onCloneContent( const WgWidget * _pOrg );
	
	WgDirection		m_direction;
	WgColor			m_LEDColors[3][2];
	int				m_nSectionLEDs[3];
	int				m_nLEDs;
	float			m_LEDSpacing;
	
	float			m_peak;
	float			m_hold;
	
};



#endif //WG_VOLUMEMETER_DOT_H