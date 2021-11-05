#include <wondergui.h>

#pragma once

class AppVisitor
{
public:
	virtual wg::RootPanel_p	rootPanel() = 0;
	virtual int64_t			time() = 0;					// Time in millisec since any arbitrary time before call to init().

	virtual wg::Blob_p		loadBlob(const char* pPath) = 0;
	virtual wg::Surface_p	loadSurface(const char* pPath, wg::SurfaceFactory* pFactory = nullptr, const wg::Surface::Blueprint& blueprint = wg::Surface::Blueprint() ) = 0;
};

class SimpleApp
{
public:

	SimpleApp() {};
	virtual ~SimpleApp() {};

	virtual wg::Size	startWindowSize() = 0;

	virtual bool	init(AppVisitor* pVisitor) = 0;
	virtual bool	update() = 0;
	virtual void	exit() = 0;
};