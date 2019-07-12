#!/bin/bash

rm -rf imports
mkdir imports
cd imports
mkdir base
mkdir interfaces
mkdir gfxdevices
mkdir gfxdevices/opengl
mkdir gfxdevices/software
cd ..

cd ../../WonderGUI/src
cp wondergui.h	../../wg2/_kato_wg3/imports

cd base
cp wg_object.h \
   wg_blob.* wg_color.* wg_enumextras.* wg_geo.* wg_gfxdevice.* \
   wg_nullgfxdevice.* \
   wg_pointers.* \
   wg_stronginterfaceptr.* \
   wg_strongptr.* \
   wg_surface.* \
   wg_surfacefactory.* \
   wg_types.* \
   wg_util.* \
   wg_weakinterfaceptr.* \
   wg_weakptr.* \
   ../../../wg2/_kato_wg3/imports/base/

cd ../interfaces
cp wg_interface.* \
   ../../../wg2/_kato_wg3/imports/interfaces

cd ../gfxdevices/opengl
cp wg_glgfxdevice.* \
   wg_glshaders.* \
   wg_glsurface.* \
   wg_glsurfacefactory.* \
   ../../../../wg2/_kato_wg3/imports/gfxdevices/opengl

cd ../software
cp wg_softgfxdevice.* \
   wg_softsurface.* \
   wg_softsurfacefactory.* \
   ../../../../wg2/_kato_wg3/imports/gfxdevices/software/

