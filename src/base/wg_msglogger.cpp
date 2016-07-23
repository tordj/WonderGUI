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

#include <cstdio>

#include <wg_msglogger.h>
#include <wg_widget.h>

namespace wg 
{
	
	using std::string;
	
	const char MsgLogger::CLASSNAME[] = {"MsgLogger"};
	
	//____ Constructor ____________________________________________________________
	
	MsgLogger::MsgLogger( std::ostream& stream ) : m_out(stream)
	{
		logAllMsgs();
		ignoreMsg( MsgType::Tick );
	}
	
	//____ Destructor _____________________________________________________________
	
	MsgLogger::~MsgLogger()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool MsgLogger::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return Receiver::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * MsgLogger::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	MsgLogger_p MsgLogger::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return MsgLogger_p( static_cast<MsgLogger*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ IgnoreMsg ____________________________________________________________
	
	void MsgLogger::ignoreMsg( MsgType type )
	{
		m_msgFilter[(int)type] = false;
	}
	
	//____ LogMsg _______________________________________________________________
	
	void MsgLogger::logMsg( MsgType type )
	{
		m_msgFilter[(int)type] = true;
	}
	
	//____ IgnorePointerMsgs ____________________________________________________________
	
	void MsgLogger::ignorePointerMsgs()
	{
		m_msgFilter[(int)MsgType::MouseEnter] = false;
		m_msgFilter[(int)MsgType::MouseMove] = false;
		m_msgFilter[(int)MsgType::MouseLeave] = false;
	}
	
	//____ LogPointerMsgs _______________________________________________________________
	
	void MsgLogger::logPointerMsgs()
	{
		m_msgFilter[(int)MsgType::MouseEnter] = true;
		m_msgFilter[(int)MsgType::MouseMove] = true;
		m_msgFilter[(int)MsgType::MouseLeave] = true;
	}
	
	
	//____ IgnoreMouseButtonMsgs ____________________________________________________________
	
	void MsgLogger::ignoreMouseButtonMsgs()
	{
		m_msgFilter[(int)MsgType::MousePress] = false;
		m_msgFilter[(int)MsgType::MouseRepeat] = false;
		m_msgFilter[(int)MsgType::MouseDrag] = false;
		m_msgFilter[(int)MsgType::MouseRelease] = false;
		m_msgFilter[(int)MsgType::MouseClick] = false;
		m_msgFilter[(int)MsgType::MouseDoubleClick] = false;
	}
	
	//____ LogMouseButtonMsgs _______________________________________________________________
	
	void MsgLogger::logMouseButtonMsgs()
	{
		m_msgFilter[(int)MsgType::MousePress] = true;
		m_msgFilter[(int)MsgType::MouseRepeat] = true;
		m_msgFilter[(int)MsgType::MouseDrag] = true;
		m_msgFilter[(int)MsgType::MouseRelease] = true;
		m_msgFilter[(int)MsgType::MouseClick] = true;
		m_msgFilter[(int)MsgType::MouseDoubleClick] = true;
	}
	
	//____ IgnoreKeyboardMsgs ____________________________________________________________
	
	void MsgLogger::ignoreKeyboardMsgs()
	{
		m_msgFilter[(int)MsgType::KeyPress] = false;
		m_msgFilter[(int)MsgType::KeyRepeat] = false;
		m_msgFilter[(int)MsgType::KeyRelease] = false;
		m_msgFilter[(int)MsgType::TextInput] = false;
	}
	
	//____ LogKeyboardMsgs _______________________________________________________________
	
	void MsgLogger::logKeyboardMsgs()
	{
		m_msgFilter[(int)MsgType::KeyPress] = true;
		m_msgFilter[(int)MsgType::KeyRepeat] = true;
		m_msgFilter[(int)MsgType::KeyRelease] = true;
		m_msgFilter[(int)MsgType::TextInput] = true;
	}
	
	//____ logMouseMsgs() _______________________________________________________
	
	void MsgLogger::logMouseMsgs()
	{
		logPointerMsgs();
		logMouseButtonMsgs();
		logMsg( MsgType::WheelRoll );
	}
	
	//____ ignoreMouseMsgs() ____________________________________________________
	
	void MsgLogger::ignoreMouseMsgs()
	{
		ignorePointerMsgs();
		ignoreMouseButtonMsgs();
		ignoreMsg( MsgType::WheelRoll );
	}
	
	
	//____ logInputMsgs() _______________________________________________________
	
	void MsgLogger::logInputMsgs()
	{
		logMouseMsgs();
		logKeyboardMsgs();
	}
	
	//____ ignoreInputMsgs() ____________________________________________________
	
	void MsgLogger::ignoreInputMsgs()
	{
		ignoreMouseMsgs();
		ignoreKeyboardMsgs();
	}
	
	
	
	
	//____ IgnoreAllMsgs ________________________________________________________
	
	void MsgLogger::ignoreAllMsgs()
	{
		for( int i = 0 ; i < (int) MsgType::Max ; i++ )
			m_msgFilter[i] = false;
	}
	
	//____ LogAllMsgs ___________________________________________________________
	void MsgLogger::logAllMsgs()
	{
		for( int i = 0 ; i < (int) MsgType::Max ; i++ )
			m_msgFilter[i] = true;
	}
	
	
	
	//____ receive() ______________________________________________________________
	
	void MsgLogger::receive( const Msg_p& _pMsg )
	{
		if( m_msgFilter[(int)_pMsg->type()] == false )
			return;
	
		string	source;
		string	copyTo;
	
		char	params[256]; params[0] = 0;			// Msg specific parameters
	
		
		switch( _pMsg->type() )
		{
			case MsgType::Dummy:
				break;
			case MsgType::Tick:
				sprintf( params, " millisec=%d", TickMsg::cast(_pMsg)->timediff() );
				break;
			case MsgType::PointerChange:
				sprintf( params, " style=%s", _formatPointerStyle( PointerChangeMsg::cast(_pMsg)).c_str() );
				break;
			case MsgType::MouseEnter:
				break;
			case MsgType::MouseMove:
				break;
			case MsgType::MouseLeave:
				break;
			case MsgType::MousePress:
				sprintf( params, " button=%s", _formatMouseButton(MousePressMsg::cast(_pMsg)->button()).c_str() );
				break;
			case MsgType::MouseRepeat:
				sprintf( params, " button=%s", _formatMouseButton(MouseRepeatMsg::cast(_pMsg)->button()).c_str() );
				break;
			case MsgType::MouseDrag:
			{
				MouseDragMsg_p pMsg = MouseDragMsg::cast(_pMsg);
	
				Coord	now		= pMsg->currPos();
				Coord 	prev	= pMsg->prevPos();
				Coord	start	= pMsg->startPos();
	
				Coord	dragNow	= pMsg->draggedNow();
				Coord	dragTotal=pMsg->draggedTotal();
	
				sprintf( params, " button=%s position(start=%d,%d prev=%d,%d now=%d,%d) dragged(now=%d,%d total=%d,%d)",
						_formatMouseButton(pMsg->button()).c_str(), start.x, start.y, prev.x, prev.y, now.x, now.y, dragNow.x, dragNow.y, dragTotal.x, dragTotal.y );
				break;
			}
			case MsgType::MouseRelease:
			{
				MouseReleaseMsg_p pMsg = MouseReleaseMsg::cast(_pMsg);
	
				const static char outside[] = "outside";
				const static char inside[] = "inside";
				const char * pPress = outside;
				const char * pRelease = outside;
		
				if( pMsg->releaseInside() )
					pRelease = inside;
	
				sprintf( params, " button=%s release=%s", _formatMouseButton(pMsg->button()).c_str(), pRelease );
				break;
			}
			case MsgType::MouseClick:
				sprintf( params, " button=%s", _formatMouseButton(MouseClickMsg::cast(_pMsg)->button()).c_str() );
				break;
			case MsgType::MouseDoubleClick:
				sprintf( params, " button=%s", _formatMouseButton(MouseDoubleClickMsg::cast(_pMsg)->button()).c_str() );
				break;
	
			case MsgType::KeyPress:
			{
				KeyPressMsg_p pMsg = KeyPressMsg::cast(_pMsg);
				sprintf( params, " wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
				break;
			}
			case MsgType::KeyRepeat:
			{
				KeyRepeatMsg_p pMsg = KeyRepeatMsg::cast(_pMsg);
				sprintf( params, " wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
				break;
			}
			case MsgType::KeyRelease:
			{
				KeyReleaseMsg_p pMsg = KeyReleaseMsg::cast(_pMsg);
				sprintf( params, " wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
				break;
			}
			case MsgType::TextInput:
				sprintf( params, " chars=%s", TextInputMsg::cast(_pMsg)->text().chars() );
				break;
				
			case MsgType::EditCommand:
				sprintf( params, " cmd=%s", _formatEditCommand(EditCommandMsg::cast(_pMsg)->command()).c_str() );
				break;
				
			case MsgType::WheelRoll:
			{
				WheelRollMsg_p pMsg = WheelRollMsg::cast(_pMsg);
				sprintf( params, "wheel=%d distance=(%d,%d)", pMsg->wheel(), pMsg->distance().x, pMsg->distance().y );
				break;
			}
	
			case MsgType::Select:
				break;
	
			case MsgType::Toggle:
				sprintf( params, " set=%d", (int) (ToggleMsg::cast(_pMsg)->isSet()) );
				break;
	
			case MsgType::ValueUpdate:
			{
				ValueUpdateMsg_p pMsg = ValueUpdateMsg::cast(_pMsg);
				sprintf( params, " value=%ld fraction=%f", pMsg->value(), pMsg->fraction() );
				break;
			}
	
			case MsgType::RangeUpdate:
			{
				RangeUpdateMsg_p pMsg = RangeUpdateMsg::cast(_pMsg);
				sprintf( params, " offset=%d length=%d fracOfs=%f fracLen=%f final=%s", pMsg->offset(), pMsg->length(), pMsg->fracOffset(), pMsg->fracLength(), pMsg->isFinal()?"true":"false" );
				break;
			}
	
			case MsgType::TextEdit:
			{
				TextEditMsg_p pMsg = TextEditMsg::cast(_pMsg);
				break;
			}
	
			case MsgType::ItemToggle:
			{
				ItemToggleMsg_p pMsg = ItemToggleMsg::cast(_pMsg);
				sprintf( params, " set=%s id=%d object=%p", pMsg->isSet()?"true":"false", pMsg->itemId(), pMsg->itemObject().rawPtr() );
				break;
			}
			case MsgType::ItemMousePress:
			{
				ItemMousePressMsg_p pMsg = ItemMousePressMsg::cast(_pMsg);
				sprintf( params, " id=%d mouseButton=%s", pMsg->itemId(), _formatMouseButton(pMsg->button()).c_str() );
				break;
			}
					
			case MsgType::ItemsSelect:
			{
				ItemsSelectMsg_p pMsg = ItemsSelectMsg::cast(_pMsg);
				sprintf( params, " nbItems=%d", pMsg->nbItems() );
				break;
			}
	
			case MsgType::ItemsUnselect:
			{
				ItemsUnselectMsg_p pMsg = ItemsUnselectMsg::cast(_pMsg);
				sprintf( params, " nbItems=%d", pMsg->nbItems() );
				break;
			}

			case MsgType::LinkMouseEnter:
			case MsgType::LinkMouseLeave:
			case MsgType::LinkSelect:
			{
				LinkMsg_p pMsg = LinkMsg::cast(_pMsg);
				sprintf( params, " link=%p string=\"%s\"", pMsg->link().rawPtr(), pMsg->link()->link().c_str() );
				break;
			}

			case MsgType::LinkMousePress:
			case MsgType::LinkMouseRepeat:
			case MsgType::LinkMouseRelease:
			case MsgType::LinkMouseClick:
			case MsgType::LinkMouseDoubleClick:
			{
				LinkMouseButtonMsg_p pMsg = LinkMouseButtonMsg::cast(_pMsg);
				sprintf( params, " link=%p button=%s", pMsg->link().rawPtr(), _formatMouseButton(pMsg->button()).c_str() );
				break;
			}
			case MsgType::ModalMoveOutside:
				break;
			case MsgType::ModalBlockedPress:
				sprintf( params, " button=%s", _formatMouseButton(ModalBlockedPressMsg::cast(_pMsg)->button()).c_str() );
				break;
			case MsgType::ModalBlockedRelease:
				sprintf( params, " button=%s", _formatMouseButton(ModalBlockedReleaseMsg::cast(_pMsg)->button()).c_str() );
				break;
	
			default:
				break;
		};
	
		source = _formatSource( _pMsg );
		copyTo = _formatCopyTo( _pMsg );

		string	timestamp;
		string	modkeys;
		string	pointerPos;

		if( _pMsg->isInstanceOf( InputMsg::CLASSNAME ) )
		{
			InputMsg_p p = InputMsg::cast(_pMsg);
			timestamp = _formatTimestamp( p->timestamp() );
			modkeys = _formatModkeys( p );
			pointerPos = _formatPointerPos( p );
		}
		
		m_out << " - " << _pMsg->className() << " - " << source << copyTo << pointerPos << modkeys << params;
		m_out << std::endl;
	}
	
	//____ _formatTimestamp() ______________________________________________________
	
	string MsgLogger::_formatTimestamp( int64_t ms ) const
	{
		char	temp[20];
	
		int milli	= (int) (ms % 1000);
		int sec		= (int) ((ms/1000) % 60);
		int min		= (int) ((ms/60000) % 60);
		int hour	= (int) (ms/3600000);
	
		sprintf( temp, "%d:%02d:%02d.%03d", hour, min, sec, milli );
		return string( temp );
	}
	
	//____ _formatSource() __________________________________________________________
	
	string MsgLogger::_formatSource( const Msg_p& _pMsg ) const
	{
		std::string	out;
	
		if( _pMsg->sourceRawPtr() )
		{
			char	temp[64];
			Object * pObject = _pMsg->sourceRawPtr();
	
			static const char def_type[] = "deleted";
			const char * pType = def_type;
	
			if( pObject )
				pType = pObject->className();
	
			sprintf( temp, " source=%p (%s)", pObject, pType );
			out = temp;
		}
	
		return out;
	}
	
	//____ _formatCopyTo() __________________________________________________________
	
	string MsgLogger::_formatCopyTo( const Msg_p& _pMsg ) const
	{
		std::string	out;
	
		if( _pMsg->hasCopyTo() )
		{
			char	temp[64];
			Receiver * pCopyTo = _pMsg->getCopyTo().rawPtr();
	
			const char * pType = pCopyTo->className();
	
			sprintf( temp, " copyTo=%p (%s)", pCopyTo, pType );
			out = temp;
		}
	
		return out;
	}
	
	
	//____ _formatModkeys() __________________________________________________________
	
	string MsgLogger::_formatModkeys( const InputMsg_p& _pMsg ) const
	{
		ModifierKeys keys = _pMsg->modKeys();
	
		string	out;
	
		if( keys != MODKEY_NONE )
		{
			char	temp[64];
			sprintf( temp, " modkeys=%X", keys );
			out = temp;
		}
	
		return out;
	}
	
	//____ _formatPointerPos() _____________________________________________________
	
	string MsgLogger::_formatPointerPos( const InputMsg_p& _pMsg ) const
	{
		Coord globalPos = _pMsg->pointerPos();
	
	
		char	temp[64];
	
		sprintf( temp, " pointer=%d,%d", globalPos.x, globalPos.y );
	
		return string(temp);
	}
	
	//____ _formatPointerStyle() _____________________________________________________
	
	string MsgLogger::_formatPointerStyle( const PointerChangeMsg_p& _pMsg ) const
	{
		switch( _pMsg->style() )
		{
			case PointerStyle::Arrow:
				return "PointerStyle::Arrow";
			case PointerStyle::Hourglass:
				return "PointerStyle::Hourglass";
			case PointerStyle::Hand:
				return "PointerStyle::Hand";
			case PointerStyle::Crosshair:
				return "PointerStyle::Crosshair";
			case PointerStyle::Help:
				return "PointerStyle::Help";
			case PointerStyle::Ibeam:
				return "PointerStyle::Ibeam";
			case PointerStyle::Stop:
				return "PointerStyle::Stop";
			case PointerStyle::UpArrow:
				return "PointerStyle::UpArrow";
			case PointerStyle::ResizeAll:
				return "PointerStyle::ResizeAll";
			case PointerStyle::ResizeNeSw:
				return "PointerStyle::ResizeNeSw";
			case PointerStyle::ResizeNwSe:
				return "PointerStyle::ResizeNwSe";
			case PointerStyle::ResizeNS:
				return "PointerStyle::ResizeNS";
			case PointerStyle::ResizeWE:
				return "PointerStyle::ResizeWE";
			default:
			{
				char	temp[64];
				sprintf( temp, "%d (unkown enum)", (int) _pMsg->style() );
				return string(temp);
			}
		}
	}
	
	//____ _formatMouseButton() ____________________________________________________
	
	std::string MsgLogger::_formatMouseButton( MouseButton button ) const
	{
		switch( button )
		{
			case MouseButton::Left:
				return "LEFT";
			case MouseButton::Middle:
				return "MIDDLE";
			case MouseButton::Right:
				return "RIGHT";
			case MouseButton::X1:
				return "X1";
			case MouseButton::X2:
				return "X2";
			default:
			{
				char	temp[64];
				sprintf( temp, "%d (unkown enum)", (int) button );
				return string(temp);
			}
		}
	}

	//____ _formatEditCommand() ____________________________________________________
	
	std::string MsgLogger::_formatEditCommand( EditCmd command ) const
	{
		switch( command )
		{
			case EditCmd::None:
				return "None";
			case EditCmd::Break:
				return "Break";
			case EditCmd::Escape:
				return "Escape";
			case EditCmd::Undo:
				return "Undo";
			case EditCmd::Redo:
				return "Redo";
			case EditCmd::Cut:
				return "Cut";
			case EditCmd::Copy:
				return "Copy";
			case EditCmd::Paste:
				return "Paste";
			case EditCmd::SelectAll:
				return "SelectAll";
			default:
			{
				char	temp[64];
				sprintf( temp, "%d (unkown enum)", (int) command );
				return string(temp);
			}
		}
	}

	
	//____ _onRouteAdded() _________________________________________________________
	
	void  MsgLogger::_onRouteAdded()
	{	
		_incRefCount();
	}
	
	//____ _onRouteRemoved() _______________________________________________________
	
	void  MsgLogger::_onRouteRemoved()
	{
		_decRefCount();
	}

} // namespace wg
