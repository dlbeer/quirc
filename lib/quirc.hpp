
#ifndef _QUIRC_HPP
#define _QUIRC_HPP

#include <stdint.h>
#include "quirc.h"

class Quirc {
    private:
        struct quirc *instance;
    public:
        Quirc();
        ~Quirc();
        const char *getVersion();
        int resize(int w, int h);

        uint8_t *begin(int *w, int *h);
        void end();

        /* This enum describes the various decoder errors which may occur. */
        enum DecodeError {
            SUCCESS = 0,
            ERROR_INVALID_GRID_SIZE,
            ERROR_INVALID_VERSION,
            ERROR_FORMAT_ECC,
            ERROR_DATA_ECC,
            ERROR_UNKNOWN_DATA_TYPE,
            ERROR_DATA_OVERFLOW,
            ERROR_DATA_UNDERFLOW
        };

        struct Point {
	        int	x;
	        int	y;
        };

        struct Code {
            /* The four corners of the QR-code, from top left, clockwise */
            Point corners[4];

            /* The number of cells across in the QR-code. The cell bitmap
            * is a bitmask giving the actual values of cells. If the cell
            * at (x, y) is black, then the following bit is set:
            *
            *     cell_bitmap[i >> 3] & (1 << (i & 7))
            *
            * where i = (y * size) + x.
            */
            int			size;
            uint8_t		cell_bitmap[QUIRC_MAX_BITMAP];
        };

        struct Data {
            /* Various parameters of the QR-code. These can mostly be
            * ignored if you only care about the data.
            */
            int			version;
            int			ecc_level;
            int			mask;

            /* This field is the highest-valued data type found in the QR
            * code.
            */
            int			data_type;

            /* Data payload. For the Kanji datatype, payload is encoded as
            * Shift-JIS. For all other datatypes, payload is ASCII text.
            */
            uint8_t			payload[QUIRC_MAX_PAYLOAD];
            int			payload_len;

            /* ECI assignment number */
            uint32_t		eci;
        };


        /* Return a string error message for an error code. */
        const char *strError(Quirc::DecodeError err);

        int count();
        Quirc::Code extract(int index);
        Quirc::DecodeError decode(const Quirc::Code *code, struct Quirc::Data *data);

};

#endif