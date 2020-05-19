Quirc
=====

QR codes are a type of high-density matrix barcodes, and quirc is a library for
extracting and decoding them from images. It has several features which make it
a good choice for this purpose:

* It is fast enough to be used with realtime video: extracting and decoding
  from VGA frame takes about 50 ms on a modern x86 core.

* It has a robust and tolerant recognition algorithm. It can correctly
  recognise and decode QR codes which are rotated and/or oblique to the camera.
  It can also distinguish and decode multiple codes within the same image.

* It is easy to use, with a simple API described in a single commented header
  file (see below for an overview).

* It is small and easily embeddable, with no dependencies other than standard C
  functions.

* It has a very small memory footprint: one byte per image pixel, plus a few kB
  per decoder object.

* It uses no global mutable state, and is safe to use in a multithreaded
  application.

* BSD-licensed, with almost no restrictions regarding use and/or modification.

The distribution comes with, in addition to the library, several test programs.
While the core library is very portable, these programs have some additional
dependencies. All of them require libjpeg, and two (`quirc-demo` and `inspect`)
require SDL. The camera demos use Linux-specific APIs:

### quirc-demo

This is an real-time demo which requires a camera and a graphical display. The
video stream is displayed on screen as it's received, and any QR codes
recognised are highlighted in the image, with the decoded information both
displayed on the image and printed on stdout.

### quirc-scanner

This program turns your camera into a barcode scanner. It's almost the same as
the `demo` application, but it doesn't display the video stream, and thus
doesn't require a graphical display.

### qrtest

This test is used to evaluate the performance of library. Given a directory
tree containing a bunch of JPEG images, it will attempt to locate and decode QR
codes in each image. Speed and success statistics are collected and printed on
stdout.

### inspect

This test is used for debugging. Given a single JPEG image, it will display a
diagram showing the internal state of the decoder as well as printing
additional information on stdout.

Installation
------------
To build the library and associated demos/tests, type `make`. If you need to
decode "large" image files build with `CFLAGS="-DQUIRC_MAX_REGIONS=65534" make`
instead. Note that this will increase the memory usage, it is discouraged for
low resource devices (i.e. embedded).

Type `make install` to install the library, header file and camera demos.

You can specify one or several of the following targets if you don't want, or
are unable to build everything:

* libquirc.a
* libquirc.so
* qrtest
* inspect
* quirc-scanner
* quirc-demo

Library use
-----------
All of the library's functionality is exposed through a single header file,
which you should include:

```C
#include <quirc.h>
```

To decode images, you'll need to instantiate a `struct quirc` object, which is
done with the `quirc_new` function. Later, when you no longer need to decode
anything, you should release the allocated memory with `quirc_destroy`:

```C
struct quirc *qr;

qr = quirc_new();
if (!qr) {
    perror("Failed to allocate memory");
    abort();
}

/* ... */

quirc_destroy(qr);
```

Having obtained a decoder object, you need to set the image size that you'll be
working with, which is done using `quirc_resize`:

```C
if (quirc_resize(qr, 640, 480) < 0) {
    perror("Failed to allocate video memory");
    abort();
}
```

`quirc_resize` and `quirc_new` are the only library functions which allocate
memory. If you plan to process a series of frames (or a video stream), you
probably want to allocate and size a single decoder and hold onto it to process
each frame.

Processing frames is done in two stages. The first stage is an
image-recognition stage called identification, which takes a grayscale image
and searches for QR codes. Using `quirc_begin` and `quirc_end`, you can feed a
grayscale image directly into the buffer that `quirc` uses for image
processing:

```C
uint8_t *image;
int w, h;

image = quirc_begin(qr, &w, &h);

/* Fill out the image buffer here.
 * image is a pointer to a w*h bytes.
 * One byte per pixel, w pixels per line, h lines in the buffer.
 */

quirc_end(qr);
```

Note that `quirc_begin` simply returns a pointer to a previously allocated
buffer. The buffer will contain uninitialized data. After the call to
`quirc_end`, the decoder holds a list of detected QR codes which can be queried
via `quirc_count` and `quirc_extract`.

At this point, the second stage of processing occurs -- decoding. This is done
via the call to `quirc_decode`, which is not associated with a decoder object.

```C
int num_codes;
int i;

/* We've previously fed an image to the decoder via
* quirc_begin/quirc_end.
*/

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
    else
        printf("Data: %s\n", data.payload);
}
```

`quirc_code` and `quirc_data` are flat structures which don't need to be
initialized or freed after use.

Copyright
---------
Copyright (C) 2010-2012 Daniel Beer <<dlbeer@gmail.com>>

Permission to use, copy, modify, and/or distribute this software for
any purpose with or without fee is hereby granted, provided that the
above copyright notice and this permission notice appear in all
copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
