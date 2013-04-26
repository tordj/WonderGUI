
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

//____ WgSimpleVolumeMeter ____________________________________________________________

class WgSimpleVolumeMeter : public WgWidget
{
public:
	WgSimpleVolumeMeter();
	virtual ~WgSimpleVolumeMeter();
	
	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgSimpleVolumeMeter(); };
	
	//____ Methods __________________________________________

	void			SetColors( WgColor bottom, WgColor middle, WgColor top );
	void			SetSections( float bottomFraction, float topFraction );
	void			SetHoldHeight( float fraction );
	
	void			SetValue( float peak, float hold );
	void			SetValue( float leftPeak, float leftHold, float rightPeak, float rightHold );
	
	
	
	WgSize			PreferredSize() const;
	
protected:
	
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onCloneContent( const WgWidget * _pOrg );
	bool			_onAlphaTest( const WgCoord& ofs );
	void			_onNewSize( const WgSize& size );
	
	void			_updateSectionPixelHeight();
	void			_renderPeak( WgGfxDevice * pDevice, int nb, const WgRect& _rect, const WgRect& _clip );
	void			_renderHold( WgGfxDevice * pDevice, int nb, const WgRect& _rect, const WgRect& _clip );
	
	
	WgColor			m_sectionColors[3];
	float			m_sectionHeight[3];
	int				m_sectionPixelHeight[3];
	float			m_holdHeight;
	bool			m_bStereo;
	float			m_peak[2];
	float			m_hold[2];
	
};



#endif //WG_SIMPLEVOLUMEMETER_DOT_H