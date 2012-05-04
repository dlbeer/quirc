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

#ifndef CAMERA_H_
#define CAMERA_H_

#include <stdint.h>

#define CAMERA_BUFFERS		8

typedef enum {
	CAMERA_FORMAT_MJPEG,
	CAMERA_FORMAT_YUYV
} camera_format_t;

struct camera {
	/* File descriptor for camera device. */
	int			fd;

	/* Width, height and frame pixel format. Once initialized,
	 * these values will not change.
	 */
	int			width;
	int			height;
	camera_format_t		format;

	/* Buffer information for the current frame. If mem_index < 0,
	 * then no frame is currently available.
	 */
	uint8_t			*mem;
	int			mem_len;
	int			mem_index;

	/* Pointers and sizes of memory-mapped buffers. */
	uint8_t			*buffer_maps[CAMERA_BUFFERS];
	int			buffer_lens[CAMERA_BUFFERS];
};

/* Set up a camera driver. This opens the video device and configures for
 * the given resolution. Streaming is started immediately.
 *
 * Returns 0 on success or -1 if an error occurs.
 */
int camera_init(struct camera *cam, const char *path, int width, int height);

/* Set the gain control for the camera device. */
int camera_set_gain(struct camera *cam, int gain);

/* Shut down the camera device and free allocated memory. */
void camera_free(struct camera *cam);

/* Retrieve the latest frame from the video queue and fill out the mem and
 * mem_len fields in the camera struct.
 */
int camera_update(struct camera *cam);

#endif
