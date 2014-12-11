# This is the Makefile for building the WonderGUI library and related projects
# on a standard Linux/Unix machine. 
#-------------------------------------------------------------------------------------------
#
# EXTERNAL DEPENDENCIES:
#
# WonderGUI by default uses FreeType2 to render text and therefore has a
# dependency on the FreeType2 development package _UNLESS_ you remove
# the WG_USE_FREETYPE define from src/wg_userdefines.h.
#
# Most examples have a dependency on SDL2, but wondergui itself does not.
#
#-------------------------------------------------------------------------------------------
#
# TARGETS:
#
# default   Builds the wondergui library, the software gfxdevice and all example files
#			that only depend on the software gfxdevice, FreeType2 and SDL2.
# all  	    Builds the wondergui library, all gfxdevices and all examples.
# lib 		Just builds the wondergui library.
# softgfx   Just builds the software gfxdevice library.
# examples  Builds all the examples, which through dependencies probably builds everything.
# clean		Removes all temporary files and output files.
#
#--------------------------------------------------------------------------------------------

VERSION = debug				# Set to one of the ones listed in VERSIONS below.
CXX = g++					# C++ compiler for compiling. Note! Makefile still needs gcc for MAKEDEPEND
FLAGS = 					# General compiler flags, set version specific ones below.


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
  -I../../src/sizebrokers/ -I../../src/skins/ -I../../src/widgets/ -I../../src/widgets/capsules/ \
  -I../../src/widgets/layers/ -I../../src/widgets/panels/ -I../../src/widgets/lists/ -I../../src/gfxdevices/software/

VPATH = ../../src/base:../../src/interfaces:../../src/sizebrokers:../../src/items:../../src/skins:../../src/widgets:../../src/widgets/capsules:../../src/widgets/layers:../../src/widgets/lists:../../src/widgets/panels:../../src/gfxdevices/software/:../../src/examples:$(OUTDIR):$(OBJDIR)

BASE = wg_anim.o \
  wg_base.o \
  wg_bitmapglyphs.o \
  wg_blob.o \
  wg_caret2.o \
  wg_caret.o \
  wg_caretinstance.o \
  wg_chain.o \
  wg_charbuffer.o \
  wg_char.o \
  wg_charseq.o \
  wg_codepages.o \
  wg_color.o \
  wg_event.o \
  wg_eventfilter.o \
  wg_eventhandler.o \
  wg_eventlogger.o \
  wg_finalizer.o \
  wg_font.o \
  wg_geo.o \
  wg_gfxanim.o \
  wg_gfxdevice.o \
  wg_glyphset.o \
  wg_hook.o \
  wg_hookptr.o \
  wg_interface_valueholder.o \
  wg_mempool.o \
  wg_memstack.o \
  wg_menuitem.o \
  wg_nullgfxdevice.o \
  wg_object.o \
  wg_patches.o \
  wg_pen.o \
  wg_pointers.o \
  wg_resdb.o \
  wg_scrollbartarget.o \
  wg_sizescaler.o \
  wg_string.o \
  wg_surface.o \
  wg_surfacefactory.o \
  wg_textprop.o \
  wg_textpropmanager.o \
  wg_textstyle.o \
  wg_texttool.o \
  wg_togglegroup.o \
  wg_util.o \
  wg_valueformat.o \
  wg_vectorglyphs.o

ITEMS = wg_columnheaderfield.o \
  wg_editvaluefield.o \
  wg_iconfield.o \
  wg_legacytextfield.o \
  wg_modvaluefield.o \
  wg_spanitem.o \
  wg_textfield.o \
  wg_valuefield.o


INTERFACES = wg_columnheader.o \
  wg_edittext.o \
  wg_editvalue.o \
  wg_icon.o \
  wg_interface.o \
  wg_iwidgets.o \
  wg_modspan.o \
  wg_modtext.o \
  wg_modvalue.o \
  wg_slidespan.o \
  wg_span.o \
  wg_text.o \
  wg_value.o

SIZEBROKERS = wg_scalepreferred_sizebroker.o \
  wg_sizebroker.o \
  wg_uniform_sizebroker.o

SKINS = wg_blockskin.o \
  wg_boxskin.o \
  wg_colorskin.o \
  wg_extendedskin.o \
  wg_skin.o

WIDGETS = wg_animplayer.o \
  wg_button.o \
  wg_combobox.o \
  wg_container.o \
  wg_filler.o \
  wg_fpsdisplay.o \
  wg_image.o \
  wg_knob.o \
  wg_lineeditor.o \
  wg_menubar.o \
  wg_menu.o \
  wg_oscilloscope.o \
  wg_refreshbutton.o \
  wg_rulerlabels.o \
  wg_scrollbar.o \
  wg_simplevolumemeter.o \
  wg_switch.o \
  wg_textdisplay.o \
  wg_togglebutton.o \
  wg_valuedisplay.o \
  wg_valueeditor.o \
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
#  wg_tablelist.o

PANELS = wg_flexpanel.o \
  wg_packpanel.o \
  wg_panel.o \
  wg_rootpanel.o \
  wg_scrollpanel.o \
  wg_stackpanel.o \
  wg_vectorpanel.o

lib_files = $(BASE) $(INTERFACES) $(ITEMS) $(SIZEBROKERS) $(SKINS) $(WIDGETS) $(CAPSULES) $(LAYERS) $(LISTS) $(PANELS)

softgfx_files = wg_softgfxdevice.o wg_softsurface.o wg_softsurfacefactory.o

default : lib softgfx example01
all : lib softgfx examples
lib : libwondergui.a
softgfx : libgfxdevice_software.a
examples : example01


libwondergui.a : $(lib_files)
	ar rcu $(OUTDIR)/libwondergui.a $(lib_files:%.o=$(OBJDIR)/%.o)
			
libgfxdevice_software.a : $(softgfx_files)
	ar rcu $(OUTDIR)/libgfxdevice_software.a $(softgfx_files:%.o=$(OBJDIR)/%.o)

example01 : libwondergui.a libgfxdevice_software.a example01.o
	$(CXX) -o $(OUTDIR)/example01 $(OBJDIR)/example01.o -L$(OUTDIR) -lSDL2 -lgfxdevice_software -lwondergui -lfreetype

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


