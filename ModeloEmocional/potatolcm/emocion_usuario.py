"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class emocion_usuario(object):
    __slots__ = ["emocion"]

    def __init__(self):
        self.emocion = ""

    def encode(self):
        buf = BytesIO()
        buf.write(emocion_usuario._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        __emocion_encoded = self.emocion.encode('utf-8')
        buf.write(struct.pack('>I', len(__emocion_encoded)+1))
        buf.write(__emocion_encoded)
        buf.write(b"\0")

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != emocion_usuario._get_packed_fingerprint():
            raise ValueError("Decode error")
        return emocion_usuario._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = emocion_usuario()
        __emocion_len = struct.unpack('>I', buf.read(4))[0]
        self.emocion = buf.read(__emocion_len)[:-1].decode('utf-8', 'replace')
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if emocion_usuario in parents: return 0
        tmphash = (0x153e4f5187372ebc) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if emocion_usuario._packed_fingerprint is None:
            emocion_usuario._packed_fingerprint = struct.pack(">Q", emocion_usuario._get_hash_recursive([]))
        return emocion_usuario._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

