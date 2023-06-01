#pragma once


#include <wg_gfxdevice.h>
#include <wg_softgfxdevice.h>
#include <wg_softkernels_default.h>

#include <wondergfxstream.h>
//#include <wg_plugingfxdevice.h>

using namespace wg;
using namespace std;

class Device;
typedef StrongPtr<Device>   Device_p;
typedef WeakPtr<Device>     Device_wp;


class Device : public Object
{
public:

    static Device_p create( const string& name, GfxDevice * pDevice, Surface * pSurface ) { return new Device(name, pDevice, pSurface);};

    const string&   name() const { return m_name; }

	GfxDevice_p		beginRender() const;
	void			endRender() const;

    GfxDevice_p		gfxDevice() const { return m_pDevice; }
    Surface_p		canvas() const { return m_pCanvas; }

protected:
    Device( const string& name, GfxDevice * pDevice, Surface * pSurface ) : m_name(name), m_pDevice(pDevice), m_pCanvas(pSurface) {};
    virtual ~Device() {};

    
    string          m_name;
    GfxDevice_p     m_pDevice;
    Surface_p       m_pCanvas;

    
};



/*
class StreamToSoftwareDevice : public Device
{
public:
	StreamToSoftwareDevice()
	{
	}

	const char * name() const
	{
		return m_pName;
	}

	bool init(SizeI canvasSize, PixelFormat canvasFormat)
	{

		m_pOutputDevice = SoftGfxDevice::create();
		addDefaultSoftKernels(m_pOutputDevice);
		m_pOutputCanvas = SoftSurface::create( { .canvas = true, .format = canvasFormat, .size = canvasSize });
		m_pOutputDevice->defineCanvas(CanvasRef::Default, m_pOutputCanvas);
		m_pStreamPlayer = GfxStreamPlayer::create(m_pOutputDevice, m_pOutputDevice->surfaceFactory());

		m_pStreamEncoder = GfxStreamFastEncoder::create( ComponentPtr( m_pStreamPlayer, m_pStreamPlayer->input) );

		m_pStreamDevice = GfxStreamDevice::create(m_pStreamEncoder);
		m_pStreamCanvas = GfxStreamSurface::create(m_pStreamEncoder, { .canvas = true, .format = canvasFormat, .size = canvasSize } );
		m_pStreamDevice->defineCanvas(CanvasRef::Default, m_pStreamCanvas);

		return true;
	}

	void exit()
	{
		m_pStreamEncoder = nullptr;
		m_pStreamDevice = nullptr;
		m_pStreamCanvas = nullptr;
		m_pOutputDevice = nullptr;
		m_pOutputCanvas = nullptr;
		m_pStreamPlayer = nullptr;
	}

	GfxDevice_p beginRender() const
	{
		m_pStreamDevice->beginRender();
		m_pStreamDevice->beginCanvasUpdate(CanvasRef::Default);
		return m_pStreamDevice;
	}

	void endRender() const
	{
		m_pStreamDevice->endCanvasUpdate();
		m_pStreamDevice->endRender();
 		m_pStreamEncoder->flush();
		return;
	}

	GfxDevice_p	gfxDevice() const
	{
		return m_pStreamDevice;
	}

	Surface_p canvas() const
	{
		return m_pOutputCanvas;
	}

private:

	const char * m_pName = { "StreamToSoftware" };

	GfxStreamEncoder_p	m_pStreamEncoder;

	GfxStreamSurface_p	m_pStreamCanvas;
	GfxStreamDevice_p	m_pStreamDevice;


	GfxStreamPlayer_p	m_pStreamPlayer;
	SoftGfxDevice_p		m_pOutputDevice;
	SoftSurface_p		m_pOutputCanvas;
};

/*
class CABIToSoftwareDevice : public Device
{
public:
	CABIToSoftwareDevice()
	{
	}

	const char * name() const
	{
		return m_pName;
	}

	bool init(SizeI canvasSize, PixelFormat canvasFormat)
	{

		m_pOutputDevice = SoftGfxDevice::create();
		addDefaultSoftKernels(m_pOutputDevice);

		m_pOutputCanvas = SoftSurface::create(canvasSize, canvasFormat, SurfaceFlag::Canvas);
		m_pOutputDevice->defineCanvas(CanvasRef::Default, m_pOutputCanvas);

		auto pFactory = PluginSurfaceFactory::create((wg_obj) m_pOutputDevice->surfaceFactory());
		
		m_pCABIDevice = PluginGfxDevice::create( (wg_obj) m_pOutputDevice, pFactory );
		
		return true;
	}

	void exit()
	{
		m_pOutputDevice = nullptr;
		m_pOutputCanvas = nullptr;
		m_pCABIDevice = nullptr;
	}

	GfxDevice_p beginRender() const
	{
		m_pCABIDevice->beginRender();
		m_pCABIDevice->beginCanvasUpdate(CanvasRef::Default);
		return m_pCABIDevice;
	}

	void endRender() const
	{
		m_pCABIDevice->endCanvasUpdate();
		m_pCABIDevice->endRender();
		return;
	}

	GfxDevice_p	gfxDevice() const
	{
		return m_pCABIDevice;
	}

	Surface_p canvas() const
	{
		return m_pOutputCanvas;
	}

private:

	const char * m_pName = { "CABIToSoftware" };


	PluginGfxDevice_p		m_pCABIDevice;

	SoftGfxDevice_p		m_pOutputDevice;
	SoftSurface_p		m_pOutputCanvas;
};
*/

