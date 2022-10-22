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

#ifndef	WG_BASE_DOT_H
#define	WG_BASE_DOT_H
#pragma once

#include <assert.h>

#include <wg_strongptr.h>
#include <wg_types.h>
#include <wg_hostbridge.h>
#include <wg_string.h>

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <atomic>


//#define WG2_MODE

namespace wg
{
	class Font;
	class MemPool;
	class WeakPtrHub;
	class MemStack;
	class MsgRouter;
	class NumberLayout;
	class InputHandler;
	class TextLayout;
	class Caret;
	class TextStyle;
	class Object;
	class Receiver;
	class BitmapCache;
	class GfxDevice;
	class SurfaceFactory;

	typedef	StrongPtr<MsgRouter>		MsgRouter_p;
	typedef	StrongPtr<NumberLayout>		NumberLayout_p;
	typedef	StrongPtr<InputHandler>		InputHandler_p;
	typedef	StrongPtr<TextLayout>		TextLayout_p;
	typedef	StrongPtr<Caret>			Caret_p;
	typedef	StrongPtr<TextStyle>		TextStyle_p;
	typedef	StrongPtr<BitmapCache>		BitmapCache_p;
	typedef	StrongPtr<GfxDevice>		GfxDevice_p;
	typedef	StrongPtr<SurfaceFactory>	SurfaceFactory_p;


	class Error
	{
	public:
		ErrorLevel		level;
		ErrorCode		code;
		std::string		message;
		const Object *	pObject;
		const char *	classname;
		const char *	function;
		const char *	file;
		int				line;
	};


	/**
	 * @brief	Static base class for WonderGUI.
	 *
	 * A static base class for WonderGUI that handles initialization and general
	 * housekeeping and resource allocation.
	 *
	 * The first thing that you need to do when starting WonderGUI is to call Base::init()
	 * and the last thing you should do is to call Base::exit().
	 *
	 */

	class Base
	{
//		friend class Object_wp;
		friend class Object;

		friend class WeakPtrHub;

	public:

		//.____ Creation __________________________________________

		static bool init( HostBridge * pHostBridge, std::function<void(Error&)> errorHandler);
		static bool exit();

		//.____ Content _____________________________________________

		struct Blueprint
		{
			Caret_p				caret;
			GfxDevice_p			gfxDevice;
			NumberLayout_p		numberLayout;
			SurfaceFactory_p	surfaceFactory;
			TextLayout_p		textLayout;
			TextStyle_p			textStyle;
		};

		static void				setDefaults(const Blueprint& blueprint);

		static MsgRouter_p		msgRouter();
		static InputHandler_p	inputHandler();
		static BitmapCache_p	defaultBitmapCache();
		static TextLayout_p		defaultTextLayout();
		static Caret_p			defaultCaret();
		static TextStyle_p 		defaultTextStyle();
		static NumberLayout_p	defaultNumberLayout();
		static GfxDevice_p		defaultGfxDevice();
		static SurfaceFactory_p	defaultSurfaceFactory();

		static void				setClipboardText( const String& text );
		static String			getClipboardText();
		

		static HostBridge *		hostBridge()					{ return s_pHostBridge; }
		static std::function<void(Error&)>	errorHandler()		{ return s_pErrorHandler; }

	
		static inline void		setGammaCorrection(bool bOn)	{ s_bGammaCorrection = bOn; }
		static inline bool		gammaCorrection()				{ return s_bGammaCorrection; }


		
		//.____ Misc ________________________________________________

		const static TypeInfo	TYPEINFO;

		static bool			isInitialized() { return s_bInitialized; }

		static void			update( int64_t timestamp_microseconds );

		static char *		memStackAlloc( int bytes );
		static void			memStackFree( int bytes );

		static void			throwError( ErrorLevel level, ErrorCode code, const char * pMsg, const Object * pObject, const TypeInfo * pClassType, const char * pFunction, const char * pFile, int line );

		static void			startTrackingObjects();
		static void			stopTrackingObjects();
		static bool			isTrackingObjects() { return s_bTrackingObjects;  }

		static void			printObjects(std::ostream& stream);

		//.____ Internal ____________________________________________________________

		template<typename T>
		static T _trackObj(const T& pObj, const char* pFileName, int lineNb)
		{
			_trackObject(pObj, pFileName, lineNb);
			return pObj;
		}

		static void			_trackObject(Object* pObject, const char* pFileName, int lineNb);

		static int64_t		_startReceiveUpdates(Receiver* pReceiver);
		static void			_stopReceiveUpdates(Receiver* pReceiver);




	private:

		static WeakPtrHub *	_allocWeakPtrHub();
		static void			_freeWeakPtrHub(WeakPtrHub * pHub);

        inline static void	_objectWasCreated(Object* pObject) { s_objectsCreated.fetch_add(1, std::memory_order_relaxed); if (s_bTrackingObjects) _trackObject(pObject, nullptr, -1); }
        inline static void	_objectWillDestroy(Object* pObject) { s_objectsDestroyed.fetch_add(1, std::memory_order_relaxed); if (s_bTrackingObjects) s_trackedObjects.erase(pObject); }


		static bool					s_bInitialized;

		static MsgRouter_p			s_pMsgRouter;
		static InputHandler_p		s_pInputHandler;
		static TextLayout_p			s_pDefaultTextLayout;
		static Caret_p				s_pDefaultCaret;
		static NumberLayout_p		s_pDefaultNumberLayout;
		static TextStyle_p			s_pDefaultTextStyle;
		static GfxDevice_p			s_pDefaultGfxDevice;					// GfxDevice that can be used by Widgets when needed.
		static SurfaceFactory_p		s_pDefaultSurfaceFactory;				// SurfaceFactory that can be used by Widgets when needed.

		static BitmapCache_p		s_pDefaultBitmapCache;

		//

		static MemPool*				s_pPtrPool;
		static MemStack*			s_pMemStack;
		static String				s_clipboardText;
		static bool					s_bGammaCorrection;
		static HostBridge*			s_pHostBridge;
		static int64_t				s_timestamp;

		static std::function<void(Error&)>	s_pErrorHandler;
		static std::vector<Receiver*>		s_updateReceivers;


		
		struct ObjectInfo
		{
			// Filename and line where this objects create() method was called.

			const char*		pFileName;
			int				lineNb;
			unsigned int	serialNb;
		};

		static std::atomic<unsigned int>	s_objectsCreated;
		static std::atomic<unsigned int>	s_objectsDestroyed;

		static bool			s_bTrackingObjects;
		static std::unordered_map<Object*, ObjectInfo>	s_trackedObjects;
	};



} // namespace wg
#endif //WG_BASE_DOT_H
