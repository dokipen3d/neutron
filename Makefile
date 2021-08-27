# Installation directory.
INSTDIR = $(HOME)/Library/Preferences/houdini/16.0
# List of C++ source files to build.
# SOP_Main.C registers the operators and handles the DSO-specifics.
SOURCES = \
    ./SOP_Neutron.C \
    ./GUI_Neutron.C \
	./utility_Neutron.C
# Icons for custom node operators.
# ICONS = \
#     SOP_cone.svg \
#     SOP_wave.svg \
#     SOP_cut.svg \
#     SOP_bend.svg
# Use the highest optimization level.
OPTIMIZER = -O3
# Additional include directories.
INCDIRS = -I/usr/local/include/
# INCDIRS = \
#     -I/opt/studio/sdk/include \
#     -I/opt/studio/third_party/sdk/include
# Additional library directories.
# LIBDIRS = \
#     -L/opt/studio/sdk/lib \
#     -L/opt/studio/third_party/sdk/lib
# Additional libraries.
# LIBS = \
#     -lstudioSDK \
#     -lthird_partySDK
# Set the plugin library name.
DSONAME = Neutron.dylib
# Include the GNU Makefile.
include $(HFS)/toolkit/makefiles/Makefile.gnu