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
typedef	WgSmartPtr<WgVolumeMeter,WgWidgetPtr>		WgVolumeMeterPtr;
typedef	WgWeakPtr<WgVolumeMeter,WgWidgetWeakPtr>	WgVolumeMeterWeakPtr;

//____ WgVolumeMeter ____________________________________________________________

class WgVolumeMeter : public WgWidget
{
public:
	static WgVolumeMeterPtr	Create() { return WgVolumeMeterPtr(new WgVolumeMeter()); }
	
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgVolumeMeterPtr	Cast( const WgObjectPtr& pObject );
	
	//____ Methods __________________________________________
	
	void			SetLEDColors( WgColor bottomOn, WgColor middleOn, WgColor topOn, 
							   WgColor bottomOff, WgColor middleOff, WgColor topOff );
	void			SetNbLEDs( int bottomSection, int middleSection, int topSection );
	void			SetLEDSpacing( float spacing );
	void			SetValue( float peak, float hold );
	void			SetDirection( WgDirection direction );
		
	WgSize			PreferredSize() const;
	
protected:
	WgVolumeMeter();
	virtual ~WgVolumeMeter();
	virtual WgWidget* _newOfMyType() const { return new WgVolumeMeter(); };
	
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