
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


#include <wg_base.h>

#ifndef WG2_MODE
	#include <wg_msgrouter.h>
	#include <wg_basictextlayout.h>
	#include <wg_basicnumberlayout.h>
	#include <wg_inputhandler.h>
#endif

#include <wg_dummyfont.h>
#include <wg_memstack.h>
#include <wg_mempool.h>
#include <wg_context.h>
#include <wg_textstyle.h>
#include <wg_texttool.h>
#include <wg_textstylemanager.h>
#include <wg_skinslotmanager.h>
#include <wg_bitmapcache.h>

#include <iostream>
#include <algorithm>

namespace wg
{
	const TypeInfo	Base::TYPEINFO = { "Base", nullptr };

	bool						Base::s_bInitialized = false;


	MsgRouter_p					Base::s_pMsgRouter;
	InputHandler_p				Base::s_pInputHandler;
	TextLayout_p				Base::s_pDefaultTextLayout;
	Caret_p						Base::s_pDefaultCaret;
	NumberLayout_p				Base::s_pDefaultNumberLayout;
	TextStyle_p					Base::s_pDefaultTextStyle;
	GfxDevice_p					Base::s_pDefaultGfxDevice;
	SurfaceFactory_p			Base::s_pDefaultSurfaceFactory;
	BitmapCache_p				Base::s_pDefaultBitmapCache;

	MemPool*					Base::s_pPtrPool = nullptr;
	MemStack*					Base::s_pMemStack = nullptr;
	String						Base::s_clipboardText;
	bool						Base::s_bGammaCorrection = true;

	HostBridge*					Base::s_pHostBridge = nullptr;
	int64_t						Base::s_timestamp = 0;
	std::function<void(Error&)>	Base::s_pErrorHandler = nullptr;
	std::vector<Receiver*>		Base::s_updateReceivers;

	std::atomic<unsigned int>	Base::s_objectsCreated;
	std::atomic<unsigned int>	Base::s_objectsDestroyed;

	bool						Base::s_bTrackingObjects = false;

	std::unordered_map<Object*, Base::ObjectInfo>	Base::s_trackedObjects;


	//____ init() __________________________________________________________________

	bool Base::init(HostBridge* pHostBridge, std::function<void(Error&)> errorHandler)
	{
		if (s_bInitialized)
		{
			throwError(ErrorLevel::Error, ErrorCode::IllegalCall, "Call to Base::init() ignored, already initialized.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		s_pHostBridge = pHostBridge;
		s_pErrorHandler = errorHandler;

		s_objectsCreated = 0;
		s_objectsDestroyed = 0;

		s_timestamp = 0;

		TextTool::setDefaultBreakRules();
		HiColor::_initTables();


		TextStyleManager::init();
		SkinSlotManager::init();


		s_pPtrPool = new MemPool( 128, sizeof( WeakPtrHub ) );
		s_pMemStack = new MemStack( 4096 );

		TextStyle::Blueprint textStyleBP;
		textStyleBP.font = DummyFont::create();

		s_pDefaultTextStyle = TextStyle::create( textStyleBP );

#ifndef WG2_MODE
		s_pDefaultCaret = Caret::create();

		s_pDefaultTextLayout = BasicTextLayout::create({});


		s_pDefaultNumberLayout = BasicNumberLayout::create( BasicNumberLayout::Blueprint() );

		s_pMsgRouter = MsgRouter::create();
      	s_pInputHandler = InputHandler::create();
#endif

		s_bInitialized = true;
		return true;
	}

	//____ exit() __________________________________________________________________

	bool Base::exit()
	{

		if (!s_bInitialized)
		{
			throwError(ErrorLevel::SilentError, ErrorCode::IllegalCall, "Call to Base::exit() ignored, not initialized or already exited.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		s_pMsgRouter = nullptr;
		s_pInputHandler = nullptr;
		s_pDefaultTextLayout = nullptr;
		s_pDefaultCaret = nullptr;
		s_pDefaultNumberLayout = nullptr;
		s_pDefaultTextStyle = nullptr;
		s_pDefaultGfxDevice = nullptr;
		s_pDefaultSurfaceFactory = nullptr;

		SkinSlotManager::exit();
		TextStyleManager::exit();

		if (s_pDefaultBitmapCache)
		{
			s_pDefaultBitmapCache->clear();
			s_pDefaultBitmapCache = nullptr;
		}

		// All Objects needs to be destroyed at this point!

		if (s_objectsCreated != s_objectsDestroyed)
			throwError(ErrorLevel::Warning, ErrorCode::SystemIntegrity, "Some objects still alive after wondergui exit. Might cause problems when they go out of scope. Forgotten to clear pointers?\nHint: Enable object tracking to find out which ones.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);


		if( !s_pPtrPool->isEmpty() )
		{
			throwError(ErrorLevel::SilentError, ErrorCode::SystemIntegrity, "Some weak pointers still in use. Can not exit WonderGUI.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		if( !s_pMemStack->isEmpty() )
		{
			throwError(ErrorLevel::Warning, ErrorCode::SystemIntegrity, "Memstack still contains data. Not all allocations have been correctly released.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
		}

		delete s_pPtrPool;
		s_pPtrPool = nullptr;
		delete s_pMemStack;
		s_pMemStack = nullptr;

		bool s_bInitialized = false;
		return true;
	}

	//____ setDefaults() ______________________________________________________

	void Base::setDefaults(const Blueprint& bp)
	{
		assert(s_bInitialized);

		if( bp.caret )
			s_pDefaultCaret				= bp.caret;

		if( bp.gfxDevice)
			s_pDefaultGfxDevice			= bp.gfxDevice;

		if( bp.numberLayout )
			s_pDefaultNumberLayout		= bp.numberLayout;

		if( bp.surfaceFactory )
			s_pDefaultSurfaceFactory	= bp.surfaceFactory;

		if( bp.textLayout )
			s_pDefaultTextLayout		= bp.textLayout;

		if( bp.textStyle )
			s_pDefaultTextStyle			= bp.textStyle;
	}

	//____ msgRouter() ________________________________________________________

	MsgRouter_p Base::msgRouter() 
	{ 
		return s_pMsgRouter; 
	};
	
	//____ inputHandler() ________________________________________________________

	InputHandler_p Base::inputHandler() 
	{ 
		return s_pInputHandler; 
	}

	//____ defaultTextLayout() ________________________________________________________

	TextLayout_p Base::defaultTextLayout() 
	{ 
		return s_pDefaultTextLayout; 
	}

	//____ defaultCaret() ________________________________________________________

	Caret_p Base::defaultCaret() 
	{
		return s_pDefaultCaret; 
	}

	//____ defaultTextStyle() ________________________________________________________

	TextStyle_p Base::defaultTextStyle() 
	{ 
		return s_pDefaultTextStyle; 
	}

	//____ defaultNumberLayout() ________________________________________________________

	NumberLayout_p Base::defaultNumberLayout() 
	{ 
		return s_pDefaultNumberLayout; 
	}

	//____ defaultGfxDevice() ________________________________________________________

	GfxDevice_p Base::defaultGfxDevice() 
	{ 
		return s_pDefaultGfxDevice; 
	}
	
	//____ defaultSurfaceFactory() ________________________________________________________

	SurfaceFactory_p Base::defaultSurfaceFactory() 
	{ 
		return s_pDefaultSurfaceFactory; 
	}

	//____ throwError() _________________________________________________________

	void Base::throwError(ErrorLevel level, ErrorCode code, const char * msg, const Object * pObject, const TypeInfo* pClassType, const char * function, const char * file, int line)
	{
		if (s_pErrorHandler)
		{
			Error	error;

			error.code = code;
			error.message = msg;
			error.pObject = pObject;
			error.classname = pClassType ? pClassType->className : nullptr;
			error.function = function;
			error.file = file;
			error.line = line;
			error.level = level;

			s_pErrorHandler(error);
		}
	}

	//____ startTrackingObjects() _________________________________________________

	void Base::startTrackingObjects()
	{
		s_bTrackingObjects = true;
	}

	//____ stopTrackingObjects() __________________________________________________

	void Base::stopTrackingObjects()
	{
		s_bTrackingObjects = false;
		s_trackedObjects.clear();
	}

	//____ printObjects() ________________________________________________

	void Base::printObjects(std::ostream& stream)
	{

		stream << "Objects created: " << s_objectsCreated << std::endl;
		stream << "Objects destroyed: " << s_objectsDestroyed << std::endl;
		stream << "Objects alive: " << (s_objectsCreated - s_objectsDestroyed) << std::endl;

        if( !s_trackedObjects.empty() )
        {
            stream << std::endl;
            stream << "Tracked objects: " << s_trackedObjects.size() << std::endl;
            stream << "---------------------------------------" << std::endl;

			for (auto& tracked : s_trackedObjects)
			{
				const char* pClassName = tracked.first->typeInfo().className;

				stream << "#" << tracked.second.serialNb << " - " << pClassName << " @ 0x" << tracked.first << " with " << tracked.first->refcount() << " references.";

				if (tracked.second.pFileName)
					stream << " Tracked from " << tracked.second.pFileName << ":" << tracked.second.lineNb;

				stream << std::endl;
			}
		}
	}


	//____ defaultBitmapCache() __________________________________________________

	BitmapCache_p Base::defaultBitmapCache()
	{
		if( s_pDefaultBitmapCache == nullptr )
			s_pDefaultBitmapCache = BitmapCache::create(16*1024*1024);

		return s_pDefaultBitmapCache;
	}

	//____ setClipboardText() ____________________________________________________

	void Base::setClipboardText( const String& text )
	{
		s_clipboardText = text;
		if( s_pHostBridge )
		{
			auto stdString = CharSeq(text).getStdString();
			
			s_pHostBridge->setClipboardText(stdString);
		}
	}

	//____ getClipboardText() ____________________________________________________

	String Base::getClipboardText()
	{
		if( s_pHostBridge )
		{
			auto stdString = s_pHostBridge->getClipboardText();
			
			return String(stdString);
		}
		else
		{
			return s_clipboardText;
		}
	}

	//____ update() ______________________________________________________________

	void Base::update( int64_t timestamp )
	{
		int microPassed = int(timestamp - s_timestamp);
		s_timestamp = timestamp;

		// Update wondergui systems

#ifndef WG2_MODE
        s_pInputHandler->_update(timestamp/1000);
#endif
        SkinSlotManager::update(microPassed/1000);

		// Update widgets
#ifndef WG2_MODE
		for (auto pReceiver : s_updateReceivers)
			pReceiver->_update(microPassed, timestamp);
#endif
	}

	//____ memStackAlloc() ________________________________________________________

	char * Base::memStackAlloc( int bytes )
	{
		return s_pMemStack->allocBytes(bytes);
	}

	//____ memStackFree() ______________________________________________________

	void Base::memStackFree( int bytes )
	{
		return s_pMemStack->releaseBytes(bytes);
	}

	//____ _trackObject() ________________________________________________________

	void Base::_trackObject(Object* pObject, const char* pFileName, int lineNb)
	{
        if( s_bTrackingObjects )
            s_trackedObjects[pObject] = { pFileName, lineNb, s_objectsCreated };
	}

	//____ _startReceiveUpdates() ________________________________________________

	int64_t Base::_startReceiveUpdates(Receiver* pReceiver)
	{
		s_updateReceivers.push_back(pReceiver);
		return s_timestamp;
	}

	//____ _stopReceiveUpdates() _________________________________________________

	void Base::_stopReceiveUpdates(Receiver* pReceiver)
	{
		s_updateReceivers.erase(std::remove(s_updateReceivers.begin(), s_updateReceivers.end(), pReceiver));
	}


	//____ _allocWeakPtrHub() ______________________________________________________

	WeakPtrHub * Base::_allocWeakPtrHub()
	{
		assert( s_bInitialized != 0 );
		WeakPtrHub * pHub = (WeakPtrHub*) s_pPtrPool->allocEntry();

		new (pHub) WeakPtrHub();

		return pHub;
	}

	//____ _freeWeakPtrHub() _______________________________________________________

	void Base::_freeWeakPtrHub( WeakPtrHub * pHub )
	{
		assert( s_bInitialized != 0 );
		pHub->~WeakPtrHub();
		s_pPtrPool->freeEntry( pHub );
	}


} // namespace wg
