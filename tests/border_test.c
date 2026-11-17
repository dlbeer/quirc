/* Regression test for QR codes touching the image edges (issue #138).
 * Build and run with: make border_test && ./border_test
 */

#include <stdio.h>
#include <string.h>
#include "quirc.h"

#define QR_SIZE 25

/* Version 2, ECC M, mask 2. Generated with Project Nayuki's QR encoder.
 * The payload is "quirc border regression".
 */
static const char qr[QR_SIZE][QR_SIZE + 1] = {
	"#######..#..#...#.#######",
	"#.....#....#..##..#.....#",
	"#.###.#.####..#...#.###.#",
	"#.###.#.#.#..####.#.###.#",
	"#.###.#.##.##.#.#.#.###.#",
	"#.....#.#...##.##.#.....#",
	"#######.#.#.#.#.#.#######",
	"........#####.#.#........",
	"#.#####.....####..#####..",
	"#.##.#...##.#.#.#..#.##..",
	"##.#####.#####.####.#..##",
	"#.###..##.....###.#....##",
	"###.####..######.####.###",
	"#.#..#.####..#..#..#.#.#.",
	"#.###.#.#.###..#.########",
	"#..##..#..##..#..####..##",
	"#.###.###..#.##########.#",
	"........##..##..#...##.#.",
	"#######..##..##.#.#.#.###",
	"#.....#.#...#.#.#...##.#.",
	"#.###.#.##..#############",
	"#.###.#.##...#.#.##.##.##",
	"#.###.#.########.#.#.#..#",
	"#.....#....#...##.#.#...#",
	"#######.#.###..#.....####"
};

static int test_image(struct quirc *q, int scale, int rotation, int borders)
{
	static const char payload[] = "quirc border regression";
	const int left = borders & 1;
	const int right = (borders >> 1) & 1;
	const int top = (borders >> 2) & 1;
	const int bottom = (borders >> 3) & 1;
	const int w = QR_SIZE * scale + left + right;
	const int h = QR_SIZE * scale + top + bottom;
	struct quirc_code code;
	struct quirc_data data;
	uint8_t *image;
	int x, y;

	if (quirc_resize(q, w, h) < 0)
		return -1;
	image = quirc_begin(q, NULL, NULL);
	memset(image, 255, w * h);

	for (y = 0; y < QR_SIZE * scale; y++) {
		for (x = 0; x < QR_SIZE * scale; x++) {
			int sx = x / scale;
			int sy = y / scale;
			int i;

			for (i = 0; i < rotation; i++) {
				int old_x = sx;

				sx = sy;
				sy = QR_SIZE - 1 - old_x;
			}
			if (qr[sy][sx] == '#')
				image[(y + top) * w + x + left] = 0;
		}
	}

	quirc_end(q);
	if (quirc_count(q) != 1) {
		fprintf(stderr, "Expected one QR code, found %d\n",
			quirc_count(q));
		return -1;
	}
	quirc_extract(q, 0, &code);
	if (quirc_decode(&code, &data) != QUIRC_SUCCESS ||
	    data.payload_len != (int)sizeof(payload) - 1 ||
	    memcmp(data.payload, payload, sizeof(payload))) {
		fprintf(stderr, "Decoded payload does not match\n");
		return -1;
	}

	return 0;
}

int main(void)
{
	static const int scales[] = {1, 2, 4};
	struct quirc *q = quirc_new();
	unsigned int i;
	int rotation, borders;

	if (!q)
		return 1;
	for (i = 0; i < sizeof(scales) / sizeof(scales[0]); i++) {
		for (rotation = 0; rotation < 4; rotation++) {
			for (borders = 0; borders < 16; borders++) {
				if (test_image(q, scales[i], rotation,
					       borders)) {
					fprintf(stderr, "scale=%d rotation=%d "
						"borders=%d\n", scales[i],
						rotation, borders);
					quirc_destroy(q);
					return 1;
				}
			}
		}
	}
	quirc_destroy(q);
	puts("Passed 192 QR image-edge cases");
	return 0;
}
