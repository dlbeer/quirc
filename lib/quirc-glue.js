
// Bindings utilities

function WrapperObject() {
}
WrapperObject.prototype = Object.create(WrapperObject.prototype);
WrapperObject.prototype.constructor = WrapperObject;
WrapperObject.prototype.__class__ = WrapperObject;
WrapperObject.__cache__ = {};
Module['WrapperObject'] = WrapperObject;

function getCache(__class__) {
  return (__class__ || WrapperObject).__cache__;
}
Module['getCache'] = getCache;

function wrapPointer(ptr, __class__) {
  var cache = getCache(__class__);
  var ret = cache[ptr];
  if (ret) return ret;
  ret = Object.create((__class__ || WrapperObject).prototype);
  ret.ptr = ptr;
  return cache[ptr] = ret;
}
Module['wrapPointer'] = wrapPointer;

function castObject(obj, __class__) {
  return wrapPointer(obj.ptr, __class__);
}
Module['castObject'] = castObject;

Module['NULL'] = wrapPointer(0);

function destroy(obj) {
  if (!obj['__destroy__']) throw 'Error: Cannot destroy object. (Did you create it yourself?)';
  obj['__destroy__']();
  // Remove from cache, so the object can be GC'd and refs added onto it released
  delete getCache(obj.__class__)[obj.ptr];
}
Module['destroy'] = destroy;

function compare(obj1, obj2) {
  return obj1.ptr === obj2.ptr;
}
Module['compare'] = compare;

function getPointer(obj) {
  return obj.ptr;
}
Module['getPointer'] = getPointer;

function getClass(obj) {
  return obj.__class__;
}
Module['getClass'] = getClass;

// Converts big (string or array) values into a C-style storage, in temporary space

var ensureCache = {
  buffer: 0,  // the main buffer of temporary storage
  size: 0,   // the size of buffer
  pos: 0,    // the next free offset in buffer
  temps: [], // extra allocations
  needed: 0, // the total size we need next time

  prepare: function() {
    if (ensureCache.needed) {
      // clear the temps
      for (var i = 0; i < ensureCache.temps.length; i++) {
        Module['_free'](ensureCache.temps[i]);
      }
      ensureCache.temps.length = 0;
      // prepare to allocate a bigger buffer
      Module['_free'](ensureCache.buffer);
      ensureCache.buffer = 0;
      ensureCache.size += ensureCache.needed;
      // clean up
      ensureCache.needed = 0;
    }
    if (!ensureCache.buffer) { // happens first time, or when we need to grow
      ensureCache.size += 128; // heuristic, avoid many small grow events
      ensureCache.buffer = Module['_malloc'](ensureCache.size);
      assert(ensureCache.buffer);
    }
    ensureCache.pos = 0;
  },
  alloc: function(array, view) {
    assert(ensureCache.buffer);
    var bytes = view.BYTES_PER_ELEMENT;
    var len = array.length * bytes;
    len = (len + 7) & -8; // keep things aligned to 8 byte boundaries
    var ret;
    if (ensureCache.pos + len >= ensureCache.size) {
      // we failed to allocate in the buffer, ensureCache time around :(
      assert(len > 0); // null terminator, at least
      ensureCache.needed += len;
      ret = Module['_malloc'](len);
      ensureCache.temps.push(ret);
    } else {
      // we can allocate in the buffer
      ret = ensureCache.buffer + ensureCache.pos;
      ensureCache.pos += len;
    }
    return ret;
  },
  copy: function(array, view, offset) {
    var offsetShifted = offset;
    var bytes = view.BYTES_PER_ELEMENT;
    switch (bytes) {
      case 2: offsetShifted >>= 1; break;
      case 4: offsetShifted >>= 2; break;
      case 8: offsetShifted >>= 3; break;
    }
    for (var i = 0; i < array.length; i++) {
      view[offsetShifted + i] = array[i];
    }
  },
};

function ensureString(value) {
  if (typeof value === 'string') {
    var intArray = intArrayFromString(value);
    var offset = ensureCache.alloc(intArray, HEAP8);
    ensureCache.copy(intArray, HEAP8, offset);
    return offset;
  }
  return value;
}
function ensureInt8(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP8);
    ensureCache.copy(value, HEAP8, offset);
    return offset;
  }
  return value;
}
function ensureInt16(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP16);
    ensureCache.copy(value, HEAP16, offset);
    return offset;
  }
  return value;
}
function ensureInt32(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP32);
    ensureCache.copy(value, HEAP32, offset);
    return offset;
  }
  return value;
}
function ensureFloat32(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAPF32);
    ensureCache.copy(value, HEAPF32, offset);
    return offset;
  }
  return value;
}
function ensureFloat64(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAPF64);
    ensureCache.copy(value, HEAPF64, offset);
    return offset;
  }
  return value;
}


// Code
/** @suppress {undefinedVars, duplicate} */function Code() { throw "cannot construct a Code, no constructor in IDL" }
Code.prototype = Object.create(WrapperObject.prototype);
Code.prototype.constructor = Code;
Code.prototype.__class__ = Code;
Code.__cache__ = {};
Module['Code'] = Code;

  Code.prototype['get_corners'] = Code.prototype.get_corners = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  return wrapPointer(_emscripten_bind_Code_get_corners_1(self, arg0), Point);
};
    Code.prototype['set_corners'] = Code.prototype.set_corners = /** @suppress {undefinedVars, duplicate} */function(arg0, arg1) {
  var self = this.ptr;
  ensureCache.prepare();
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  _emscripten_bind_Code_set_corners_2(self, arg0, arg1);
};
    Object.defineProperty(Code.prototype, 'corners', { get: Code.prototype.get_corners, set: Code.prototype.set_corners });
  Code.prototype['get_size'] = Code.prototype.get_size = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Code_get_size_0(self);
};
    Code.prototype['set_size'] = Code.prototype.set_size = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Code_set_size_1(self, arg0);
};
    Object.defineProperty(Code.prototype, 'size', { get: Code.prototype.get_size, set: Code.prototype.set_size });
  Code.prototype['get_cell_bitmap'] = Code.prototype.get_cell_bitmap = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  return _emscripten_bind_Code_get_cell_bitmap_1(self, arg0);
};
    Code.prototype['set_cell_bitmap'] = Code.prototype.set_cell_bitmap = /** @suppress {undefinedVars, duplicate} */function(arg0, arg1) {
  var self = this.ptr;
  ensureCache.prepare();
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  _emscripten_bind_Code_set_cell_bitmap_2(self, arg0, arg1);
};
    Object.defineProperty(Code.prototype, 'cell_bitmap', { get: Code.prototype.get_cell_bitmap, set: Code.prototype.set_cell_bitmap });
  Code.prototype['__destroy__'] = Code.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  _emscripten_bind_Code___destroy___0(self);
};
// Quirc
/** @suppress {undefinedVars, duplicate} */function Quirc() {
  this.ptr = _emscripten_bind_Quirc_Quirc_0();
  getCache(Quirc)[this.ptr] = this;
};;
Quirc.prototype = Object.create(WrapperObject.prototype);
Quirc.prototype.constructor = Quirc;
Quirc.prototype.__class__ = Quirc;
Quirc.__cache__ = {};
Module['Quirc'] = Quirc;

Quirc.prototype['getVersion'] = Quirc.prototype.getVersion = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return UTF8ToString(_emscripten_bind_Quirc_getVersion_0(self));
};;

Quirc.prototype['resize'] = Quirc.prototype.resize = /** @suppress {undefinedVars, duplicate} */function(w, h) {
  var self = this.ptr;
  if (w && typeof w === 'object') w = w.ptr;
  if (h && typeof h === 'object') h = h.ptr;
  return _emscripten_bind_Quirc_resize_2(self, w, h);
};;

Quirc.prototype['begin'] = Quirc.prototype.begin = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return wrapPointer(_emscripten_bind_Quirc_begin_0(self), VoidPtr);
};;

Quirc.prototype['end'] = Quirc.prototype.end = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  _emscripten_bind_Quirc_end_0(self);
};;

Quirc.prototype['strError'] = Quirc.prototype.strError = /** @suppress {undefinedVars, duplicate} */function(err) {
  var self = this.ptr;
  if (err && typeof err === 'object') err = err.ptr;
  return UTF8ToString(_emscripten_bind_Quirc_strError_1(self, err));
};;

Quirc.prototype['count'] = Quirc.prototype.count = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Quirc_count_0(self);
};;

Quirc.prototype['extract'] = Quirc.prototype.extract = /** @suppress {undefinedVars, duplicate} */function(index) {
  var self = this.ptr;
  if (index && typeof index === 'object') index = index.ptr;
  return wrapPointer(_emscripten_bind_Quirc_extract_1(self, index), Code);
};;

Quirc.prototype['decode'] = Quirc.prototype.decode = /** @suppress {undefinedVars, duplicate} */function(code, data) {
  var self = this.ptr;
  if (code && typeof code === 'object') code = code.ptr;
  if (data && typeof data === 'object') data = data.ptr;
  return _emscripten_bind_Quirc_decode_2(self, code, data);
};;

Quirc.prototype['getPixel'] = Quirc.prototype.getPixel = /** @suppress {undefinedVars, duplicate} */function(index) {
  var self = this.ptr;
  if (index && typeof index === 'object') index = index.ptr;
  return _emscripten_bind_Quirc_getPixel_1(self, index);
};;

  Quirc.prototype['__destroy__'] = Quirc.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  _emscripten_bind_Quirc___destroy___0(self);
};
// VoidPtr
/** @suppress {undefinedVars, duplicate} */function VoidPtr() { throw "cannot construct a VoidPtr, no constructor in IDL" }
VoidPtr.prototype = Object.create(WrapperObject.prototype);
VoidPtr.prototype.constructor = VoidPtr;
VoidPtr.prototype.__class__ = VoidPtr;
VoidPtr.__cache__ = {};
Module['VoidPtr'] = VoidPtr;

  VoidPtr.prototype['__destroy__'] = VoidPtr.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  _emscripten_bind_VoidPtr___destroy___0(self);
};
// Data
/** @suppress {undefinedVars, duplicate} */function Data() { throw "cannot construct a Data, no constructor in IDL" }
Data.prototype = Object.create(WrapperObject.prototype);
Data.prototype.constructor = Data;
Data.prototype.__class__ = Data;
Data.__cache__ = {};
Module['Data'] = Data;

  Data.prototype['get_version'] = Data.prototype.get_version = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Data_get_version_0(self);
};
    Data.prototype['set_version'] = Data.prototype.set_version = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Data_set_version_1(self, arg0);
};
    Object.defineProperty(Data.prototype, 'version', { get: Data.prototype.get_version, set: Data.prototype.set_version });
  Data.prototype['get_ecc_level'] = Data.prototype.get_ecc_level = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Data_get_ecc_level_0(self);
};
    Data.prototype['set_ecc_level'] = Data.prototype.set_ecc_level = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Data_set_ecc_level_1(self, arg0);
};
    Object.defineProperty(Data.prototype, 'ecc_level', { get: Data.prototype.get_ecc_level, set: Data.prototype.set_ecc_level });
  Data.prototype['get_mask'] = Data.prototype.get_mask = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Data_get_mask_0(self);
};
    Data.prototype['set_mask'] = Data.prototype.set_mask = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Data_set_mask_1(self, arg0);
};
    Object.defineProperty(Data.prototype, 'mask', { get: Data.prototype.get_mask, set: Data.prototype.set_mask });
  Data.prototype['get_data_type'] = Data.prototype.get_data_type = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Data_get_data_type_0(self);
};
    Data.prototype['set_data_type'] = Data.prototype.set_data_type = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Data_set_data_type_1(self, arg0);
};
    Object.defineProperty(Data.prototype, 'data_type', { get: Data.prototype.get_data_type, set: Data.prototype.set_data_type });
  Data.prototype['get_payload'] = Data.prototype.get_payload = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  return _emscripten_bind_Data_get_payload_1(self, arg0);
};
    Data.prototype['set_payload'] = Data.prototype.set_payload = /** @suppress {undefinedVars, duplicate} */function(arg0, arg1) {
  var self = this.ptr;
  ensureCache.prepare();
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  _emscripten_bind_Data_set_payload_2(self, arg0, arg1);
};
    Object.defineProperty(Data.prototype, 'payload', { get: Data.prototype.get_payload, set: Data.prototype.set_payload });
  Data.prototype['get_payload_len'] = Data.prototype.get_payload_len = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Data_get_payload_len_0(self);
};
    Data.prototype['set_payload_len'] = Data.prototype.set_payload_len = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Data_set_payload_len_1(self, arg0);
};
    Object.defineProperty(Data.prototype, 'payload_len', { get: Data.prototype.get_payload_len, set: Data.prototype.set_payload_len });
  Data.prototype['get_eci'] = Data.prototype.get_eci = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Data_get_eci_0(self);
};
    Data.prototype['set_eci'] = Data.prototype.set_eci = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Data_set_eci_1(self, arg0);
};
    Object.defineProperty(Data.prototype, 'eci', { get: Data.prototype.get_eci, set: Data.prototype.set_eci });
  Data.prototype['__destroy__'] = Data.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  _emscripten_bind_Data___destroy___0(self);
};
// Point
/** @suppress {undefinedVars, duplicate} */function Point() { throw "cannot construct a Point, no constructor in IDL" }
Point.prototype = Object.create(WrapperObject.prototype);
Point.prototype.constructor = Point;
Point.prototype.__class__ = Point;
Point.__cache__ = {};
Module['Point'] = Point;

  Point.prototype['get_x'] = Point.prototype.get_x = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Point_get_x_0(self);
};
    Point.prototype['set_x'] = Point.prototype.set_x = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Point_set_x_1(self, arg0);
};
    Object.defineProperty(Point.prototype, 'x', { get: Point.prototype.get_x, set: Point.prototype.set_x });
  Point.prototype['get_y'] = Point.prototype.get_y = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  return _emscripten_bind_Point_get_y_0(self);
};
    Point.prototype['set_y'] = Point.prototype.set_y = /** @suppress {undefinedVars, duplicate} */function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_Point_set_y_1(self, arg0);
};
    Object.defineProperty(Point.prototype, 'y', { get: Point.prototype.get_y, set: Point.prototype.set_y });
  Point.prototype['__destroy__'] = Point.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} */function() {
  var self = this.ptr;
  _emscripten_bind_Point___destroy___0(self);
};
(function() {
  function setupEnums() {
    

    // Quirc_DecodeError

    Module['Quirc']['SUCCESS'] = _emscripten_enum_Quirc_DecodeError_SUCCESS();

    Module['Quirc']['ERROR_INVALID_GRID_SIZE'] = _emscripten_enum_Quirc_DecodeError_ERROR_INVALID_GRID_SIZE();

    Module['Quirc']['ERROR_INVALID_VERSION'] = _emscripten_enum_Quirc_DecodeError_ERROR_INVALID_VERSION();

    Module['Quirc']['ERROR_FORMAT_ECC'] = _emscripten_enum_Quirc_DecodeError_ERROR_FORMAT_ECC();

    Module['Quirc']['ERROR_DATA_ECC'] = _emscripten_enum_Quirc_DecodeError_ERROR_DATA_ECC();

    Module['Quirc']['ERROR_UNKNOWN_DATA_TYPE'] = _emscripten_enum_Quirc_DecodeError_ERROR_UNKNOWN_DATA_TYPE();

    Module['Quirc']['ERROR_DATA_OVERFLOW'] = _emscripten_enum_Quirc_DecodeError_ERROR_DATA_OVERFLOW();

    Module['Quirc']['ERROR_DATA_UNDERFLOW'] = _emscripten_enum_Quirc_DecodeError_ERROR_DATA_UNDERFLOW();

  }
  if (runtimeInitialized) setupEnums();
  else addOnPreMain(setupEnums);
})();
