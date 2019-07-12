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

#include <vector>

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
    void            SetSidePadding( float padding ) { m_fSidePadding = padding; }
    void            SetZeroInMiddle(bool mb) { m_bZeroInMiddle = mb; }
    void            EnableFades(bool b) { m_bUseFades = b; }
	WgSize			PreferredPixelSize() const;
	
protected:
	
	void			_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
    void			_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window );
	void			_onCloneContent( const WgWidget * _pOrg );
	bool			_onAlphaTest( const WgCoord& ofs );
	void			_onNewSize( const WgSize& size );

	
	WgDirection		m_direction;
	WgColor			m_LEDColors[3][2];
	int				m_nSectionLEDs[3];
	int				m_nLEDs;
	float			m_LEDSpacing;
	
	int			    m_iPeak;
    
	int			    m_iHold;
    float           m_fPeak;
	
    float           m_fSidePadding;
    int             m_iSidePadding;
    
    bool            m_bZeroInMiddle;
    float           d;
    float           d2;
    
    
    std::vector<float> m_LEDStates;

    bool            m_bUseFades;
};



#endif //WG_VOLUMEMETER_DOT_H
