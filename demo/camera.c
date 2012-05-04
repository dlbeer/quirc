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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev.h>

#include "camera.h"

static int set_video_format(struct camera *cam, uint32_t format,
			    int w, int h, camera_format_t my_fmt)
{
	struct v4l2_format fmt;

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = w;
	fmt.fmt.pix.height = h;
	fmt.fmt.pix.pixelformat = format;
	fmt.fmt.pix.field = V4L2_FIELD_ANY;

	if (ioctl(cam->fd, VIDIOC_S_FMT, &fmt) < 0)
		return -1;

	/* Note that the size obtained may not match what was requested. */
	cam->format = my_fmt;
	cam->width = fmt.fmt.pix.width;
	cam->height = fmt.fmt.pix.height;

	return 0;
}

int camera_set_gain(struct camera *cam, int value)
{
	struct v4l2_queryctrl query;
	struct v4l2_control ctrl;

	query.id = V4L2_CID_GAIN;
	if (ioctl(cam->fd, VIDIOC_QUERYCTRL, &query) < 0) {
		perror("ioctl: VIDIOC_QUERYCTRL");
		return -1;
	}

	ctrl.id = V4L2_CID_GAIN;

	if (value < 0)
		ctrl.value = query.default_value;
	else
		ctrl.value = value;

	if (ioctl(cam->fd, VIDIOC_S_CTRL, &ctrl) < 0) {
		perror("ioctl: VIDIOC_S_CTRL");
		return -1;
	}

	return 0;
}

int camera_init(struct camera *cam, const char *path, int width, int height)
{
	struct v4l2_streamparm fps;
	struct v4l2_requestbuffers rb;
	int type;
	int i;

	cam->fd = open(path, O_RDONLY);
	if (cam->fd < 0) {
		fprintf(stderr, "%s: can't open camera device: %s\n", path,
			strerror(errno));
		return -1;
	}

	/* Set the video format */
	if (set_video_format(cam, V4L2_PIX_FMT_MJPEG,
			     width, height, CAMERA_FORMAT_MJPEG) < 0 &&
	    set_video_format(cam, V4L2_PIX_FMT_YUYV,
			     width, height, CAMERA_FORMAT_YUYV) < 0) {
		fprintf(stderr, "%s: can't find a supported video format\n",
			path);
		goto fail;
	}

	/* Set the frame rate */
	memset(&fps, 0, sizeof(fps));
	fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fps.parm.capture.timeperframe.numerator = 1;
	fps.parm.capture.timeperframe.denominator = 20;

	if (ioctl(cam->fd, VIDIOC_S_PARM, &fps) < 0) {
		fprintf(stderr, "%s: can't set video frame rate: %s\n", path,
			strerror(errno));
		goto fail;
	}

	/* Request buffer */
	memset(&rb, 0, sizeof(rb));
	rb.count = CAMERA_BUFFERS;
	rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	rb.memory = V4L2_MEMORY_MMAP;

	if (ioctl(cam->fd, VIDIOC_REQBUFS, &rb) < 0) {
		fprintf(stderr, "%s: can't request buffers: %s\n", path,
			strerror(errno));
		goto fail;
	}

	/* Map buffers and submit them */
	for (i = 0; i < CAMERA_BUFFERS; i++) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(buf));
		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		if (ioctl(cam->fd, VIDIOC_QUERYBUF, &buf) < 0) {
			fprintf(stderr, "%s: can't query buffer: %s\n", path,
				strerror(errno));
			goto fail;
		}

		cam->buffer_maps[i] = mmap(0, buf.length,
			PROT_READ, MAP_SHARED, cam->fd, buf.m.offset);
		if (cam->buffer_maps[i] == MAP_FAILED) {
			fprintf(stderr, "%s: can't map buffer memory: %s\n",
				path, strerror(errno));
			goto fail;
		}

		cam->buffer_lens[i] = buf.length;
	}

	/* Switch on streaming */
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(cam->fd, VIDIOC_STREAMON, &type) < 0) {
		fprintf(stderr, "%s: can't enable streaming: %s\n", path,
			strerror(errno));
		goto fail;
	}

	/* Queue the buffers */
	for (i = 0; i < CAMERA_BUFFERS; i++) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(buf));
		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if (ioctl(cam->fd, VIDIOC_QBUF, &buf) < 0) {
			perror("can't start buffer queue");
			return -1;
		}
	}

	cam->mem_index = -1;
	return 0;

fail:
	close(cam->fd);
	return -1;
}

void camera_free(struct camera *cam)
{
	int i;

	for (i = 0; i < CAMERA_BUFFERS; i++)
		munmap(cam->buffer_maps[i], cam->buffer_lens[i]);
	close(cam->fd);
}

int camera_update(struct camera *cam)
{
	struct v4l2_buffer buf;
	int last_index = cam->mem_index;

	/* De-queue a buffer */
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	if (ioctl(cam->fd, VIDIOC_DQBUF, &buf) < 0) {
		perror("can't de-queue buffer");
		return -1;
	}

	cam->mem_index = buf.index;
	cam->mem = cam->buffer_maps[cam->mem_index];
	cam->mem_len = cam->buffer_lens[cam->mem_index];

	/* Re-queue the last buffer */
	if (last_index >= 0) {
		memset(&buf, 0, sizeof(buf));
		buf.index = last_index;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		if (ioctl(cam->fd, VIDIOC_QBUF, &buf) < 0) {
			perror("can't queue buffer");
			return -1;
		}
	}

	return 0;
}
