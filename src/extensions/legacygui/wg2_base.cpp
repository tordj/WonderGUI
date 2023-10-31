
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


#include <wg2_base.h>
#include <wg_gfxbase.h>
#include <wg_textstyle.h>
#include <wg_texttool.h>
#include <wg_textstylemanager.h>
#include <wg_memstack.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_freetypefont.h>
#include <wg_dummyfont.h>

#include <wg2_smartptr.h>


WgBase::Data *			WgBase::s_pData = 0;
int WgBase::s_iSoftubeNumberOfInstances = 0;

wg::HostBridge *		WgBase::s_pHostBridge = nullptr;

//____ Init() __________________________________________________________________

void WgBase::Init( wg::HostBridge * pHostBridge)
{
	s_iSoftubeNumberOfInstances++;
	if(s_iSoftubeNumberOfInstances != 1)
		return;

	wg::GfxBase::init();

	s_pHostBridge = pHostBridge;
	
	wg::TextTool::setDefaultBreakRules();


	wg::TextStyleManager::init();

	wg::TextStyle::Blueprint textStyleBP;
	textStyleBP.font = wg::DummyFont::create();

	wg::TextStyle::s_pDefaultStyle = wg::TextStyle::create( textStyleBP );
	
	assert( s_pData == 0 );
	s_pData = new Data;

	s_pData->pDefaultCursor = 0;

	s_pData->doubleClickTimeTreshold 		= 250;
	s_pData->doubleClickDistanceTreshold 	= 2;

	s_pData->buttonRepeatDelay 	= 300;
	s_pData->buttonRepeatRate 	= 200;

	s_pData->keyRepeatDelay 	= 300;
	s_pData->keyRepeatRate 		= 150;

	wg::TextTool::setDefaultBreakRules();
}

//____ Exit() __________________________________________________________________

void WgBase::Exit()
{
	s_iSoftubeNumberOfInstances--;
	if(s_iSoftubeNumberOfInstances != 0)
		return;

	assert( s_pData != 0 );

	delete s_pData;
	s_pData = 0;
	
	wg::TextStyle::s_pDefaultStyle = nullptr;

	wg::GfxBase::exit();
}

//____ SetDefaultCursor() ___________________________________________________

void WgBase::SetDefaultCursor( WgCursor * pCursor )
{
	assert( s_pData != 0 );
	s_pData->pDefaultCursor = pCursor;
}

//____ SetDoubleClickTresholds() _______________________________________________

bool WgBase::SetDoubleClickTresholds( int time, int distance )
{
	assert( s_pData != 0 );
	if( time <= 0 || distance <= 0 )
		return false;

	s_pData->doubleClickTimeTreshold		= time;
	s_pData->doubleClickDistanceTreshold	= distance;
	return true;
}

//____ SetMouseButtonRepeat() ______________________________________________________

bool WgBase::SetMouseButtonRepeat( int delay, int rate )
{
	assert( s_pData != 0 );
	if( delay <= 0 || rate <= 0 )
		return false;

	s_pData->buttonRepeatDelay	= delay;
	s_pData->buttonRepeatRate	= rate;
	return true;
}

//____ SetKeyRepeat() _________________________________________________________

bool WgBase::SetKeyRepeat( int delay, int rate )
{
	assert( s_pData != 0 );
	if( delay <= 0 || rate <= 0 )
		return false;

	s_pData->keyRepeatDelay	= delay;
	s_pData->keyRepeatRate	= rate;
	return true;
}

//____ MapKey() _______________________________________________________________

void WgBase::MapKey( WgKey translated_keycode, int native_keycode )
{
	assert( s_pData != 0 );
	s_pData->keycodeMap[native_keycode] = translated_keycode;
}


//____ UnmapKey() _____________________________________________________________

void WgBase::UnmapKey( WgKey translated_keycode )
{
	assert( s_pData != 0 );
	std::map<int,WgKey>::iterator it = s_pData->keycodeMap.begin();

	while( it != s_pData->keycodeMap.end() )
	{
		if( it->second == translated_keycode )
		{
			std::map<int,WgKey>::iterator it2 = it++;
			s_pData->keycodeMap.erase(it2);
		}
		else
			++it;
	}
}

//____ ClearKeyMap() __________________________________________________________

void WgBase::ClearKeyMap()
{
	assert( s_pData != 0 );
	s_pData->keycodeMap.clear();
}

//____ TranslateKey() __________________________________________________________

WgKey WgBase::TranslateKey( int native_keycode )
{
	assert( s_pData != 0 );
	std::map<int,WgKey>::iterator it = s_pData->keycodeMap.find(native_keycode);
	if( it != s_pData->keycodeMap.end() )
		return  it->second;
	else
		return WG_KEY_UNMAPPED;
}

//____ setDefaultStyle() ______________________________________________________

void WgBase::setDefaultStyle( wg::TextStyle* pStyle )
{
	wg::TextStyle::s_pDefaultStyle = pStyle;
}
