//call of all ifj. functions
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.readstr();
    if(a) |A|{
        ifj.write(A);
        ifj.write("\n");
    }else{}

    const b = ifj.readi32();
    if(b) |B|{
        ifj.write(B);
        ifj.write("\n");
    }else{}

    const c = ifj.readf64();
    if(c) |C|{
        ifj.write(C);
        ifj.write("\n");
    }else {}

    const d = 32;
    const i2f = ifj.i2f(d);
    ifj.write(i2f);
    ifj.write("\n");

    const e = 32.0;
    const f2i = ifj.f2i(e);
    ifj.write(f2i);
    ifj.write("\n");

    const f = ifj.string("str");
    ifj.write(f);
    ifj.write("\n");

    const g = ifj.length(f);
    ifj.write(g);
    ifj.write("\n");

    const h = ifj.concat(f,f);
    ifj.write(h);
    ifj.write("\n");

    const i = ifj.substring(f,0,3);
    if(i) |I|{
        ifj.write(I);
        ifj.write("\n");
        
        const j = ifj.strcmp(I,f);
        ifj.write(j);
        ifj.write("\n");
        
        const k = ifj.ord(I,1);
        ifj.write(k);
        ifj.write("\n");

    } else {}

    const l = ifj.chr(65);
    ifj.write(l);
    ifj.write("\n");
}
