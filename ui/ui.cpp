#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "../libs/imgui/imgui.h"
#include "../libs/imgui/examples/imgui_impl_sdl.h"
#include "../libs/imgui/examples/imgui_impl_opengl3.h"
#include "../libs/imgui_club/imgui_memory_editor/imgui_memory_editor.h"
#include <GL/gl3w.h>

#include "ui.hpp"
#include "../nes/nes.h"

static SDL_Window *window = {0};
static SDL_GLContext gl_context;

int gfx_init()
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow("t-nes: OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    assert(window);

    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    assert(gl3wInit() == 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    return 0;
}

void gfx_exit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

static bool loaded_rom = false;

static void file_menu()
{
    if (ImGui::MenuItem("Open", "Ctrl+O"))
    {
        nes_loadrom("testroms/nestest.nes");
        cpu_power_up();
        loaded_rom = true;
    }
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("game1.ch8");
        ImGui::MenuItem("game2.ch8");
        ImGui::MenuItem("game3.ch8");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("MORE");
            ImGui::MenuItem("AND MORE");
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Quick Save", "Ctrl+S", false, false)) {}
    if (ImGui::MenuItem("Quick Load", "Ctrl+L", false, false)) {}
    if (ImGui::MenuItem("Save State")) {}
    if (ImGui::MenuItem("Load State")) {}
    if (ImGui::BeginMenu("Save State Slot"))
    {
        ImGui::MenuItem("Slot 0");
        ImGui::MenuItem("Slot 1");
        ImGui::MenuItem("Slot 2");
        ImGui::MenuItem("Slot 3");
        ImGui::MenuItem("Slot 4");
        ImGui::MenuItem("Slot 5");
        ImGui::MenuItem("Slot 6");
        ImGui::MenuItem("Slot 7");
        ImGui::MenuItem("Slot 8");
        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

void gfx_menubar()
{
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("File"))
        {
            file_menu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Emulation"))
        {
            static bool sel = false;
            if (ImGui::MenuItem(sel ? "Pause" : "Play", NULL, &sel)) {}
            if (ImGui::MenuItem("Stop")) {}
            if (ImGui::MenuItem("Reset")) {}
            ImGui::Separator();

            if (ImGui::MenuItem("Fast Forward")) {}
            if (ImGui::MenuItem("Rewind")) {}
            ImGui::Separator();

            if (ImGui::MenuItem("Fullscreen")) {}

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Configure...")) {}
            ImGui::Separator();

            if (ImGui::MenuItem("Graphics Settings")) {}
            if (ImGui::MenuItem("Audio Settings")) {}
            if (ImGui::MenuItem("Controller Settings")) {}
            if (ImGui::MenuItem("Hotkey Settings")) {}

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Debug")) {};
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Accent Colour");
            ImGui::MenuItem("Background Colour");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Info")) {}
            if (ImGui::MenuItem("Link To GitHub")) {}
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();
}

void gfx_start()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    gfx_menubar();
}

void gfx_end()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}

void gfx_debug()
{
    ImGui::Begin("Debug Time");
    {
        static int breakpoint = 0;
        static bool run = false;
        cpu_t *cpu = cpu_debug_get();

        if (run)
        {
            if (cpu->opcode == breakpoint)
            {
                run = false;
            }
            else
            for (int i =0; i < 1000; i++)
            cpu_debug_step();
        }

        ImGui::Text("Debug TIME! %s", loaded_rom ? "ROM LOADED" : "NA");
        ImGui::Separator();

        if (ImGui::Button("Step"))
        {
            cpu_debug_step();
        }

        if (ImGui::Button("Run"))
        {
            run = true;
        }

        ImGui::InputInt("set breakpoint", &breakpoint);

        ImGui::Text("Opcode 0x%X", cpu->opcode);
        ImGui::SameLine();
        ImGui::Text("Oprand 0x%X", cpu->oprand);
        ImGui::Separator();

        ImGui::Text("PC: 0x%X", cpu->reg.PC);
        ImGui::SameLine();
        ImGui::Text("SP: 0x%X", cpu->reg.SP);
        ImGui::Separator();
    }
    ImGui::End();
}

void ui()
{
    gfx_init();

    static bool quit = false;

    while (!quit)
    {
        static SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                default:
                    break;
            }
        }

        gfx_start();

        gfx_debug();

        gfx_end();
    }

    gfx_exit();
}