/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <unistd.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <quirc.h>
#include <time.h>
#include <getopt.h>

#include "camera.h"
#include "mjpeg.h"
#include "convert.h"
#include "dthash.h"
#include "demoutil.h"

/* Collected command-line arguments */
static const char *camera_path = "/dev/video0";
static int video_width = 640;
static int video_height = 480;
static int want_frame_rate = 0;
static int want_verbose = 0;
static int gain_request = -1;
static int printer_timeout = 2;

static void fat_text(SDL_Surface *screen, int x, int y, const char *text)
{
	int i, j;

	for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++)
			stringColor(screen, x + i, y + j, text, 0xffffffff);
	stringColor(screen, x, y, text, 0x008000ff);
}

static void fat_text_cent(SDL_Surface *screen, int x, int y, const char *text)
{
	x -= strlen(text) * 4;

	fat_text(screen, x, y, text);
}

static void draw_qr(SDL_Surface *screen, struct quirc *q, struct dthash *dt)
{
	int count = quirc_count(q);
	int i;

	for (i = 0; i < count; i++) {
		struct quirc_code code;
		struct quirc_data data;
		quirc_decode_error_t err;
		int j;
		int xc = 0;
		int yc = 0;
		char buf[128];

		quirc_extract(q, i, &code);

		for (j = 0; j < 4; j++) {
			struct quirc_point *a = &code.corners[j];
			struct quirc_point *b = &code.corners[(j + 1) % 4];

			xc += a->x;
			yc += a->y;
			lineColor(screen, a->x, a->y, b->x, b->y, 0x008000ff);
		}

		xc /= 4;
		yc /= 4;

		if (want_verbose) {
			snprintf(buf, sizeof(buf), "Code size: %d cells",
				 code.size);
			fat_text_cent(screen, xc, yc - 20, buf);
		}

		err = quirc_decode(&code, &data);

		if (err) {
			if (want_verbose)
				fat_text_cent(screen, xc, yc,
						quirc_strerror(err));
		} else {
			fat_text_cent(screen, xc, yc, (char *)data.payload);
			print_data(&data, dt, want_verbose);

			if (want_verbose) {
				snprintf(buf, sizeof(buf),
					 "Ver: %d, ECC: %c, Mask: %d, Type: %d",
					 data.version, "MLHQ"[data.ecc_level],
					 data.mask, data.data_type);
				fat_text_cent(screen, xc, yc + 20, buf);
			}
		}
	}
}

static int main_loop(struct camera *cam, SDL_Surface *screen,
		     struct quirc *q, struct mjpeg_decoder *mj)
{
	SDL_Event ev;
	time_t last_rate = 0;
	int frame_count = 0;
	char rate_text[64];
	struct dthash dt;

	rate_text[0] = 0;
	dthash_init(&dt, printer_timeout);

	for (;;) {
		time_t now = time(NULL);

		if (camera_update(cam) < 0)
			return -1;

		SDL_LockSurface(screen);
		switch (cam->format) {
		case CAMERA_FORMAT_MJPEG:
			mjpeg_decode_rgb32(mj, cam->mem, cam->mem_len,
					   screen->pixels, screen->pitch,
					   screen->w, screen->h);
			break;

		case CAMERA_FORMAT_YUYV:
			yuyv_to_rgb32(cam->mem, cam->width * 2,
				      cam->width, cam->height,
				      screen->pixels, screen->pitch);
			break;
		}

		rgb32_to_luma(screen->pixels, screen->pitch,
			      screen->w, screen->h,
			      quirc_begin(q, NULL, NULL),
			      screen->w);
		quirc_end(q);
		SDL_UnlockSurface(screen);

		draw_qr(screen, q, &dt);
		if (want_frame_rate)
			fat_text(screen, 5, 5, rate_text);
		SDL_Flip(screen);

		while (SDL_PollEvent(&ev) > 0) {
			if (ev.type == SDL_QUIT)
				return 0;

			if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == 'q')
				return 0;
		}

		if (now != last_rate) {
			snprintf(rate_text, sizeof(rate_text),
				 "Frame rate: %d fps", frame_count);
			frame_count = 0;
			last_rate = now;
		}

		frame_count++;
	}
}

static int run_demo(void)
{
	struct quirc *qr;
	struct camera cam;
	struct mjpeg_decoder mj;
	SDL_Surface *screen;
	int ret;

	if (camera_init(&cam, camera_path, video_width, video_height) < 0)
		return -1;

	camera_set_gain(&cam, gain_request);

	qr = quirc_new();
	if (!qr) {
		perror("couldn't allocate QR decoder");
		goto fail_qr;
	}

	if (quirc_resize(qr, cam.width, cam.height) < 0) {
		perror("couldn't allocate QR buffer");
		goto fail_qr_resize;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		perror("couldn't init SDL");
		goto fail_sdl_init;
	}

	screen = SDL_SetVideoMode(cam.width, cam.height, 32,
				  SDL_SWSURFACE | SDL_DOUBLEBUF);
	if (!screen) {
		perror("couldn't init video mode");
		goto fail_video_mode;
	}

	mjpeg_init(&mj);
	ret = main_loop(&cam, screen, qr, &mj);
	mjpeg_free(&mj);

	SDL_Quit();
	quirc_destroy(qr);
	camera_free(&cam);

	return 0;

fail_video_mode:
	SDL_Quit();
fail_qr_resize:
fail_sdl_init:
	quirc_destroy(qr);
fail_qr:
	camera_free(&cam);

	return 0;
}

static void usage(const char *progname)
{
	printf("Usage: %s [options]\n\n"
"Valid options are:\n\n"
"    -f             Show frame rate on screen.\n"
"    -v             Show extra data for detected codes.\n"
"    -d <device>    Specify camera device path.\n"
"    -s <WxH>       Specify video dimensions.\n"
"    -g <value>     Set camera gain.\n"
"    -p <timeout>   Set printer timeout (seconds).\n"
"    --help         Show this information.\n"
"    --version      Show library version information.\n",
	progname);
}

int main(int argc, char **argv)
{
	const static struct option longopts[] = {
		{"help",		0, 0, 'H'},
		{"version",		0, 0, 'V'},
		{NULL,			0, 0, 0}
	};
	int opt;

	printf("quirc demo\n");
	printf("Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>\n");
	printf("\n");

	while ((opt = getopt_long(argc, argv, "d:s:fvg:p:",
				  longopts, NULL)) >= 0)
		switch (opt) {
		case 'V':
			printf("Library version: %s\n", quirc_version());
			return 0;

		case 'H':
			usage(argv[0]);
			return 0;

		case 'v':
			want_verbose = 1;
			break;

		case 'f':
			want_frame_rate = 1;
			break;

		case 's':
			if (parse_size(optarg, &video_width, &video_height) < 0)
				return -1;
			break;

		case 'p':
			printer_timeout = atoi(optarg);
			break;

		case 'd':
			camera_path = optarg;
			break;

		case 'g':
			gain_request = atoi(optarg);
			break;

		case '?':
			fprintf(stderr, "Try --help for usage information\n");
			return -1;
		}

	return run_demo();
}
