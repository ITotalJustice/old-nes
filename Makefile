# TotalJustice

EXE			= t-nes

# Main source file.
SOURCES 	= main.c

# UI
SOURCES		+= ui/ui.cpp

# Nes files
SOURCES 	+= ../nes/nes.c ../nes/cpu.c nes/ppu.c nes/apu.c nes/cart.c

# imgui
SOURCES		+= libs/imgui/imgui.cpp libs/imgui/imgui_widgets.cpp libs/imgui/imgui_draw.cpp libs/imgui/imgui_demo.cpp

# imgui specific
SOURCES		+= libs/imgui/examples/imgui_impl_sdl.cpp libs/imgui/examples/imgui_impl_opengl3.cpp

# Libs
LIBS		= -lGL -ldl `sdl2-config --libs`

CXXFLAGS	= -I./libs/imgui -I./libs/imgui/examples/

CXXFLAGS	+= -march=native -Wall -Wformat -O3 -ffunction-sections -fno-exceptions -Wl,--gc-sections

OBJS		= $(addsuffix .o, $(basename $(notdir $(SOURCES))))

# Using OpenGL loader: gl3w
SOURCES		+= libs/imgui/examples/libs/gl3w/GL/gl3w.c
CXXFLAGS	+= -Ilibs/imgui/examples/libs/gl3w -DIMGUI_IMPL_OPENGL_LOADER_GL3W

CFLAGS		= $(CXXFLAGS)


##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
%.o:%.c
	$(CC) $(CXXFLAGS) -c -o $@ $<

%.o:ui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:nes/%.c
	$(CC) $(CXXFLAGS) -c -o $@ $<

%.o:libs/imgui/examples/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:libs/imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:libs/imgui/examples/libs/gl3w/GL/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o:libs/imgui/examples/libs/glad/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(EXE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)

run: all
	./$(EXE)