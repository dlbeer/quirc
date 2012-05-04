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

#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <jpeglib.h>
#include "dbgutil.h"

void dump_data(const struct quirc_data *data)
{
	printf("    Version: %d\n", data->version);
	printf("    ECC level: %c\n", "MLHQ"[data->ecc_level]);
	printf("    Mask: %d\n", data->mask);
	printf("    Data type: %d\n", data->data_type);
	printf("    Length: %d\n", data->payload_len);
	printf("    Payload: %s\n", data->payload);
}

void dump_cells(const struct quirc_code *code)
{
	int u, v;

	printf("    %d cells, corners:", code->size);
	for (u = 0; u < 4; u++)
		printf(" (%d,%d)", code->corners[u].x,
				   code->corners[u].y);
	printf("\n");

	for (v = 0; v < code->size; v++) {
		printf("    ");
		for (u = 0; u < code->size; u++) {
			int p = v * code->size + u;

			if (code->cell_bitmap[p >> 3] & (1 << (p & 7)))
				printf("[]");
			else
				printf("  ");
		}
		printf("\n");
	}
}

struct my_jpeg_error {
	struct jpeg_error_mgr   base;
	jmp_buf                 env;
};

static void my_output_message(struct jpeg_common_struct *com)
{
	struct my_jpeg_error *err = (struct my_jpeg_error *)com->err;
	char buf[JMSG_LENGTH_MAX];

	err->base.format_message(com, buf);
	fprintf(stderr, "JPEG error: %s", buf);
}

static void my_error_exit(struct jpeg_common_struct *com)
{
	struct my_jpeg_error *err = (struct my_jpeg_error *)com->err;

	my_output_message(com);
	longjmp(err->env, 0);
}

static struct jpeg_error_mgr *my_error_mgr(struct my_jpeg_error *err)
{
	jpeg_std_error(&err->base);

	err->base.error_exit = my_error_exit;
	err->base.output_message = my_output_message;

	return &err->base;
}

int load_jpeg(struct quirc *q, const char *filename)
{
	FILE *infile = fopen(filename, "rb");
	struct jpeg_decompress_struct dinfo;
	struct my_jpeg_error err;
	uint8_t *image;
	int y;

	if (!infile) {
		perror("can't open input file");
		return -1;
	}

	memset(&dinfo, 0, sizeof(dinfo));
	dinfo.err = my_error_mgr(&err);

	if (setjmp(err.env))
		goto fail;

	jpeg_create_decompress(&dinfo);
	jpeg_stdio_src(&dinfo, infile);

	jpeg_read_header(&dinfo, TRUE);
	dinfo.output_components = 1;
	dinfo.out_color_space = JCS_GRAYSCALE;
	jpeg_start_decompress(&dinfo);

	if (dinfo.output_components != 1) {
		fprintf(stderr, "Unexpected number of output components: %d",
			 dinfo.output_components);
		goto fail;
	}

	if (quirc_resize(q, dinfo.output_width, dinfo.output_height) < 0)
		goto fail;

	image = quirc_begin(q, NULL, NULL);

	for (y = 0; y < dinfo.output_height; y++) {
		JSAMPROW row_pointer = image + y * dinfo.output_width;

		jpeg_read_scanlines(&dinfo, &row_pointer, 1);
	}

	fclose(infile);
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
	return 0;

fail:
	fclose(infile);
	jpeg_destroy_decompress(&dinfo);
	return -1;
}
