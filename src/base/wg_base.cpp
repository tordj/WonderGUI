
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
#include <wg_msgrouter.h>
#include <wg_dummyfont.h>
#include <wg_memstack.h>
#include <wg_stdtextmapper.h>
#include <wg_mempool.h>
#include <wg_standardformatter.h>
#include <wg_inputhandler.h>
#include <wg_context.h>
#include <wg_textstyle.h>
#include <wg_texttool.h>
#include <wg_textstylemanager.h>

namespace wg
{

	Base::Data *			Base::s_pData = 0;

	std::function<void(Error&)>	Base::s_pErrorHandler = nullptr;

	//____ init() __________________________________________________________________

	void Base::init()
	{
		assert( s_pData == 0 );

		TextStyleManager::init();

		s_pData = new Data;

		s_pData->pPtrPool = new MemPool( 128, sizeof( WeakPtrHub ) );
		s_pData->pMemStack = new MemStack( 4096 );

		s_pData->pActiveContext = Context::create();

		s_pData->pDefaultCaret = Caret::create();

		s_pData->pDefaultTextMapper = StdTextMapper::create();

		s_pData->pDefaultStyle = TextStyle::create();

		s_pData->pDefaultValueFormatter = StandardFormatter::create();

		s_pData->pMsgRouter = MsgRouter::create();
		s_pData->pInputHandler = InputHandler::create();

		s_pData->pDefaultStyle = TextStyle::create();
		s_pData->pDefaultStyle->setFont( DummyFont::create() );

		TextTool::setDefaultBreakRules();

		MU::s_scale = 1.f;
		MU::s_qpixPerPoint = 4;

	}

	//____ exit() __________________________________________________________________

	int Base::exit()
	{

		if( s_pData == 0 )
			return -1;					// Base already exited or not intialized.

		if( !s_pData->pPtrPool->isEmpty() )
			return -2;					// There are weak pointers left.

		if( !s_pData->pMemStack->isEmpty() )
			return -3;					// There is data left in memstack.

		s_pData->pDefaultCaret = nullptr;
		s_pData->pDefaultTextMapper = nullptr;
		s_pData->pDefaultStyle = nullptr;
		s_pData->pDefaultValueFormatter = nullptr;

		delete s_pData->pPtrPool;
		delete s_pData->pMemStack;
		delete s_pData;
		s_pData = nullptr;

		TextStyleManager::exit();
		return 0;
	}

	//____ handleError() _________________________________________________________

	void Base::handleError(ErrorCode code, const char * msg, const Object * pObject, const TypeInfo& classType, const char * function, const char * file, int line)
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

	//____ defaultValueFormatter() _____________________________________________

	ValueFormatter_p Base::defaultValueFormatter()
	{
		assert(s_pData != 0);
		return s_pData->pDefaultValueFormatter;
	}

	//___ setActiveContext() __________________________________________________

	void Base::setActiveContext(Context * pContext)
	{
		assert(s_pData != 0);
		s_pData->pActiveContext = pContext;

		MU::s_scale = pContext->scale();
		MU::s_qpixPerPoint = int(pContext->scale() * 4.f);
	}

	//____ activeContext() ____________________________________________________

	Context_p Base::activeContext()
	{
		assert(s_pData != 0);
		return s_pData->pActiveContext;
	}


	//____ setDefaultValueFormatter() _______________________________________________________

	void Base::setDefaultValueFormatter(ValueFormatter * pFormatter)
	{
		assert(s_pData != 0);
		s_pData->pDefaultValueFormatter = pFormatter;
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
		return s_pData->pMemStack->alloc(bytes);
	}

	//____ memStackRelease() ______________________________________________________

	void Base::memStackRelease( int bytes )
	{	assert(s_pData!=0);
		return s_pData->pMemStack->release(bytes);
	}

} // namespace wg
