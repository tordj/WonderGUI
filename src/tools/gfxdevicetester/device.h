#pragma once


#include <wg_gfxdevice.h>
#include <wg_softgfxdevice.h>
#include <wg_streamgfxdevice.h>
#include <wg_streamsurface.h>
#include <wg_gfxstreamplug.h>
#include <wg_gfxstreamplayer.h>

using namespace wg;

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
		m_pStreamBuffer = GfxStreamPlug::create();

		m_pOutputDevice = SoftGfxDevice::create();
		m_pStreamPlayer = GfxStreamPlayer::create(m_pStreamBuffer->output[0], m_pOutputDevice, m_pOutputDevice->surfaceFactory());
		m_pStreamBuffer->openOutput(0);

		m_pStreamDevice = StreamGfxDevice::create(m_pStreamBuffer->input);
		m_pStreamCanvas = StreamSurface::create(m_pStreamBuffer->input, canvasSize, canvasFormat);

		return true;
	}

	void exit()
	{
		m_pStreamBuffer = nullptr;
		m_pStreamDevice = nullptr;
		m_pStreamCanvas = nullptr;
		m_pOutputDevice = nullptr;
		m_pStreamPlayer = nullptr;
	}

	GfxDevice_p beginRender() const
	{
		m_pStreamDevice->beginRender();
		m_pStreamDevice->beginCanvasUpdate(m_pStreamCanvas);
		return m_pStreamDevice;
	}

	void endRender() const
	{
		m_pStreamDevice->endCanvasUpdate();
		m_pStreamDevice->endRender();
		m_pStreamPlayer->playAll();
		return;
	}

	GfxDevice_p	gfxDevice() const
	{
		return m_pStreamDevice;
	}

	Surface_p canvas() const
	{
		return m_pOutputDevice->canvas().pSurface;
	}

private:

	const char * m_pName = { "StreamToSoftware" };

	GfxStreamPlug_p		m_pStreamBuffer;

	StreamSurface_p		m_pStreamCanvas;
	StreamGfxDevice_p	m_pStreamDevice;


	GfxStreamPlayer_p	m_pStreamPlayer;
	SoftGfxDevice_p		m_pOutputDevice;
	SoftSurface_p		m_pOutputCanvas;
};
