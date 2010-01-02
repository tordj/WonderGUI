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

#ifndef	WG_SIGNALSTANZA_DOT_H
#define WG_SIGNALSTANZA_DOT_H


namespace WgSignal
{
class Signal 
	{
		friend bool operator==(const Signal&, const Signal&);	
	public:
		Signal()
		{
			type = 0;
			data = 0;
		}

	protected:


		Uint16	type;
		Uint16	data;
	};


	inline bool operator==(const Signal& c1, const Signal& c2)
	{ 
		if( c1.type == c2.type && c1.data == c2.data ) 
			return true; 
		return false;
	};



	//____

	class Signal_void 			: public Signal { protected: Signal_void(){}; };
	class Signal_bool 			: public Signal { protected: Signal_bool(){}; };
	class Signal_int  			: public Signal { protected: Signal_int(){}; };
	class Signal_float			: public Signal { protected: Signal_float(){}; };
	class Signal_voidptr		: public Signal { protected: Signal_voidptr(){}; };
	class Signal_charptr		: public Signal { protected: Signal_charptr(){}; };
	class Signal_bool_bool		: public Signal { protected: Signal_bool_bool(){}; };
	class Signal_int_int		: public Signal { protected: Signal_int_int(){}; };
	class Signal_float_float	: public Signal { protected: Signal_float_float(){}; };
	class Signal_item			: public Signal { protected: Signal_item(){}; };
	class Signal_menuitem		: public Signal { protected: Signal_menuitem(){}; };
	class Signal_treeentry		: public Signal { protected: Signal_treeentry(){}; };
};

#endif	//WG_SIGNALSTANZA_DOT_H
