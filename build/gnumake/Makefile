# This is the Makefile for building the WonderGUI library and related projects
# on a standard Linux/Unix machine. 
#-------------------------------------------------------------------------------------------
#
# EXTERNAL DEPENDENCIES:
#
# The WonderGUI library itself has no dependencies.
#
# The freetype support library naturally has a dependency on freetype2.
# The opengl support library has a dependency on openGL.
#
# Most examples have a dependency on SDL2 and SDL2_Image.
#
#-------------------------------------------------------------------------------------------
#
# TARGETS:
#
# default   Builds the wondergui library, the software gfxdevice and all example files
#			that only depend on the software gfxdevice, FreeType2 and SDL2.
# all  	    Builds the wondergui library, all gfxdevices, fontsystems and all examples.
# lib 		Just builds the wondergui library.
# softgfx   Just builds the software gfxdevice library.
# glgfx     Just builds the openGL gfxdevice library.
# streamgfx Just builds the stream gfxdevice library.
# freetype  Just builds the freetype fontsystem library.
# examples  Builds all the examples, which through dependencies probably builds everything.
# clean		Removes all temporary files and output files.
#
#--------------------------------------------------------------------------------------------

# Set to one of the ones listed in VERSIONS below.
VERSION = debug

# C++ compiler for compiling. Note! Makefile still needs gcc for MAKEDEPEND
CXX = g++

# General compiler flags, set version specific ones below.
FLAGS = -std=c++11


# List of versions that can be built. You can add your own, but should run 'make clean' afterwards
# for updating the directory structure. 

VERSIONS = debug release 	

### SETTINGS FOR THE DIFFERENT VERSIONS, FEEL FREE TO CHANGE ###

ifeq ($(VERSION),debug)
	CXXFLAGS = $(FLAGS) -g -Og
endif

ifeq ($(VERSION),release)
	CXXFLAGS = $(FLAGS) -O3
endif

####################################################################
### YOU PROBABLY SHOULDN'T TOUCH ANYTHING FROM HERE DOWNWARDS... ###
####################################################################

MAKEDEPEND = gcc -M $(CPPFLAGS) -o $*.d $<
OUTDIR = $(VERSION)
OBJDIR = $(OUTDIR)/obj
DEPDIR = deps



CPPFLAGS = -I/usr/include/freetype2/ -I../../src/ -I../../src/base/ -I../../src/components/ \
  -I../../src/sizebrokers/ -I../../src/skins/ -I../../src/skins/meter/ -I../../src/skins/misc/ -I../../src/skins/state/ -I../../src/skins/static/ -I../../src/textmappers/ -I../../src/valueformatters/ \
  -I../../src/widgets/ -I../../src/widgets/capsules/ -I../../src/widgets/layers/ -I../../src/widgets/panels/ -I../../src/widgets/lists/ \
  -I../../src/gfxdevices/software/ -I../../src/gfxdevices/opengl/ -I../../src/gfxdevices/stream/ -I../../src/fonts/freetype/

VPATH = ../../src/base::../../src/sizebrokers:../../src/components:../../src/sizebrokers:../../src/skins:../../src/skins/meter:../../src/skins/misc:../../src/skins/state:../../src/skins/static:../../src/textmappers:../../src/valueformatters:../../src/widgets:../../src/widgets/capsules:../../src/widgets/layers:../../src/widgets/lists:../../src/widgets/panels:../../src/gfxdevices/software/:../../src/gfxdevices/opengl/:../../src/gfxdevices/stream/:../../src/fonts/freetype:../../src/examples:$(OUTDIR):$(OBJDIR)

BASE = \
  wg_base.o \
  wg_bitmapfont.o \
  wg_blob.o \
  wg_caret.o \
  wg_chain.o \
  wg_charbuffer.o \
  wg_charseq.o \
  wg_codepages.o \
  wg_color.o \
  wg_context.o \
  wg_dataset.o \
  wg_dummyfont.o \
  wg_enumextras.o \
  wg_font.o \
  wg_geo.o \
  wg_gfxdevice.o \
  wg_gfxstreamlogger.o \
  wg_gfxstreamplayer.o \
  wg_gfxstreamplug.o \
  wg_gfxstreamreader.o \
  wg_gfxstreamwriter.o \
  wg_inputhandler.o \
  wg_mempool.o \
  wg_memstack.o \
  wg_msg.o \
  wg_msglogger.o \
  wg_msgrouter.o \
  wg_mu.o \
  wg_nullgfxdevice.o \
  wg_object.o \
  wg_patches.o \
  wg_receiver.o \
  wg_resdb.o \
  wg_scrollbartarget.o \
  wg_slot.o \
  wg_string.o \
  wg_surface.o \
  wg_surfacefactory.o \
  wg_textlink.o \
  wg_textstyle.o \
  wg_textstylemanager.o \
  wg_texttool.o \
  wg_togglegroup.o \
  wg_util.o

COMPONENTS = \
  wg_ccanvas.o \
  wg_canimframes.o \
  wg_ccanvas.o \
  wg_ccolumnheader.o \
  wg_cdynamicslotcollection.o \
  wg_cgfxinstream.o \
  wg_cgfxoutstream.o \
  wg_cicondisplay.o \
  wg_component.o \
  wg_cshadows.o \
  wg_cslot.o \
  wg_cstandardslot.o \
  wg_cstaticslotcollection.o \
  wg_cstatictextdisplay.o \
  wg_cstaticvaluedisplay.o \
  wg_ctext.o \
  wg_ctextdisplay.o \
  wg_ctexteditor.o \
  wg_cvaluedisplay.o \
  wg_cvalueeditor.o \
  wg_geocomponent.o


SIZEBROKERS = \
  wg_scalepreferred_sizebroker.o \
  wg_sizebroker.o \
  wg_uniform_sizebroker.o

SKINS = \
  wg_skin.o \
  wg_stateskin.o \
  wg_fillmeterskin.o \
  wg_framemeterskin.o \
  wg_piemeterskin.o \
  wg_spinmeterskin.o \
  wg_bakeskin.o \
  wg_doubleskin.o \
  wg_blockskin.o \
  wg_boxskin.o \
  wg_colorskin.o \
  wg_lambdaskin.o \
  wg_multiblockskin.o \
  wg_tileskin.o \
  wg_staticblockskin.o \
  wg_staticboxskin.o \
  wg_staticcolorskin.o \
  wg_statictileskin.o

TEXTMAPPERS = \
  wg_stdtextmapper.o \
  wg_textmapper.o

VALUEFORMATTERS = \
  wg_standardformatter.o \
  wg_timeformatter.o \
  wg_valueformatter.o

WIDGETS = \
  wg_animplayer.o \
  wg_button.o \
  wg_canvas.o \
  wg_container.o \
  wg_filler.o \
  wg_fpsdisplay.o \
  wg_image.o \
  wg_knob.o \
  wg_lineeditor.o \
  wg_pianokeyboard.o \
  wg_popupopener.o \
  wg_rangeslider.o \
  wg_rulerlabels.o \
  wg_scrollbar.o \
  wg_selectbox.o \
  wg_sidecanvas.o \
  wg_simplevolumemeter.o \
  wg_slider.o \
  wg_textdisplay.o \
  wg_texteditor.o \
  wg_timer.o \
  wg_togglebutton.o \
  wg_valuedisplay.o \
  wg_volumemeter.o \
  wg_widget.o


CAPSULES = \
  wg_canvasstack.o \
  wg_capsule.o \
  wg_shadercapsule.o \
  wg_sizecapsule.o

LAYERS = \
  wg_designlayer.o \
  wg_dragndroplayer.o \
  wg_layer.o \
  wg_modallayer.o \
  wg_popuplayer.o

LISTS = \
  wg_list.o \
  wg_packlist.o 

PANELS = \
  wg_flexpanel.o \
  wg_lambdapanel.o \
  wg_packpanel.o \
  wg_panel.o \
  wg_rootpanel.o \
  wg_scrollpanel.o \
  wg_splitpanel.o \
  wg_stackpanel.o

lib_files = $(BASE) $(COMPONENTS) $(SIZEBROKERS) $(SKINS) $(TEXTMAPPERS) $(VALUEFORMATTERS) $(WIDGETS) $(CAPSULES) $(LAYERS) $(LISTS) $(PANELS)

softgfx_files = wg_softgfxdevice.o wg_softsurface.o wg_softsurfacefactory.o

glgfx_files = wg_glgfxdevice.o wg_glsurface.o wg_glsurfacefactory.o

streamgfx_files = wg_streamgfxdevice.o wg_streamsurface.o wg_streamsurfacefactory.o

freetype_files = wg_freetypefont.o

default : lib softgfx example01
all : lib softgfx glgfx streamgfx freetype examples
lib : libwondergui.a
softgfx : libwg_gfx_software.a
glgfx : libwg_gfx_opengl.a
streamgfx : libwg_gfx_stream.a
freetype : libwg_font_freetype.a
examples : example01


libwondergui.a : $(lib_files)
	ar rcu $(OUTDIR)/libwondergui.a $(lib_files:%.o=$(OBJDIR)/%.o)
			
libwg_gfx_software.a : $(softgfx_files)
	ar rcu $(OUTDIR)/libwg_gfx_software.a $(softgfx_files:%.o=$(OBJDIR)/%.o)

libwg_gfx_opengl.a : $(glgfx_files)
	ar rcu $(OUTDIR)/libwg_gfx_opengl.a $(glgfx_files:%.o=$(OBJDIR)/%.o)

libwg_gfx_stream.a : $(streamgfx_files)
	ar rcu $(OUTDIR)/libwg_gfx_stream.a $(streamgfx_files:%.o=$(OBJDIR)/%.o)


libwg_font_freetype.a : $(freetype_files)
	ar rcu $(OUTDIR)/libwg_font_freetype.a $(freetype_files:%.o=$(OBJDIR)/%.o)

example01 : libwondergui.a libwg_gfx_software.a example01.o
	$(CXX) -o $(OUTDIR)/example01 $(OBJDIR)/example01.o -L$(OUTDIR) -lSDL2 -lwg_gfx_software -lwondergui -lfreetype

.PHONY : clean init

clean :
	rm -rf $(VERSIONS)
	mkdir -p $(VERSIONS:%=%/obj) $(DEPDIR)
	rm -f $(DEPDIR)/*
	

df = $(DEPDIR)/$(*F)

%.o:%.cpp
	@$(MAKEDEPEND); \
	cp $(*F).d $(df).d; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(*F).d >> $(df).d; \
	rm -f $(*F).d	
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@


DEPS = $(lib_files) $(softgfx_files) example01.o

-include $(DEPS:%.o=$(DEPDIR)/%.d)


