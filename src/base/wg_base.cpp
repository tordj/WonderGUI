
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
#include <wg_finalizer.h>
#include <wg_standardformatter.h>


namespace wg 
{
	
	Base::Data *			Base::s_pData = 0;
	
	
	//____ init() __________________________________________________________________
	
	void Base::init()
	{
		assert( s_pData == 0 );

		TextStyleManager::init();

		s_pData = new Data;
		
		s_pData->pPtrPool = new MemPool( 128, sizeof( WeakPtrHub ) );
		s_pData->pMemStack = new MemStack( 4096 );
	
		StdTextMapper_p pTextMapper = StdTextMapper::create();
		pTextMapper->setCaret( Caret::create() );
		s_pData->pDefaultTextMapper = pTextMapper;

		s_pData->pDefaultStyle = TextStyle::create();
		
		s_pData->pDefaultValueFormatter = StandardFormatter::create();

		s_pData->pMsgRouter = MsgRouter::create();
		s_pData->pInputHandler = InputHandler::create();
	
		s_pData->pDefaultStyle = TextStyle::create();
		s_pData->pDefaultStyle->setFont( DummyFont::create() );
	
	
		TextTool::setDefaultBreakRules();
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
	
		s_pData->pDefaultTextMapper = 0;
		s_pData->pDefaultStyle = 0;
		s_pData->pDefaultValueFormatter = 0;
	
		delete s_pData->pPtrPool;
		delete s_pData->pMemStack;
		delete s_pData;
		s_pData = 0;
		
		TextStyleManager::exit();
		return 0;
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
		
	//____ setDefaultTextMapper() ___________________________________________________
	
	void Base::setDefaultTextMapper( TextMapper * pTextMapper )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultTextMapper = pTextMapper;	
	}
	
	//____ setDefaultStyle() _______________________________________________________
	
	void Base::setDefaultStyle( TextStyle * pStyle )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultStyle = pStyle;	
	}
		
	//____ setDefaultValueFormatter() _______________________________________________________

	void Base::setDefaultValueFormatter(ValueFormatter * pFormatter)
	{
		assert(s_pData != 0);
		s_pData->pDefaultValueFormatter = pFormatter;
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
