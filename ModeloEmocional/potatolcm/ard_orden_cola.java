/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package potatolcm;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class ard_orden_cola implements lcm.lcm.LCMEncodable
{
    public byte prioridad;
    public byte posicion;
    public byte velocidad;
 
    public ard_orden_cola()
    {
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0xa4f830cabe455be6L;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(potatolcm.ard_orden_cola.class))
            return 0L;
 
        classes.add(potatolcm.ard_orden_cola.class);
        long hash = LCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        outs.writeByte(this.prioridad); 
 
        outs.writeByte(this.posicion); 
 
        outs.writeByte(this.velocidad); 
 
    }
 
    public ard_orden_cola(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public ard_orden_cola(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static potatolcm.ard_orden_cola _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        potatolcm.ard_orden_cola o = new potatolcm.ard_orden_cola();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.prioridad = ins.readByte();
 
        this.posicion = ins.readByte();
 
        this.velocidad = ins.readByte();
 
    }
 
    public potatolcm.ard_orden_cola copy()
    {
        potatolcm.ard_orden_cola outobj = new potatolcm.ard_orden_cola();
        outobj.prioridad = this.prioridad;
 
        outobj.posicion = this.posicion;
 
        outobj.velocidad = this.velocidad;
 
        return outobj;
    }
 
}

