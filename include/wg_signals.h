#ifndef	WG_SIGNALS_DOT_H
#define	WG_SIGNALS_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_SIGNALSTANZA_DOT_H
#	include <wg_signalstanza.h>
#endif

namespace WgSignal
{

	class PointerEnter			: public Signal_void { public: PointerEnter(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerExit			: public Signal_void { public: PointerExit(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOver			: public Signal_void { public: PointerOver(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOverOfs		: public Signal_int_int { public: PointerOverOfs(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOverPos		: public Signal_int_int { public: PointerOverPos(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOverOfsX		: public Signal_int { public: PointerOverOfsX(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOverOfsY		: public Signal_int { public: PointerOverOfsY(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOverPosX		: public Signal_int { public: PointerOverPosX(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOverPosY		: public Signal_int { public: PointerOverPosY(){ type = (Uint16) __LINE__; data = 0; };};

	class WheelRoll				: public Signal_int { public: WheelRoll( Uint8 wheel ){ type = (Uint16) __LINE__; data = wheel; };};

	class ButtonPress			: public Signal_void { public: ButtonPress( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonPressOfs		: public Signal_int_int { public: ButtonPressOfs( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonPressPos		: public Signal_int_int { public: ButtonPressPos( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonPressOfsX		: public Signal_int { public: ButtonPressOfsX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonPressPosX		: public Signal_int { public: ButtonPressPosX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonPressOfsY		: public Signal_int { public: ButtonPressOfsY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonPressPosY		: public Signal_int { public: ButtonPressPosY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};

	class ButtonDown			: public Signal_void { public: ButtonDown( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDownOfs			: public Signal_int_int { public: ButtonDownOfs( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDownPos			: public Signal_int_int { public: ButtonDownPos( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDownOfsX		: public Signal_int { public: ButtonDownOfsX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDownPosX		: public Signal_int { public: ButtonDownPosX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDownOfsY		: public Signal_int { public: ButtonDownOfsY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDownPosY		: public Signal_int { public: ButtonDownPosY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};

	class ButtonRepeat			: public Signal_void { public: ButtonRepeat( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonRepeatOfs		: public Signal_int_int { public: ButtonRepeatOfs( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonRepeatPos		: public Signal_int_int { public: ButtonRepeatPos( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonRepeatOfsX		: public Signal_int { public: ButtonRepeatOfsX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonRepeatPosX		: public Signal_int { public: ButtonRepeatPosX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonRepeatOfsY		: public Signal_int { public: ButtonRepeatOfsY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonRepeatPosY		: public Signal_int { public: ButtonRepeatPosY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};

	class ButtonRelease			: public Signal_void { public: ButtonRelease( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOfs		: public Signal_int_int { public: ButtonReleaseOfs( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleasePos		: public Signal_int_int { public: ButtonReleasePos( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOfsX		: public Signal_int { public: ButtonReleaseOfsX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleasePosX		: public Signal_int { public: ButtonReleasePosX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOfsY		: public Signal_int { public: ButtonReleaseOfsY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleasePosY		: public Signal_int { public: ButtonReleasePosY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};

	class ButtonReleaseOutside		: public Signal_void { public: ButtonReleaseOutside( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOutsideOfs	: public Signal_int_int { public: ButtonReleaseOutsideOfs( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOutsidePos	: public Signal_int_int { public: ButtonReleaseOutsidePos( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOutsideOfsX	: public Signal_int { public: ButtonReleaseOutsideOfsX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOutsidePosX	: public Signal_int { public: ButtonReleaseOutsidePosX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOutsideOfsY	: public Signal_int { public: ButtonReleaseOutsideOfsY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOutsidePosY	: public Signal_int { public: ButtonReleaseOutsidePosY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};


	class ButtonClick			: public Signal_void { public: ButtonClick( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonClickOfs		: public Signal_int_int { public: ButtonClickOfs( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonClickPos		: public Signal_int_int { public: ButtonClickPos( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonClickOfsX		: public Signal_int { public: ButtonClickOfsX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonClickPosX		: public Signal_int { public: ButtonClickPosX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonClickOfsY		: public Signal_int { public: ButtonClickOfsY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonClickPosY		: public Signal_int { public: ButtonClickPosY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};

	class ButtonDoubleClick			: public Signal_void { public: ButtonDoubleClick( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDoubleClickOfs		: public Signal_int_int { public: ButtonDoubleClickOfs( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDoubleClickPos		: public Signal_int_int { public: ButtonDoubleClickPos( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDoubleClickOfsX		: public Signal_int { public: ButtonDoubleClickOfsX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDoubleClickPosX		: public Signal_int { public: ButtonDoubleClickPosX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDoubleClickOfsY		: public Signal_int { public: ButtonDoubleClickOfsY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDoubleClickPosY		: public Signal_int { public: ButtonDoubleClickPosY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};

	class ButtonPressOutsideModal	: public Signal_void { public: ButtonPressOutsideModal( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDownOutsideModal	: public Signal_void { public: ButtonDownOutsideModal( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonRepeatOutsideModal	: public Signal_void { public: ButtonRepeatOutsideModal( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonReleaseOutsideModal	: public Signal_void { public: ButtonReleaseOutsideModal( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonClickOutsideModal	: public Signal_void { public: ButtonClickOutsideModal( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDoubleClickOutsideModal	: public Signal_void { public: ButtonDoubleClickOutsideModal( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class WheelRollOutsideModal		: public Signal_int { public: WheelRollOutsideModal( Uint8 wheel ){ type = (Uint16) __LINE__; data = wheel; };};

	class PointerOutsideModal		: public Signal_void { public: PointerOutsideModal(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOutsideModalOfs	: public Signal_int_int { public: PointerOutsideModalOfs(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOutsideModalPos	: public Signal_int_int { public: PointerOutsideModalPos(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOutsideModalOfsX	: public Signal_int { public: PointerOutsideModalOfsX(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOutsideModalOfsY	: public Signal_int { public: PointerOutsideModalOfsY(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOutsideModalPosX	: public Signal_int { public: PointerOutsideModalPosX(){ type = (Uint16) __LINE__; data = 0; };};
	class PointerOutsideModalPosY	: public Signal_int { public: PointerOutsideModalPosY(){ type = (Uint16) __LINE__; data = 0; };};



	class ButtonDragTotal			: public Signal_int_int { public: ButtonDragTotal( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDragTotalX			: public Signal_int { public: ButtonDragTotalX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDragTotalY			: public Signal_int { public: ButtonDragTotalY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};

	class ButtonDragSinceLast		: public Signal_int_int { public: ButtonDragSinceLast( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDragSinceLastX		: public Signal_int { public: ButtonDragSinceLastX( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};
	class ButtonDragSinceLastY		: public Signal_int { public: ButtonDragSinceLastY( Uint8 button ){ type = (Uint16) __LINE__; data = button; };};

	class KeyPress					: public Signal_void { public: KeyPress( Uint8 keycode, Uint8 modifier ) { type = (Uint16) __LINE__; data = ((Uint16)modifier << 8) + keycode;; };};
	class KeyDown					: public Signal_void { public: KeyDown( Uint8 keycode, Uint8 modifier ) { type = (Uint16) __LINE__; data = ((Uint16)modifier << 8) + keycode;; };};
	class KeyRepeat					: public Signal_void { public: KeyRepeat( Uint8 keycode, Uint8 modifier ) { type = (Uint16) __LINE__; data = ((Uint16)modifier << 8) + keycode;; };};
	class KeyRelease				: public Signal_void { public: KeyRelease( Uint8 keycode, Uint8 modifier ) { type = (Uint16) __LINE__; data = ((Uint16)modifier << 8) + keycode;; };};


	class ItemEnter					: public Signal_item { public: ItemEnter() { type = (Uint16) __LINE__; data = 0; };};
	class ItemOver					: public Signal_item { public: ItemOver() { type = (Uint16) __LINE__; data = 0; };};
	class ItemExit					: public Signal_item { public: ItemExit() { type = (Uint16) __LINE__; data = 0; };};

	class ItemPress					: public Signal_item { public: ItemPress( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class ItemDown					: public Signal_item { public: ItemDown( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class ItemRepeat				: public Signal_item { public: ItemRepeat( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class ItemRelease				: public Signal_item { public: ItemRelease( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class ItemClick					: public Signal_item { public: ItemClick( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class ItemDoubleClick			: public Signal_item { public: ItemDoubleClick( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};

	//		static Signal_WgItemPtr	sig_itemPointerEnter;
	//		static Signal_WgItemPtr	sig_itemPointerExit; 
	//		static Signal_WgItemPtr sig_itemPointerOver;  

	class TableRowPress				: public Signal_int { public: TableRowPress( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class TableColumnPress			: public Signal_int { public: TableColumnPress( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class TableCellPress			: public Signal_int_int { public: TableCellPress( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};

	class TableRowClick				: public Signal_int { public: TableRowClick( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class TableColumnClick			: public Signal_int { public: TableColumnClick( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class TableCellClick			: public Signal_int_int { public: TableCellClick( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};

	class TableRowDoubleClick		: public Signal_int { public: TableRowDoubleClick( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class TableColumnDoubleClick	: public Signal_int { public: TableColumnDoubleClick( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};
	class TableCellDoubleClick		: public Signal_int_int { public: TableCellDoubleClick( Uint8 button ) { type = (Uint16) __LINE__; data = button; };};


	class ViewPos					: public Signal_float_float { public: ViewPos() { type = (Uint16) __LINE__; data = 0; };};
	class ViewPosPixel				: public Signal_int_int { public: ViewPosPixel() { type = (Uint16) __LINE__; data = 0; };};

	class ViewPosX					: public Signal_float { public: ViewPosX() { type = (Uint16) __LINE__; data = 0; };};
	class ViewPosY					: public Signal_float { public: ViewPosY() { type = (Uint16) __LINE__; data = 0; };};

	class ViewPosPixelX				: public Signal_int { public: ViewPosPixelX() { type = (Uint16) __LINE__; data = 0; };};
	class ViewPosPixelY				: public Signal_int { public: ViewPosPixelY() { type = (Uint16) __LINE__; data = 0; };};

	class ViewSize					: public Signal_float_float { public: ViewSize() { type = (Uint16) __LINE__; data = 0; };};
	class ViewSizePixel				: public Signal_int_int { public: ViewSizePixel() { type = (Uint16) __LINE__; data = 0; };};

	class ViewSizeX					: public Signal_float { public: ViewSizeX() { type = (Uint16) __LINE__; data = 0; };};
	class ViewSizeY					: public Signal_float { public: ViewSizeY() { type = (Uint16) __LINE__; data = 0; };};

	class ViewSizePixelX			: public Signal_int { public: ViewSizePixelX() { type = (Uint16) __LINE__; data = 0; };};
	class ViewSizePixelY			: public Signal_int { public: ViewSizePixelY() { type = (Uint16) __LINE__; data = 0; };};

	class ViewPosSizeX				: public Signal_float_float { public: ViewPosSizeX() { type = (Uint16) __LINE__; data = 0; };};
	class ViewPosSizeY				: public Signal_float_float { public: ViewPosSizeY() { type = (Uint16) __LINE__; data = 0; };};

	class ViewPosSizePixelX			: public Signal_int_int { public: ViewPosSizePixelX() { type = (Uint16) __LINE__; data = 0; };};
	class ViewPosSizePixelY			: public Signal_int_int { public: ViewPosSizePixelY() { type = (Uint16) __LINE__; data = 0; };};



	class Set						: public Signal_void { public: Set() { type = (Uint16) __LINE__; data = 0; };};
	class Unset						: public Signal_void { public: Unset() { type = (Uint16) __LINE__; data = 0; };};
	class Flipped					: public Signal_bool { public: Flipped() { type = (Uint16) __LINE__; data = 0; };};


	class SliderPos					: public Signal_float { public: SliderPos() { type = (Uint16) __LINE__; data = 0; };};
	class PrevPage					: public Signal_void { public: PrevPage() { type = (Uint16) __LINE__; data = 0; };};
	class NextPage					: public Signal_void { public: NextPage() { type = (Uint16) __LINE__; data = 0; };};
	class Forward					: public Signal_void { public: Forward() { type = (Uint16) __LINE__; data = 0; };};
	class Back						: public Signal_void { public: Back() { type = (Uint16) __LINE__; data = 0; };};

	class IntegerChanged			: public Signal_int64 { public: IntegerChanged() { type = (Uint16) __LINE__; data = 0; };};
	class Fraction					: public Signal_float { public: Fraction() { type = (Uint16) __LINE__; data = 0; };};

	class TabPressed				: public Signal_int { public: TabPressed() { type = (Uint16) __LINE__; data = 0; };};
	class TabSelected				: public Signal_int { public: TabSelected() { type = (Uint16) __LINE__; data = 0; };};

	class GeometryChange			: public Signal_void { public: GeometryChange() { type = (Uint16) __LINE__; data = 0; }; };

	class MenuEntryPressed			: public Signal_menuitem { public: MenuEntryPressed() { type = (Uint16) __LINE__; data = 0; };};
	class MenuEntrySelected			: public Signal_menuitem { public: MenuEntrySelected() { type = (Uint16) __LINE__; data = 0; };};
	class MenuEntryChecked			: public Signal_menuitem { public: MenuEntryChecked() { type = (Uint16) __LINE__; data = 0; };};
	class MenuEntryUnchecked		: public Signal_menuitem { public: MenuEntryUnchecked() { type = (Uint16) __LINE__; data = 0; };};

	class MenuOpened				: public Signal_void { public: MenuOpened() { type = (Uint16) __LINE__; data = 0; }; };
	class MenuClosed				: public Signal_void { public: MenuClosed() { type = (Uint16) __LINE__; data = 0; }; };

	class TreeEntrySelected			: public Signal_treeentry { public: TreeEntrySelected() { type = (Uint16) __LINE__; data = 0; };};
	class TreeEntryUnselected		: public Signal_treeentry { public: TreeEntryUnselected() { type = (Uint16) __LINE__; data = 0; };};
	class TreeEntryMarked			: public Signal_treeentry { public: TreeEntryMarked() { type = (Uint16) __LINE__; data = 0; };};
	class TreeEntryUnmarked			: public Signal_treeentry { public: TreeEntryUnmarked() { type = (Uint16) __LINE__; data = 0; };};
	class TreeEntryOpened			: public Signal_treeentry { public: TreeEntryOpened() { type = (Uint16) __LINE__; data = 0; };};
	class TreeEntryClosed			: public Signal_treeentry { public: TreeEntryClosed() { type = (Uint16) __LINE__; data = 0; };};

};

	
#endif	// WG_SIGNALS_DOT_H
