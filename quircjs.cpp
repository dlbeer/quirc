extern "C" {
	#include <emscripten.h>
	#include <quirc.h>

	struct quirc *qr;
	uint8_t *image;

	void setup() {
		qr = quirc_new();
		if (!qr) {
			perror("Failed to allocate memory");
		}
	}

	void teardown() {
		quirc_destroy(qr);
	}

	void resize(int width, int height) {
		 if (quirc_resize(qr, width, height) < 0) {
			perror("Failed to allocate video memory");
		}
	}

	void fill() {
		image = quirc_begin(qr, NULL, NULL);
	}

	void filled() {
		quirc_end(qr);
	}

	void process() {
		int num_codes;
		int i;

		num_codes = quirc_count(qr);
		for (i = 0; i < num_codes; i++) {
			struct quirc_code code;
			struct quirc_data data;
			quirc_decode_error_t err;

			quirc_extract(qr, i, &code);

			/* Decoding stage */
			err = quirc_decode(&code, &data);
			if (err)
				printf("DECODE FAILED: %s\n", quirc_strerror(err));
			else {
				printf("Data: %s\n", data.payload);

				EM_ASM_({
					decoded($0, $1)
				},
					i, data.payload
				);
			}
		}
	}


	int xsetup(int width, int height) {
		setup();
		resize(width, height);
		fill();
		EM_ASM_({
			setuped($0)
		},
			image
		);

		return (int)&image;
	}

	int xprocess() {
		filled();
		process();
		fill();
		return (int)&image;
	}
}