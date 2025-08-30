print("Before defining function");
function debug_func(int a) {
    print("Inside function, a =", a);
    return a * 2;
}
print("After defining function");
print("Calling function...");
result = debug_func(5);
print("Result:", result);
