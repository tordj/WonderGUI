#include <wg_object.h>
#include <wg_blob.h>
#include <wg_surface.h>
#include <wg_rootpanel.h>

#pragma once


class WonderApp;
typedef	wg::StrongPtr<WonderApp>	WonderApp_p;
typedef	wg::WeakPtr<WonderApp>		WonderApp_wp;


class WonderApp : public wg::Object
{
public:

	static WonderApp_p create();

	class Visitor
	{
	public:
		virtual wg::RootPanel_p	rootPanel() = 0;
		virtual int64_t			time() = 0;					// Time in millisec since any arbitrary time before call to init().

		virtual wg::Blob_p		loadBlob(const std::string& path) = 0;
		virtual wg::Surface_p	loadSurface(const std::string& path, wg::SurfaceFactory* pFactory = nullptr, const wg::Surface::Blueprint& blueprint = wg::Surface::Blueprint() ) = 0;
	};

	virtual wg::Size	startWindowSize() = 0;

	virtual bool	init(Visitor* pVisitor) = 0;
	virtual bool	update() = 0;
	virtual void	exit() = 0;
};
