
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
	#include <wg_stdtextmapper.h>
	#include <wg_standardformatter.h>
	#include <wg_inputhandler.h>
#endif

#include <wg_dummyfont.h>
#include <wg_memstack.h>
#include <wg_mempool.h>
#include <wg_context.h>
#include <wg_textstyle.h>
#include <wg_texttool.h>
#include <wg_textstylemanager.h>

#include <iostream>

namespace wg
{
	const TypeInfo	Base::TYPEINFO = { "Base", nullptr };

	Base::Data *				Base::s_pData = 0;

	std::function<void(Error&)>	Base::s_pErrorHandler = nullptr;

	unsigned int				Base::s_objectsCreated = 0;
	unsigned int				Base::s_objectsDestroyed = 0;

	bool						Base::s_bTrackingObjects = false;

	std::unordered_map<Object*, Base::ObjectInfo>	Base::s_trackedObjects;


	//____ init() __________________________________________________________________

	bool Base::init()
	{
		if (s_pData != 0)
		{
			handleError(ErrorSeverity::SilentFail, ErrorCode::IllegalCall, "Call to Base::init() ignored, already initialized.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		TextStyleManager::init();

		s_pData = new Data;

		s_pData->pPtrPool = new MemPool( 128, sizeof( WeakPtrHub ) );
		s_pData->pMemStack = new MemStack( 4096 );

		s_pData->pActiveContext = Context::create();
		s_pData->pDefaultStyle = TextStyle::create();

#ifndef WG2_MODE
		s_pData->pDefaultCaret = Caret::create();

		s_pData->pDefaultTextMapper = StdTextMapper::create();


		s_pData->pDefaultValueFormatter = StandardFormatter::create();

		s_pData->pMsgRouter = MsgRouter::create();
      	s_pData->pInputHandler = InputHandler::create();
#endif

		s_pData->pDefaultStyle = TextStyle::create();
		s_pData->pDefaultStyle->setFont( DummyFont::create() );

		TextTool::setDefaultBreakRules();

		MU::s_scale = 1.f;
		MU::s_qpixPerPoint = 4;
		return true;
	}

	//____ exit() __________________________________________________________________

	bool Base::exit()
	{

		if (s_pData == 0)
		{
			handleError(ErrorSeverity::SilentFail, ErrorCode::IllegalCall, "Call to Base::exit() ignored, not initialized or already exited.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		if( !s_pData->pPtrPool->isEmpty() )
		{
			handleError(ErrorSeverity::SilentFail, ErrorCode::SystemIntegrity, "Some weak pointers still in use. Can not exit WonderGUI.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		if( !s_pData->pMemStack->isEmpty() )
		{
			handleError(ErrorSeverity::Warning, ErrorCode::SystemIntegrity, "Memstack still contains data. Not all allocations have been correctly released.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
		}


		delete s_pData->pPtrPool;
		delete s_pData->pMemStack;
		delete s_pData;
		s_pData = nullptr;

		TextStyleManager::exit();

		if (s_objectsCreated != s_objectsDestroyed)
			handleError(ErrorSeverity::Warning, ErrorCode::SystemIntegrity, "Some objects still alive after wondergui exit. Might cause problems when they go out of scope. Forgotten to clear pointers?\nHint: Enable object tracking to find out which ones.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);

		return true;
	}

	//____ handleError() _________________________________________________________

	void Base::handleError(ErrorSeverity severity, ErrorCode code, const char * msg, const Object * pObject, const TypeInfo& classType, const char * function, const char * file, int line)
	{
		if (s_pErrorHandler)
		{
			Error	error;

			error.code = code;
			error.message = msg;
			error.pObject = pObject;
			error.classname = classType.className;
			error.function = function;
			error.file = file;
			error.line = line;

			s_pErrorHandler(error);
		}
	}

	//____ beginObjectTracking() _________________________________________________

	void Base::beginObjectTracking()
	{
		s_bTrackingObjects = true;
	}

	//____ endObjectTracking() __________________________________________________

	void Base::endObjectTracking()
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


#ifndef WG2_MODE

	//____ msgRouter() _________________________________________________________

	MsgRouter_p	Base::msgRouter()
	{
		return s_pData->pMsgRouter;
	}

	//____ inputHandler() ______________________________________________________

	InputHandler_p Base::inputHandler()
	{
		return s_pData->pInputHandler;
	}

	//____ defaultCaret() ______________________________________________________

	Caret_p Base::defaultCaret()
	{
		assert(s_pData != 0);
		return s_pData->pDefaultCaret;
	}

	//____ setDefaultCaret() ___________________________________________________

	void Base::setDefaultCaret( Caret * pCaret )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultCaret = pCaret;
	}

	//_____ defaultTextMapper() ________________________________________________

	TextMapper_p Base::defaultTextMapper()
	{
		assert(s_pData!=0);
		return s_pData->pDefaultTextMapper;
	}


	//____ setDefaultTextMapper() ___________________________________________________

	void Base::setDefaultTextMapper( TextMapper * pTextMapper )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultTextMapper = pTextMapper;
	}

	//____ defaultValueFormatter() _____________________________________________

	ValueFormatter_p Base::defaultValueFormatter()
	{
		assert(s_pData != 0);
		return s_pData->pDefaultValueFormatter;
	}

	//____ setDefaultValueFormatter() _______________________________________________________

	void Base::setDefaultValueFormatter(ValueFormatter * pFormatter)
	{
		assert(s_pData != 0);
		s_pData->pDefaultValueFormatter = pFormatter;
	}

#endif


	//____ defaultStyle() ______________________________________________________

	TextStyle_p Base::defaultStyle()
	{
		assert(s_pData!=0);
		return s_pData->pDefaultStyle;
	}

	//____ setDefaultStyle() _______________________________________________________

	void Base::setDefaultStyle( TextStyle * pStyle )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultStyle = pStyle;
	}

	//___ setActiveContext() __________________________________________________

	void Base::setActiveContext(Context * pContext)
	{
		assert(s_pData != 0);
		s_pData->pActiveContext = pContext;

        if( pContext )
        {
            MU::s_scale = pContext->scale();
            MU::s_qpixPerPoint = int(pContext->scale() * 4.f);
        }
        else
        {
            MU::s_scale = 1.f;
            MU::s_qpixPerPoint = 4;
        }

	}

	//____ activeContext() ____________________________________________________

	Context_p Base::activeContext()
	{
		assert(s_pData != 0);
		return s_pData->pActiveContext;
	}

	//____ setErrorHandler() _________________________________________________________

	void Base::setErrorHandler(std::function<void(Error&)> handler)
	{
		s_pErrorHandler = handler;
	}

	//____ errorHandler() ____________________________________________________________

	std::function<void(Error&)>	Base::errorHandler()
	{
		return s_pErrorHandler;
	}

	//____ memStackAlloc() ________________________________________________________

	char * Base::memStackAlloc( int bytes )
	{
		assert(s_pData!=0);
		return s_pData->pMemStack->allocBytes(bytes);
	}

	//____ memStackRelease() ______________________________________________________

	void Base::memStackRelease( int bytes )
	{	assert(s_pData!=0);
		return s_pData->pMemStack->releaseBytes(bytes);
	}

	//____ _trackObject() ________________________________________________________

	void Base::_trackObject(Object* pObject, const char* pFileName, int lineNb)
	{
		s_trackedObjects[pObject] = { pFileName, lineNb, s_objectsCreated };
	}

	//____ _allocWeakPtrHub() ______________________________________________________

	WeakPtrHub * Base::_allocWeakPtrHub()
	{
		assert( s_pData != 0 );
		WeakPtrHub * pHub = (WeakPtrHub*) s_pData->pPtrPool->allocEntry();

		new (pHub) WeakPtrHub();

		return pHub;
	}

	//____ _freeWeakPtrHub() _______________________________________________________

	void Base::_freeWeakPtrHub( WeakPtrHub * pHub )
	{
		assert( s_pData != 0 );
		pHub->~WeakPtrHub();
		s_pData->pPtrPool->freeEntry( pHub );
	}


} // namespace wg
