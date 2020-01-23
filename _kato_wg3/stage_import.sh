#!/bin/bash

rm -rf imports
mkdir imports
cd imports
mkdir base
mkdir components
mkdir gfxdevices
mkdir gfxdevices/opengl
mkdir gfxdevices/software
cd ..

cd ../../WonderGUI/src

cd base
cp wg_object.h \
   wg_blob.* \
   wg_color.* \
   wg_enumextras.* \
   wg_geo.* \
   wg_gfxdevice.* \
   wg_nullgfxdevice.* \
   wg_patches.* \
   wg_payload.* \
   wg_pointers.* \
   wg_qpix.* \
   wg_strongcomponentptr.* \
   wg_strongptr.* \
   wg_surface.* \
   wg_surfacefactory.* \
   wg_types.* \
   wg_util.* \
   wg_weakcomponentptr.* \
   wg_weakptr.* \
   ../../../wg2/_kato_wg3/imports/base/

cd ../components
cp wg_component.* \
   ../../../wg2/_kato_wg3/imports/components

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

