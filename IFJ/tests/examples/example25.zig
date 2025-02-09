const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = increment();
    ifj.write(a);
}

pub fn increment() i32{
    return decrement() + 2;
}

pub fn decrement() i32{
    return multiply() - 2;
}

pub fn multiply() i32{
    return constas() * 2;
}

pub fn constas() i32{
    return 3;
}
