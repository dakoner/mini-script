// Test 7: While Loops
print("=== Test 7: While Loops ===");

// Basic while loop
print("Counting up from 1 to 5:");
i = 1;
while (i <= 5) {
    print("Count:", i);
    i = i + 1;
}

// Countdown while loop
print("Countdown from 5 to 1:");
j = 5;
while (j >= 1) {
    print("T-minus", j);
    j = j - 1;
}
print("Liftoff!");

// While loop with condition
print("Finding first power of 2 greater than 100:");
power = 1;
exponent = 0;
while (power <= 100) {
    power = power * 2;
    exponent = exponent + 1;
}
print("2^" + exponent + " =", power);

// While loop with string concatenation
print("Building a string:");
str = "";
counter = 1;
while (counter <= 3) {
    str = str + "Step" + counter + " ";
    counter = counter + 1;
}
print("Result:", str);

// While loop with boolean condition
print("While loop with boolean:");
continue_loop = true;
loop_count = 0;
while (continue_loop) {
    loop_count = loop_count + 1;
    print("Loop iteration:", loop_count);
    if (loop_count >= 3) {
        continue_loop = false;
    }
}

// Nested while loops
print("Nested while loops (multiplication table):");
outer = 1;
while (outer <= 3) {
    inner = 1;
    while (inner <= 3) {
        result = outer * inner;
        print(outer + " x " + inner + " =", result);
        inner = inner + 1;
    }
    outer = outer + 1;
}

print("Test 7: PASSED");
print("");
