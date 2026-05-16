#pragma once

#include "common.hpp"
#include "template.hpp"
#include "math_util.hpp"
#include "ui.hpp"
#include "asset.hpp"
#include "input.hpp"
#include "draw.hpp"

#include "language/lang.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

enum ApplicationMode {
    ModeMenu,
    ModeGame,

    ModeCount,
};

enum MenuName {
    MenuMain,
    MenuSettings,
};

enum UiStates {
    UiMainMenu,
    UiSettings,
    UiGame,
    UiCount,
};

struct Window {
    SDL_Window* window;
};

struct Event_Timeout {
    s64 event = 0;
    bool active = false;
};

#define NS_PER_SECONDS 1'000'000'000

// about 11 and a half days
#define EVENT_TIMEOUT_LONG 1000000.0

enum Events {
    EVENT_DUMMY,
    EVENT_COUNT,
};

class Application {
public:
    ApplicationMode m_mode = ModeMenu;
    MenuName m_menu = MenuMain;

    Window m_window = {};
    RenderContext m_render = {};
    AudioPlayer m_audio_player = {};
    Input m_input = {};
    AssetCatalog m_catalog = {};

    UiState m_ui[UiCount];
    Color m_background_color = DEFAULT_BACKGROUND_COLOR;

    s64 m_time = 0;
    double m_time_seconds = 0;

    DArray<Interp*> programs = {};

    Event_Timeout m_events[EVENT_COUNT] = {};

    DArray<Text> m_rendered_text = {};

    AssetId m_font = {};
    AssetId m_editor_font = {};

    bool quit = false;
    bool doing_text_input = false;

    bool initialize();

    void handle_events();
    void update();
    void draw();

    void cleanup();
private:
    void init_ui();
    bool load_assets();

    UiState& get_active_ui();

    void timeout();
    void update_ui_state(vec2 window_size);
    void update_ui_pos();

    void set_event_active(int event_index, double timeout_seconds);
    void set_event_deactive(int event_index);

    void draw_game();
	void draw_ui();

    void draw_main_menu();
    void draw_settings_menu();
    void draw_game_ui();

    void draw_ui_state(const UiState& state);

    bool on_mouse_down();
    void on_mouse_up();

    bool mouse_input_game();
    bool mouse_input_menu();
    bool mouse_input_main_menu();
    bool mouse_input_settings();

    void update_keyboard_state();
    bool keyboard_input(SDL_KeyboardEvent keyboard);

    bool gen_static_text(Color color);

    void text_input_start();
    void text_input_stop();
    void toggle_text_input();

    bool read_asset_catalog(String_Builder& path);

    bool set_eval_string(String s);
    bool set_eval_string_left(String s);
    bool set_eval_string_right(String s);

    void render_rectangle(Rectangle rect, Color color, bool center = true) const;
    void render_textured_rectangle(Rectangle rect, SDL_Texture* texture, Color color, bool strech = false, bool center = true) const;

    void render_slider(Rectangle area, vec2 knob_scale, float value, Color slider_color, Color knob_color, const Text& text) const;
    void render_text_field(const Text_Field& text_field) const;
    void render_text_editor(const TextEditor& editor) const;
    void render_dropdown(const Drop_Down_List& list) const;

    void clear_text_input_selection();

    void switch_modes(ApplicationMode mode);
    void switch_menu(MenuName menu);

    bool is_fullscreen() const;
    vec2 get_window_size() const;
};

void get_base_path(String_Builder& builder);
