#pragma once


#include <wg_gfxdevice.h>

#include <wondergui.h>
#include <wondergfxstream.h>
//#include <wg_plugingfxdevice.h>

using namespace wg;
using namespace std;

class Device;
typedef StrongPtr<Device>   Device_p;
typedef WeakPtr<Device>     Device_wp;

class Theme;


class Device : public PackPanel
{
public:

    static Device_p create( const string& name, GfxDevice * pDevice, Surface * pSurface, Theme * pTheme ) { return new Device(name, pDevice, pSurface, pTheme);};

    const string&   name() const { return m_name; }

	GfxDevice_p		beginRender();
	void			endRender();

    GfxDevice_p		gfxDevice() const { return m_pDevice; }
    Surface_p		canvas() const { return m_pCanvas; }
	Surface_p		displaySurface() const { return m_pDisplaySurface; }

	void			setNeedsRedraw() { m_bNeedsRedraw = true; };
	bool			needsRedraw() { return m_bNeedsRedraw; };

protected:
	Device( const string& name, GfxDevice * pDevice, Surface * pSurface, Theme * pTheme );
    virtual ~Device() {};

    
    string          	m_name;
    GfxDevice_p     	m_pDevice;
    Surface_p       	m_pCanvas;
	Surface_p			m_pDisplaySurface;
	SurfaceDisplay_p	m_pDisplay;

	bool				m_bNeedsRedraw = true;
    
};

