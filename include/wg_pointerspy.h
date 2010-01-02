
#ifndef WG_POINTERSPY_DOT_H
#define WG_POINTERSPY_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgWidget;

class WgPointerSpy
{
public:
//	virtual void EnterWidget( WgWidget * pWidget ) = 0;
//	virtual void LeaveWidget( WgWidget * pWidget ) = 0;
	virtual void PointerPosition( Sint32 x, Sint32 y, WgWidget * pWidget, Uint32 msHover ) = 0;
};

#endif //WG_POINTERSPY_DOT_H

