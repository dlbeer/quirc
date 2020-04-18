
#include <emscripten.h>

extern "C" {

// Not using size_t for array indices as the values used by the javascript code are signed.
void array_bounds_check(const int array_size, const int array_idx) {
  if (array_idx < 0 || array_idx >= array_size) {
    EM_ASM({
      throw 'Array index ' + $0 + ' out of bounds: [0,' + $1 + ')';
    }, array_idx, array_size);
  }
}

// Code

Quirc::Point* EMSCRIPTEN_KEEPALIVE emscripten_bind_Code_get_corners_1(Quirc::Code* self, int arg0) {
  return (array_bounds_check(sizeof(self->corners) / sizeof(self->corners[0]), arg0), self->corners[arg0]);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Code_set_corners_2(Quirc::Code* self, int arg0, Quirc::Point* arg1) {
  (array_bounds_check(sizeof(self->corners) / sizeof(self->corners[0]), arg0), self->corners[arg0] = arg1);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Code_get_size_0(Quirc::Code* self) {
  return self->size;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Code_set_size_1(Quirc::Code* self, int arg0) {
  self->size = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Code___destroy___0(Quirc::Code* self) {
  delete self;
}

// Quirc

Quirc* EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_Quirc_0() {
  return new Quirc();
}

const char* EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_getVersion_0(Quirc* self) {
  return self->getVersion();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_resize_2(Quirc* self, int w, int h) {
  return self->resize(w, h);
}

void* EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_begin_0(Quirc* self) {
  return self->begin();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_end_0(Quirc* self) {
  self->end();
}

const char* EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_strError_1(Quirc* self, Quirc_DecodeError err) {
  return self->strError(err);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_count_0(Quirc* self) {
  return self->count();
}

Quirc::Code* EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_extract_1(Quirc* self, int index) {
  static Quirc::Code temp;
  return (temp = self->extract(index), &temp);
}

Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc_decode_2(Quirc* self, Quirc::Code* code, Quirc::Data* data) {
  return self->decode(code, data);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Quirc___destroy___0(Quirc* self) {
  delete self;
}

// VoidPtr

void EMSCRIPTEN_KEEPALIVE emscripten_bind_VoidPtr___destroy___0(void** self) {
  delete self;
}

// Data

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_get_version_0(Quirc::Data* self) {
  return self->version;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_set_version_1(Quirc::Data* self, int arg0) {
  self->version = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_get_ecc_level_0(Quirc::Data* self) {
  return self->ecc_level;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_set_ecc_level_1(Quirc::Data* self, int arg0) {
  self->ecc_level = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_get_mask_0(Quirc::Data* self) {
  return self->mask;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_set_mask_1(Quirc::Data* self, int arg0) {
  self->mask = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_get_data_type_0(Quirc::Data* self) {
  return self->data_type;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_set_data_type_1(Quirc::Data* self, int arg0) {
  self->data_type = arg0;
}

char* EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_get_payload_0(Quirc::Data* self) {
  return self->payload;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_set_payload_1(Quirc::Data* self, char* arg0) {
  self->payload = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_get_payload_len_0(Quirc::Data* self) {
  return self->payload_len;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_set_payload_len_1(Quirc::Data* self, int arg0) {
  self->payload_len = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_get_eci_0(Quirc::Data* self) {
  return self->eci;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Data_set_eci_1(Quirc::Data* self, int arg0) {
  self->eci = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Data___destroy___0(Quirc::Data* self) {
  delete self;
}

// Point

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Point_get_x_0(Quirc::Point* self) {
  return self->x;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Point_set_x_1(Quirc::Point* self, int arg0) {
  self->x = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Point_get_y_0(Quirc::Point* self) {
  return self->y;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Point_set_y_1(Quirc::Point* self, int arg0) {
  self->y = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Point___destroy___0(Quirc::Point* self) {
  delete self;
}

// Quirc_DecodeError
Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_enum_Quirc_DecodeError_SUCCESS() {
  return Quirc::SUCCESS;
}
Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_enum_Quirc_DecodeError_ERROR_INVALID_GRID_SIZE() {
  return Quirc::ERROR_INVALID_GRID_SIZE;
}
Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_enum_Quirc_DecodeError_ERROR_INVALID_VERSION() {
  return Quirc::ERROR_INVALID_VERSION;
}
Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_enum_Quirc_DecodeError_ERROR_FORMAT_ECC() {
  return Quirc::ERROR_FORMAT_ECC;
}
Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_enum_Quirc_DecodeError_ERROR_DATA_ECC() {
  return Quirc::ERROR_DATA_ECC;
}
Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_enum_Quirc_DecodeError_ERROR_UNKNOWN_DATA_TYPE() {
  return Quirc::ERROR_UNKNOWN_DATA_TYPE;
}
Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_enum_Quirc_DecodeError_ERROR_DATA_OVERFLOW() {
  return Quirc::ERROR_DATA_OVERFLOW;
}
Quirc_DecodeError EMSCRIPTEN_KEEPALIVE emscripten_enum_Quirc_DecodeError_ERROR_DATA_UNDERFLOW() {
  return Quirc::ERROR_DATA_UNDERFLOW;
}

}

