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



CPPFLAGS = -I/usr/include/freetype2/ -I../../src/ -I../../src/base/ -I../../src/items/ -I../../src/interfaces/   \
  -I../../src/sizebrokers/ -I../../src/skins/ -I../../src/textmappers/ -I../../src/valueformatters/ \
  -I../../src/widgets/ -I../../src/widgets/capsules/ -I../../src/widgets/layers/ -I../../src/widgets/panels/ -I../../src/widgets/lists/ \
  -I../../src/gfxdevices/software/ -I../../src/gfxdevices/opengl/ -I../../src/fonts/freetype/

VPATH = ../../src/base:../../src/interfaces:../../src/sizebrokers:../../src/items:../../src/sizebrokers:../../src/skins:../../src/textmappers:../../src/valueformatters:../../src/widgets:../../src/widgets/capsules:../../src/widgets/layers:../../src/widgets/lists:../../src/widgets/panels:../../src/gfxdevices/software/:../../src/gfxdevices/opengl/:../../src/fonts/freetype:../../src/examples:$(OUTDIR):$(OBJDIR)

BASE = wg_anim.o \
  wg_base.o \
  wg_bitmapfont.o \
  wg_blob.o \
  wg_caret.o \
  wg_chain.o \
  wg_charbuffer.o \
  wg_charseq.o \
  wg_codepages.o \
  wg_color.o \
  wg_dummyfont.o \
  wg_finalizer.o \
  wg_font.o \
  wg_geo.o \
  wg_gfxanim.o \
  wg_gfxdevice.o \
  wg_inputhandler.o \
  wg_mempool.o \
  wg_memstack.o \
  wg_msg.o \
  wg_msglogger.o \
  wg_msgrouter.o \
  wg_nullgfxdevice.o \
  wg_object.o \
  wg_patches.o \
  wg_receiver.o \
  wg_resdb.o \
  wg_scrollbartarget.o \
  wg_string.o \
  wg_surface.o \
  wg_surfacefactory.o \
  wg_textlink.o \
  wg_textstyle.o \
  wg_textstylemanager.o \
  wg_texttool.o \
  wg_togglegroup.o \
  wg_util.o

INTERFACES = wg_columnheader.o \
  wg_interface.o \
  wg_modvalue.o \
  wg_slidespan.o \
  wg_span.o \
  wg_text.o \
  wg_value.o

ITEMS = wg_columnheaderitem.o \
  wg_edittextitem.o \
  wg_editvalueitem.o \
  wg_iconitem.o \
  wg_modvalueitem.o \
  wg_spanitem.o \
  wg_textbaseitem.o \
  wg_textitem.o \
  wg_valueitem.o

SIZEBROKERS = wg_scalepreferred_sizebroker.o \
  wg_sizebroker.o \
  wg_uniform_sizebroker.o

SKINS = wg_blockskin.o \
  wg_boxskin.o \
  wg_colorskin.o \
  wg_extendedskin.o \
  wg_skin.o

TEXTMAPPERS = wg_stdtextmapper.o \
  wg_textmapper.o

VALUEFORMATTERS = wg_standardformatter.o \
  wg_timeformatter.o \
  wg_valueformatter.o

WIDGETS = wg_animplayer.o \
  wg_button.o \
  wg_container.o \
  wg_filler.o \
  wg_fpsdisplay.o \
  wg_image.o \
  wg_lineeditor.o \
  wg_oscilloscope.o \
  wg_refreshbutton.o \
  wg_rulerlabels.o \
  wg_scrollbar.o \
  wg_simplevolumemeter.o \
  wg_textdisplay.o \
  wg_texteditor.o \
  wg_togglebutton.o \
  wg_valuedisplay.o \
  wg_volumemeter.o \
  wg_widget.o


CAPSULES = wg_capsule.o \
  wg_shadercapsule.o \
  wg_sizecapsule.o

LAYERS = wg_layer.o \
  wg_modallayer.o \
  wg_popuplayer.o

LISTS = wg_list.o \
  wg_packlist.o 

PANELS = wg_flexpanel.o \
  wg_lambdapanel.o \
  wg_packpanel.o \
  wg_panel.o \
  wg_rootpanel.o \
  wg_scrollpanel.o \
  wg_splitpanel.o \
  wg_stackpanel.o

lib_files = $(BASE) $(INTERFACES) $(ITEMS) $(SIZEBROKERS) $(SKINS) $(TEXTMAPPERS) $(VALUEFORMATTERS) $(WIDGETS) $(CAPSULES) $(LAYERS) $(LISTS) $(PANELS)

softgfx_files = wg_softgfxdevice.o wg_softsurface.o wg_softsurfacefactory.o

glgfx_files = wg_glgfxdevice.o wg_glsurface.o wg_glsurfacefactory.o

freetype_files = wg_freetypefont.o

default : lib softgfx example01
all : lib softgfx glgfx freetype examples
lib : libwondergui.a
softgfx : libwg_gfx_software.a
glgfx : libwg_gfx_opengl.a
freetype : libwg_font_freetype.a
examples : example01


libwondergui.a : $(lib_files)
	ar rcu $(OUTDIR)/libwondergui.a $(lib_files:%.o=$(OBJDIR)/%.o)
			
libwg_gfx_software.a : $(softgfx_files)
	ar rcu $(OUTDIR)/libwg_gfx_software.a $(softgfx_files:%.o=$(OBJDIR)/%.o)

libwg_gfx_opengl.a : $(glgfx_files)
	ar rcu $(OUTDIR)/libwg_gfx_opengl.a $(glgfx_files:%.o=$(OBJDIR)/%.o)

libwg_font_freetype.a : $(freetype_files)
	ar rcu $(OUTDIR)/libwg_font_freetype.a $(freetype_files:%.o=$(OBJDIR)/%.o)

example01 : libwondergui.a libwg_gfx_software.a example01.o
	$(CXX) -o $(OUTDIR)/example01 $(OBJDIR)/example01.o -L$(OUTDIR) -lSDL2 -lwg_gfx_software -lwondergui -lfreetype

.PHONY : clean init

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


