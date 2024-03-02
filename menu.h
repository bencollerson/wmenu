#ifndef WMENU_MENU_H
#define WMENU_MENU_H

#include <xkbcommon/xkbcommon.h>

#include "pool-buffer.h"

// A menu item.
struct item {
	char *text;
	int width;
	struct item *next;       // traverses all items
	struct item *prev_match; // previous matching item
	struct item *next_match; // next matching item
	struct page *page;       // the page holding this item
};

// A page of menu items.
struct page {
	struct item *first; // first item in the page
	struct item *last;  // last item in the page
	struct page *prev;  // previous page
	struct page *next;  // next page
};

// A Wayland output.
struct output {
	struct menu *menu;
	struct wl_output *output;
	const char *name;    // output name
	int32_t scale;       // output scale
	struct output *next; // next output
};

// Keyboard state.
struct keyboard {
	struct menu *menu;
	struct wl_keyboard *keyboard;
	struct xkb_context *context;
	struct xkb_keymap *keymap;
	struct xkb_state *state;

	int repeat_timer;
	int repeat_delay;
	int repeat_period;
	enum wl_keyboard_key_state repeat_key_state;
	xkb_keysym_t repeat_sym;
};

// Menu state.
struct menu {
	// Whether the menu appears at the bottom of the screen
	bool bottom;
	// The function used to match menu items
	int (*strncmp)(const char *, const char *, size_t);
	// The font used to display the menu
	char *font;
	// The number of lines to list items vertically
	int lines;
	// The name of the output to display on
	char *output_name;
	// The prompt displayed to the left of the input field
	char *prompt;
	// Normal colors
	uint32_t normalbg, normalfg;
	// Prompt colors
	uint32_t promptbg, promptfg;
	// Selection colors
	uint32_t selectionbg, selectionfg;

	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	struct wl_seat *seat;
	struct wl_data_device_manager *data_device_manager;
	struct zwlr_layer_shell_v1 *layer_shell;
	struct output *output_list;

	struct keyboard *keyboard;
	struct wl_data_device *data_device;
	struct wl_surface *surface;
	struct zwlr_layer_surface_v1 *layer_surface;
	struct wl_data_offer *data_offer;
	struct output *output;

	struct pool_buffer buffers[2];
	struct pool_buffer *current;

	int width;
	int height;
	int line_height;
	int padding;
	int inputw;
	int promptw;
	int left_arrow;
	int right_arrow;

	char input[BUFSIZ];
	size_t cursor;

	struct item *items;       // list of all items
	struct item *matches;     // list of matching items
	struct item *matches_end; // last matching item
	struct item *sel;         // selected item
	struct page *pages;       // list of pages

	bool exit;
	bool failure;
};

struct menu *menu_create();
struct keyboard *keyboard_create(struct menu *menu, struct wl_keyboard *wl_keyboard);
void menu_set_keyboard(struct menu *menu, struct keyboard *keyboard);
struct output *output_create(struct menu *menu, struct wl_output *wl_output);
void menu_add_output(struct menu *menu, struct output *output);
void menu_getopts(struct menu *menu, int argc, char *argv[]);
void read_menu_items(struct menu *menu);
void menu_keypress(struct menu *menu, enum wl_keyboard_key_state key_state,
		xkb_keysym_t sym);
void menu_destroy(struct menu *menu);

#endif
