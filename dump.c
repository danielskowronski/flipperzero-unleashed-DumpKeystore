#include <stdint.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/icon.h>
#include "DumpKeystore_icons.h"
#include <lib/subghz/subghz_keystore.h>
#include <storage/storage.h>
#include <toolbox/hex.h>
#include <toolbox/stream/stream.h>
#include <flipper_format/flipper_format.h>
#include <flipper_format/flipper_format_i.h>
#define SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE 512
#define RECORD_STORAGE "storage"

int status=0;

static void draw_callback(Canvas* canvas, void* ctx) {
	UNUSED(ctx);
	canvas_clear(canvas);

	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 0, 7, "\"Please do not share");

	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 1, 14, "decrypted manufacture keys\"");

	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 0, 25, "\"Sharing them will bring some");

	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 2, 33, "discomfort to legal owners\"");

	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 0, 43, "\"And potential");

	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 1, 50, "legal action");

	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 6, 57, "against you\"");

	if (status==0) canvas_draw_icon(canvas, 65, 36, &I_Connect_me_62x31);
	if (status==1) canvas_draw_icon(canvas, 65, 36, &I_Auth_62x31);
	if (status==2) canvas_draw_icon(canvas, 65, 36, &I_Error_62x31);
}

static void input_callback(InputEvent* input_event, void* ctx) {
	furi_assert(ctx);
	FuriMessageQueue* event_queue = ctx;
	furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t dumpkeystore_app(void* p) {
	UNUSED(p);

	InputEvent event;
	FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
	ViewPort* view_port = view_port_alloc();
	view_port_draw_callback_set(view_port, draw_callback, NULL);
	view_port_input_callback_set(view_port, input_callback, event_queue);
	Gui* gui = furi_record_open(RECORD_GUI);
	gui_add_view_port(gui, view_port, GuiLayerFullscreen);

	char* decrypted_line = malloc(SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE);
	SubGhzKeystore* keystore = subghz_keystore_alloc();

	Storage* storage = furi_record_open(RECORD_STORAGE);
	FlipperFormat* flipper_format = flipper_format_file_alloc(storage);
	Stream* stream = flipper_format_get_raw_stream(flipper_format);

	if(!subghz_keystore_load(keystore, "/ext/subghz/assets/keeloq_mfcodes") || !flipper_format_file_open_always(flipper_format, "/ext/subghz/assets/keeloq_mfcodes.dumped")) {
		status=2;
	}
	else {
		for M_EACH(manufacture_code, *subghz_keystore_get_data(keystore), SubGhzKeyArray_t) {
			memset(decrypted_line, 0, SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE);
			int len = snprintf(
				decrypted_line,
				SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE,
				"%08lX%08lX:%hu:%s",
				(uint32_t)(manufacture_code->key >> 32),
				(uint32_t)manufacture_code->key,
				manufacture_code->type,
				furi_string_get_cstr(manufacture_code->name));
			FURI_LOG_E("manufacture_code: %s (%i)", decrypted_line, len);
			stream_write_cstring(stream, decrypted_line);
			stream_write_char(stream, '\n');
		}
		free(decrypted_line);
		flipper_format_free(flipper_format);
		furi_record_close(RECORD_STORAGE);
		status=1;
	}

	while(true) {
		furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);
		if(event.key == InputKeyBack) {
			break;
		}
	}

	furi_message_queue_free(event_queue);
	gui_remove_view_port(gui, view_port);
	view_port_free(view_port);
	furi_record_close(RECORD_GUI);

	return 0;
}
