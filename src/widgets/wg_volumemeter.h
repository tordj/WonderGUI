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

class WgVolumeMeter;
typedef	WgStrongPtr<WgVolumeMeter,WgWidget_p>		WgVolumeMeter_p;
typedef	WgWeakPtr<WgVolumeMeter,WgWidget_wp>	WgVolumeMeter_wp;

//____ WgVolumeMeter ____________________________________________________________

class WgVolumeMeter : public WgWidget
{
public:
	static WgVolumeMeter_p	create() { return WgVolumeMeter_p(new WgVolumeMeter()); }
	
	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgVolumeMeter_p	cast( const WgObject_p& pObject );
	
	//____ Methods __________________________________________
	
	void			SetLEDColors( WgColor bottomOn, WgColor middleOn, WgColor topOn, 
							   WgColor bottomOff, WgColor middleOff, WgColor topOff );
	void			SetNbLEDs( int bottomSection, int middleSection, int topSection );
	void			SetLEDSpacing( float spacing );
	void			setValue( float peak, float hold );
	void			SetDirection( WgDirection direction );
		
	WgSize			preferredSize() const;
	
protected:
	WgVolumeMeter();
	virtual ~WgVolumeMeter();
	virtual WgWidget* _newOfMyType() const { return new WgVolumeMeter(); };
	
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onStateChanged( WgState oldState );
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
