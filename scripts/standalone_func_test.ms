function double_it(int x) {
    return x * 2;
}

function add_numbers(int a, int b) {
    return a + b;
}

print("Testing functions...");
result1 = double_it(5);
print("double_it(5) =", result1);

result2 = add_numbers(10, 15);
print("add_numbers(10, 15) =", result2);

print("All tests passed!");
