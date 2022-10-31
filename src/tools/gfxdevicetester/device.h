#pragma once


#include <wg_gfxdevice.h>
#include <wg_softgfxdevice.h>
#include <wg_streamgfxdevice.h>
#include <wg_streamsurface.h>
#include <wg_gfxstreamplayer.h>
#include <wg_cabi_gfxdevice.h>

using namespace wg;

extern void addGeneratedKernels( SoftGfxDevice * pDevice );


class Device
{
public:

	virtual ~Device() {};

	virtual const char * name() const = 0;

	virtual bool			init(SizeI canvasSize, PixelFormat canvasFormat) = 0;
	virtual void			exit() = 0;

	virtual GfxDevice_p		beginRender() const = 0;
	virtual void			endRender() const = 0;

	virtual GfxDevice_p		gfxDevice() const = 0;
	virtual Surface_p		canvas() const = 0;
};


class SoftwareDevice : public Device
{
public:
	SoftwareDevice()
	{
	}

	const char * name() const
	{
		return m_pName;
	}

	bool init(SizeI canvasSize, PixelFormat canvasFormat)
	{
		m_pCanvas = SoftSurface::create(canvasSize, canvasFormat, SurfaceFlag::Canvas);
		m_pDevice = SoftGfxDevice::create();
		addGeneratedKernels(m_pDevice);
		return true;
	}

	void exit()
	{
		m_pDevice = nullptr;
		m_pCanvas = nullptr;
	}

	GfxDevice_p beginRender() const
	{
		m_pDevice->beginRender();
		m_pDevice->beginCanvasUpdate(m_pCanvas);
		return m_pDevice;
	}

	void endRender() const
	{
		m_pDevice->endCanvasUpdate();
		m_pDevice->endRender();
	}

	GfxDevice_p	gfxDevice() const
	{
		return m_pDevice;
	}

	Surface_p canvas() const
	{
		return m_pCanvas;
	}

private:

	const char * m_pName = { "Software" };

	SoftGfxDevice_p		m_pDevice;
	SoftSurface_p		m_pCanvas;
};



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
		addGeneratedKernels(m_pOutputDevice);
		m_pOutputCanvas = SoftSurface::create(canvasSize, canvasFormat, SurfaceFlag::Canvas);
		m_pOutputDevice->defineCanvas(CanvasRef::Default, m_pOutputCanvas);
		m_pStreamPlayer = GfxStreamPlayer::create(m_pOutputDevice, m_pOutputDevice->surfaceFactory());

		m_pStreamEncoder = GfxStreamEncoder::create( ComponentPtr( m_pStreamPlayer, m_pStreamPlayer->input) );

		m_pStreamDevice = StreamGfxDevice::create(m_pStreamEncoder);
		m_pStreamCanvas = StreamSurface::create(m_pStreamEncoder, canvasSize, canvasFormat, SurfaceFlag::Canvas);
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

	StreamSurface_p		m_pStreamCanvas;
	StreamGfxDevice_p	m_pStreamDevice;


	GfxStreamPlayer_p	m_pStreamPlayer;
	SoftGfxDevice_p		m_pOutputDevice;
	SoftSurface_p		m_pOutputCanvas;
};


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
		addGeneratedKernels(m_pOutputDevice);

		m_pOutputCanvas = SoftSurface::create(canvasSize, canvasFormat, SurfaceFlag::Canvas);
		m_pOutputDevice->defineCanvas(CanvasRef::Default, m_pOutputCanvas);

		auto pFactory = CABISurfaceFactory::create((wg_obj) m_pOutputDevice->surfaceFactory());
		
		m_pCABIDevice = CABIGfxDevice::create( (wg_obj) m_pOutputDevice, pFactory );
		
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


	CABIGfxDevice_p		m_pCABIDevice;

	SoftGfxDevice_p		m_pOutputDevice;
	SoftSurface_p		m_pOutputCanvas;
};
