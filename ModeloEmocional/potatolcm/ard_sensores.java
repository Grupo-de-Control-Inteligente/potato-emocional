/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package potatolcm;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class ard_sensores implements lcm.lcm.LCMEncodable
{
    public int timestamp_sec;
    public int timestamp_nsec;
    public int luz_derecha;
    public int luz_izquierda;
    public int oreja_derecha;
    public int oreja_izquierda;
    public byte diadema;
    public byte antena;
 
    public ard_sensores()
    {
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0x188d1eca92de3c64L;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(potatolcm.ard_sensores.class))
            return 0L;
 
        classes.add(potatolcm.ard_sensores.class);
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
        outs.writeInt(this.timestamp_sec); 
 
        outs.writeInt(this.timestamp_nsec); 
 
        outs.writeInt(this.luz_derecha); 
 
        outs.writeInt(this.luz_izquierda); 
 
        outs.writeInt(this.oreja_derecha); 
 
        outs.writeInt(this.oreja_izquierda); 
 
        outs.writeByte(this.diadema); 
 
        outs.writeByte(this.antena); 
 
    }
 
    public ard_sensores(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public ard_sensores(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static potatolcm.ard_sensores _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        potatolcm.ard_sensores o = new potatolcm.ard_sensores();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.timestamp_sec = ins.readInt();
 
        this.timestamp_nsec = ins.readInt();
 
        this.luz_derecha = ins.readInt();
 
        this.luz_izquierda = ins.readInt();
 
        this.oreja_derecha = ins.readInt();
 
        this.oreja_izquierda = ins.readInt();
 
        this.diadema = ins.readByte();
 
        this.antena = ins.readByte();
 
    }
 
    public potatolcm.ard_sensores copy()
    {
        potatolcm.ard_sensores outobj = new potatolcm.ard_sensores();
        outobj.timestamp_sec = this.timestamp_sec;
 
        outobj.timestamp_nsec = this.timestamp_nsec;
 
        outobj.luz_derecha = this.luz_derecha;
 
        outobj.luz_izquierda = this.luz_izquierda;
 
        outobj.oreja_derecha = this.oreja_derecha;
 
        outobj.oreja_izquierda = this.oreja_izquierda;
 
        outobj.diadema = this.diadema;
 
        outobj.antena = this.antena;
 
        return outobj;
    }
 
}

