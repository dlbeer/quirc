class QuircQR{
  constructor(){
    this.quirc = new Module.Quirc();
    this._data = [];
  }
  detect(imageData){
    this._data = [];
    this.quirc.resize(imageData.width, imageData.height);
    const buf = this.quirc.begin();
    Module.HEAPU8.set(imageData.data, buf.ptr);
    this.quirc.end();
    let count = this.quirc.count();
    for (let i = 0; i < count; i++){
      let c = this.quirc.extract(i); 
      let result = this.quirc.decode(c);
      let d = result.data;
      let payload = this._data2payload(d);
      let corners = this._code2corners(c);
      let p = {corners : corners ,error : result.error, version: d.version, ecc_level : d.ecc_level, mask : d.mask, data_type : d.data_type, payload : payload, eci : d.eci };
      this._data.push(p); 
    }
    return count;
  }
  count(){
    return this.quirc.count();
  }

  version(){
    return this.quirc.getVersion();
  }

  getData(num){
    if (num < 0 || num >= this.count()){
      return -1;
    }
    return this._data[num];
  }

  _code2corners(code){
    let c = [];
    for (let i = 0; i < 4; i++){
      let corner = {x : code.get_corners(i).x, y : code.get_corners(i).y} 
      c.push(corner);
    }
    return c;
  }
  _data2payload(data){
    let len = data.payload_len;
    let str = "";
    for (let i = 0; i < len; i++) {
      str += String.fromCharCode(data.get_payload(i));
    }
    return str;
  }
  
}  