print("=== Mini Script Language - Final Test ===");
print("");

print("Testing all features of the language:");
print("");

print("1. Variable Assignment and Types:");
intVar = 42;
floatVar = 3.14159;
stringVar = "Hello, World!";
boolVar = true;
listVar = [1, 2, 3, "mixed", true];

print("  Integer:", intVar);
print("  Float:", floatVar);
print("  String:", stringVar);
print("  Boolean:", boolVar);
print("  List:", listVar);
print("  List length:", len(listVar));
print("");

print("2. Arithmetic Operations:");
a = 10;
b = 3;
print("  a =", a, ", b =", b);
print("  a + b =", a + b);
print("  a - b =", a - b);
print("  a * b =", a * b);
print("  a / b =", a / b);
print("");

print("3. String Operations:");
first = "Mini";
second = "Script";
combined = first + " " + second;
print("  Combined string:", combined);
print("  Length:", len(combined));
print("");

print("4. Boolean Operations:");
x = true;
y = false;
print("  x =", x, ", y =", y);
print("  x && y =", x && y);
print("  x || y =", x || y);
print("  !x =", !x);
print("");

print("5. Comparison Operations:");
num1 = 15;
num2 = 20;
print("  num1 =", num1, ", num2 =", num2);
print("  num1 == num2:", num1 == num2);
print("  num1 != num2:", num1 != num2);
print("  num1 < num2:", num1 < num2);
print("  num1 > num2:", num1 > num2);
print("  num1 <= num2:", num1 <= num2);
print("  num1 >= num2:", num1 >= num2);
print("");

print("6. Conditional Statements:");
score = 85;
print("  Score:", score);
if (score >= 90) {
    print("  Grade: A (Excellent!)");
} else if (score >= 80) {
    print("  Grade: B (Good work!)");
} else if (score >= 70) {
    print("  Grade: C (Satisfactory)");
} else if (score >= 60) {
    print("  Grade: D (Needs improvement)");
} else {
    print("  Grade: F (Failed)");
}
print("");

print("7. While Loop:");
print("  Countdown from 5:");
counter = 5;
while (counter > 0) {
    print("    T-minus", counter);
    counter = counter - 1;
}
print("    Blast off!");
print("");

print("8. For Loop:");
print("  Multiplication table for 7:");
for (i = 1; i <= 5; i = i + 1) {
    result = i * 7;
    print("    7 x", i, "=", result);
}
print("");

print("9. Complex Expressions:");
val1 = 5;
val2 = 10;
val3 = 2;
complex1 = val1 + val2 * val3;
complex2 = (val1 + val2) * val3;
print("  5 + 10 * 2 =", complex1);
print("  (5 + 10) * 2 =", complex2);
print("");

print("10. Nested Conditions and Loops:");
print("  Finding even numbers from 1 to 10:");
for (num = 1; num <= 10; num = num + 1) {
    remainder = num - (num / 2) * 2;
    if (remainder == 0) {
        print("    ", num, "is even");
    }
}
print("");

print("=== All tests completed successfully! ===");
