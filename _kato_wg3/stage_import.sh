#!/bin/bash

rm -rf imports
mkdir imports
cd imports
mkdir baselibrary
mkdir baselibrary/base
mkdir baselibrary/c-interface
mkdir baselibrary/components
mkdir baselibrary/skins
mkdir baselibrary/gfxstream
mkdir c-abi-wrapper
mkdir gfxdevices
mkdir gfxdevices/opengl
mkdir gfxdevices/software
mkdir gfxdevices/metal
mkdir gfxdevices/stream
mkdir fontengines
cd ..

cd ../../WonderGUI/src

cd baselibrary/base
cp wg_base.* \
   wg_object.* \
   wg_bitmapfont.* \
   wg_bitmapcache.* \
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
   wg_gridwalker.* \
   wg_hostbridge.* \
   wg_mempool.* \
   wg_memstack.* \
   wg_nullgfxdevice.* \
   wg_patches.* \
   wg_pointers.* \
   wg_skinslotmanager.* \
   wg_state.*\
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

cd ../c-interface
cp * ../../../../wg2/_kato_wg3/imports/baselibrary/c-interface


cd ../components
cp wg_component.* \
   wg_cstaticvector.* \
   wg_cdynamicvector.* \
   wg_canimframes.* \
   wg_cgfxstreamsink.* \
   wg_cgfxstreamsource.* \
   ../../../../wg2/_kato_wg3/imports/baselibrary/components


cd  ../gfxstream
cp * ../../../../wg2/_kato_wg3/imports/baselibrary/gfxstream

cd ../skins
cp -r * ../../../../wg2/_kato_wg3/imports/baselibrary/skins

cd ../../c-abi-wrapper
cp * ../../../wg2/_kato_wg3/imports/c-abi-wrapper

cd ../gfxdevices/opengl
cp * ../../../../wg2/_kato_wg3/imports/gfxdevices/opengl

cd ../metal
cp * ../../../../wg2/_kato_wg3/imports/gfxdevices/metal

cd ../software
cp * ../../../../wg2/_kato_wg3/imports/gfxdevices/software/

cd ../stream
cp * ../../../../wg2/_kato_wg3/imports/gfxdevices/stream/

cd ../../fontengines
cp -r * ../../../wg2/_kato_wg3/imports/fontengines

