const ifj = @import("ifj24.zig");
pub fn decrement(n: i32, m: i32) i32 {
    const a : i32 = n+m+ifj;
    return (50+12)*a;
}

pub fn main() void{
    ifj.write("Enter first number: ");
    const a = ifj.readi32();
    if (a) |A|{
        ifj.write("Enter second number: ");
        const b = ifj.readi32();
        if(b) |B|{
            const result = decrement(A, B);
            ifj.write("Result: ");
            ifj.write(result);
            ifj.write("\n");
        }
        else {
            ifj.write("Wrong second number ");
        }
    }
    else{
        ifj.write("Wrong first number ");
    }
}

