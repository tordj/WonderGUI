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

#include <string>
#include <functional>
#include <unordered_map>

namespace wg
{
	class Font;
	class MemPool;
	class WeakPtrHub;
	class MemStack;
	class MsgRouter;
	class ValueFormatter;
	class InputHandler;
	class TextMapper;
	class Caret;
	class TextStyle;
	class Context;
	class Object;

	typedef	StrongPtr<MsgRouter>		MsgRouter_p;
	typedef	StrongPtr<ValueFormatter>	ValueFormatter_p;
	typedef	StrongPtr<InputHandler>		InputHandler_p;
	typedef	StrongPtr<TextMapper>		TextMapper_p;
	typedef	StrongPtr<Caret>			Caret_p;
	typedef	StrongPtr<TextStyle>		TextStyle_p;
	typedef	StrongPtr<Context>			Context_p;


	class Error
	{
	public:
		ErrorSeverity	severity;
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

		template<class T> friend class WeakComponentPtr;

		friend class WeakPtrHub;

	public:

		//.____ Creation __________________________________________

		static void init();
		static int exit();

		//.____ Content _____________________________________________

		static MsgRouter_p	msgRouter();
		static InputHandler_p	inputHandler();

		static void			setDefaultTextMapper( TextMapper * pTextMapper );
		static TextMapper_p defaultTextMapper();

		static void			setDefaultCaret(Caret * pCaret);
		static Caret_p		defaultCaret();

		static void			setDefaultStyle( TextStyle * pStyle );
		static TextStyle_p 	defaultStyle();

		static void			setDefaultValueFormatter(ValueFormatter * pFormatter);
		static ValueFormatter_p defaultValueFormatter();

		static void			setActiveContext(Context * pContext);
		static Context_p	activeContext();

		static void			setErrorHandler(std::function<void(Error&)> handler);
		std::function<void(Error&)>	errorHandler();

		//.____ Misc ________________________________________________

		static char *		memStackAlloc( int bytes );
		static void			memStackRelease( int bytes );


		static void			handleError( ErrorSeverity severity, ErrorCode code, const char * pMsg, const Object * pObject, const TypeInfo& pClassType, const char * pFunction, const char * pFile, int line );

		static void			beginObjectTracking();
		static void			endObjectTracking();
		static bool			isObjectTracking() { return s_bTrackingObjects;  }

		static void			printObjects(std::ostream& stream);

		//.____ Internal ____________________________________________________________

		template<typename T>
		static T _trackObj(const T& pObj, const char* pFileName, int lineNb)
		{
			_trackObject(pObj, pFileName, lineNb);
			return pObj;
		}

		static void			_trackObject(Object* pObject, const char* pFileName, int lineNb);



	private:

		static WeakPtrHub *	_allocWeakPtrHub();
		static void			_freeWeakPtrHub(WeakPtrHub * pHub);

		inline static void	_objectWasCreated(Object* pObject) { s_objectsCreated++; if (s_bTrackingObjects) _trackObject(pObject, nullptr, -1); }
		inline static void	_objectWillDestroy(Object* pObject) { s_objectsDestroyed++; if (s_bTrackingObjects) s_trackedObjects.erase(pObject); }


		struct Data
		{
			MsgRouter_p		pMsgRouter;
			InputHandler_p	pInputHandler;

			Context_p		pActiveContext;

			TextMapper_p		pDefaultTextMapper;
			Caret_p				pDefaultCaret;
			TextStyle_p			pDefaultStyle;
			ValueFormatter_p	pDefaultValueFormatter;


			//

			MemPool *		pPtrPool;
			MemStack *		pMemStack;


		};

		static Data *						s_pData;
		static std::function<void(Error&)>	s_pErrorHandler;

		struct ObjectInfo
		{
			// Filename and line where this objects create() method was called.

			const char*		pFileName;
			int				lineNb;
			unsigned int	serialNb;
		};

		static unsigned int	s_objectsCreated;
		static unsigned int	s_objectsDestroyed;

		static bool			s_bTrackingObjects;
		static std::unordered_map<Object*, ObjectInfo>	s_trackedObjects;
	};



} // namespace wg
#endif //WG_BASE_DOT_H
