/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/
#pragma once

#include <wg_object.h>
#include <wg_blob.h>
#include <wg_surface.h>
#include <wg_rootpanel.h>



class WonderApp;
typedef	wg::StrongPtr<WonderApp>	WonderApp_p;
typedef	wg::WeakPtr<WonderApp>		WonderApp_wp;




class WonderApp : public wg::Object
{
public:

	enum class IconType
	{
		Info,
		Warning,
		Error,
		Question
	};
	
	enum class DialogType
	{
		Ok,
		OkCancel,
		YesNo,
		YesNoCancel		
	};
	
	enum class DialogButton
	{
		Undefined,
		Cancel,
		No,
		Yes,
		Ok
	};

	class Window;
	typedef	wg::StrongPtr<Window>	Window_p;
	typedef	wg::WeakPtr<Window>		Window_wp;

	typedef	void* LibId;

	class Window : public wg::Object
	{
	public:

		//.____ Blueprint _________________________________________

		struct Blueprint
		{
			//NOTE: origo is only for initial positioning and is not maintained.
			// The origo-relative positioning of this blueprint will be recalculated
			// to a position in a coordinate system with origo at NorthWest. 

			wg::Size			maxSize;
			wg::Size			minSize;			
			wg::Placement		origo = wg::Placement::Center;
			wg::Coord			pos;
			bool				resizable = true;
			wg::Size			size = { 640,480 };
			std::string			title = "WonderGUI Application";
		};

		//.____ Identification __________________________________________

		const wg::TypeInfo& typeInfo(void) const override;
		const static wg::TypeInfo	TYPEINFO;

		//.___ Content __________________________________________________

		wg::RootPanel_p	rootPanel() const { return m_pRootPanel; }

		//.____ Geometry ______________________________________________________

		wg::Rect		setGeo(const wg::Rect& geo);
		wg::Rect		geo() const { return m_geo; }

		wg::Coord		setPos(wg::Coord pos);
		wg::Coord		pos() const { return m_geo.pos(); }

		wg::Size		setSize(wg::Size size);
		wg::Size		size() const { return m_geo.size(); }

		//.____ Appearance ____________________________________________________

		virtual bool			setTitle(std::string& title) = 0;
		virtual std::string		title() const = 0;

		virtual bool			setIcon(wg::Surface * pIcon) = 0;

	protected:
		Window(wg::RootPanel* pRootPanel, const wg::Rect& geo);
		~Window() {}

		virtual wg::Rect	_updateWindowGeo(const wg::Rect& geo) = 0;

		wg::RootPanel_p		m_pRootPanel;
		wg::Rect			m_geo;

	};


	static WonderApp_p create();

	class Visitor
	{
	public:

		virtual std::vector<std::string> programArguments() const = 0;

		virtual int64_t			time() = 0;					// Time in millisec since any arbitrary time before call to init().

		virtual wg::Blob_p		loadBlob(const std::string& path) = 0;
		virtual wg::Surface_p	loadSurface(const std::string& path, wg::SurfaceFactory* pFactory = nullptr,
											const wg::Surface::Blueprint& blueprint = wg::Surface::Blueprint() ) = 0;
		
		virtual bool			notifyPopup(const std::string& title, const std::string& message, IconType iconType) = 0;
		
		virtual DialogButton	messageBox(const std::string& title, const std::string& message, DialogType dialogType,
										   IconType iconType, DialogButton defaultButton = DialogButton::Undefined ) = 0;

		virtual std::string		inputBox(const std::string& title, const std::string& message, const std::string& defaultInput) = 0;

		virtual std::string		saveFileDialog(	const std::string& title, const std::string& defaultPathAndFile,
												const std::vector<std::string>& filterPatterns,
												const std::string& singleFilterDescription) = 0;
		
		virtual std::string		openFileDialog(	const std::string& title, const std::string& defaultPathAndFile,
												const std::vector<std::string>& filterPatterns,
												const std::string& singleFilterDescription) = 0;

		virtual std::vector<std::string> openMultiFileDialog( const std::string& title, const std::string& defaultPathAndFile,
															const std::vector<std::string>& filterPatterns,
															const std::string& singleFilterDescription ) = 0;

		virtual std::string		selectFolderDialog(const std::string& title, const std::string& defaultPath) = 0;

		virtual Window_p		createWindow(const Window::Blueprint& blueprint) = 0;

		virtual LibId			openLibrary(const std::string& path) = 0;
		virtual void*			loadSymbol(LibId lib, const std::string& symbol) = 0;
		virtual bool			closeLibrary(LibId lib) = 0;


	};

	virtual bool	init(Visitor* pVisitor) = 0;
	virtual bool	update() = 0;
	virtual void	exit() = 0;
	
protected:

	~WonderApp() {}
	
};




