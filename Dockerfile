FROM alpine:latest

RUN apk add --no-cache \
    build-base \
    pkgconfig \
    sdl12-compat-dev \
    libjpeg-turbo-dev \
    libpng-dev

WORKDIR /quirc
