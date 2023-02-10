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

#include <string>

#include <wg_c_gearbase.h>
#include <wg_gearbase.h>

using namespace wg;

int wg_initGearBase()
{
	return GearBase::init();
}

int wg_exitGearBase()
{
	return GearBase::exit();
}

int wg_isGearBaseInitialized()
{
	return GearBase::isInitialized();
}

void wg_setErrorHandler( void (*errorHandler)(const wg_errorInfo * pError) )
{

	GearBase::setErrorHandler( [errorHandler](Error& error)
	{
		wg_errorInfo	errorInfo;

		wg_errorCode		errorCode;
		wg_errorLevel	severity;

		switch( error.code )
		{
			case ErrorCode::OutOfRange:
				errorCode = WG_ERROR_OUT_OF_RANGE;
				break;

			case ErrorCode::InvalidIterator:
				errorCode = WG_ERROR_INVALID_ITERATOR;
				break;

			case ErrorCode::InvalidParam:
				errorCode = WG_ERROR_INVALID_PARAM;
				break;

			case ErrorCode::FailedPrerequisite:
				errorCode = WG_ERROR_FAILED_PREREQUISITE;
				break;

			case ErrorCode::OpenGL:
				errorCode = WG_ERROR_OPENGL;
				break;

			case ErrorCode::Internal:
				errorCode = WG_ERROR_INTERNAL;
				break;

			case ErrorCode::SystemIntegrity:
				errorCode = WG_ERROR_SYSTEM_INTEGRITY;
				break;

			case ErrorCode::IllegalCall:
				errorCode = WG_ERROR_ILLEGAL_CALL;
				break;

			case ErrorCode::ResourceExhausted:
				errorCode = WG_ERROR_RESOURCE_EXHAUSTED;
				break;

			default:
				errorCode = WG_ERROR_OTHER;
		}

		switch( error.severity )
		{
			case ErrorLevel::Warning:
			severity = WG_WARNING;
			break;

			case ErrorLevel::SilentError:
			severity = WG_SILENT_ERROR;
			break;

			case ErrorLevel::Error:
			severity = WG_ERROR;
			break;

			case ErrorLevel::Critical:
			severity = WG_CRITICAL;
			break;
		}

		errorInfo.classname = error.classname;
		errorInfo.code = errorCode;
		errorInfo.file = error.file;
		errorInfo.function = error.function;
		errorInfo.object = error.pObject;
		errorInfo.line = error.line;
		errorInfo.message = error.message.c_str();
		errorInfo.severity = severity;


		errorHandler(&errorInfo);
	});
}
