#ifndef	Wdg_Profiler_DOT_H
#define Wdg_Profiler_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#include <wg_textprop.h>

class WgFont;


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_PROFILER_DOT_H
#	include <wg_gizmo_profiler.h>
#endif

class Wdg_Profiler : public Wdg_Widget<WgGizmoProfiler>
{
public:
	Wdg_Profiler( void ) {};
	Wdg_Profiler ( const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoProfiler>(geometry, pParent) {};
	Wdg_Profiler( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoProfiler>( origo, geometry, pParent ) {};
	Wdg_Profiler( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoProfiler>(upperLeft,x1, y1, lowerRight, x2, y2, pParent) {}	
	virtual ~Wdg_Profiler() {};

};


#endif //WDG_PROFILER_DOT_H
