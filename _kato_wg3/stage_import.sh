#!/bin/bash

rm -rf imports
mkdir imports
cd imports
mkdir baselibrary
mkdir baselibrary/base
mkdir baselibrary/components
mkdir gfxdevices
mkdir gfxdevices/opengl
mkdir gfxdevices/software
mkdir gfxdevices/metal
mkdir baselibrary/skins
mkdir fontengines
cd ..

cd ../../WonderGUI/src

cd baselibrary/base
cp wg_base.* \
   wg_object.* \
   wg_bitmapfont.* \
   wg_blob.* \
   wg_canvaslayers.* \
   wg_chain.* \
   wg_char.* \
   wg_charbuffer.* \
   wg_charseq.* \
   wg_codepages.* \
   wg_color.* \
   wg_context.* \
   wg_dataset.* \
   wg_dummyfont.* \
   wg_enumextras.* \
   wg_font.* \
   wg_geo.* \
   wg_gfxdevice.* \
   wg_gradient.* \
   wg_hostbridge.* \
   wg_internal.* \
   wg_mempool.* \
   wg_memstack.* \
   wg_nullgfxdevice.* \
   wg_patches.* \
   wg_pointers.* \
   wg_mu.* \
   wg_skinslotmanager.* \
   wg_string.* \
   wg_strongcomponentptr.* \
   wg_strongptr.* \
   wg_surface.* \
   wg_surfacefactory.* \
   wg_textlink.* \
   wg_textstyle.* \
   wg_textstylemanager.* \
   wg_texttool.* \
   wg_types.* \
   wg_util.* \
   wg_weakcomponentptr.* \
   wg_weakptr.* \
   ../../../../wg2/_kato_wg3/imports/baselibrary/base

cd ../components
cp wg_component.* \
   wg_cstaticvector.* \
   wg_cdynamicvector.* \
   wg_canimframes.* \
   ../../../../wg2/_kato_wg3/imports/baselibrary/components

cd ../skins
cp -r * ../../../../wg2/_kato_wg3/imports/baselibrary/skins

cd ../../gfxdevices/opengl
cp * ../../../../wg2/_kato_wg3/imports/gfxdevices/opengl

cd ../metal
cp * ../../../../wg2/_kato_wg3/imports/gfxdevices/metal

cd ../software
cp * ../../../../wg2/_kato_wg3/imports/gfxdevices/software/

cd ../../fontengines
cp -r * ../../../wg2/_kato_wg3/imports/fontengines

