print("=== Mini Script Language Demo ===");

print("1. Variables and Arithmetic");
num1 = 10;
num2 = 25;
result = num1 + num2 * 2;
print("10 + 25 * 2 =", result);

print("");
print("2. String Operations");
firstName = "Alice";
lastName = "Johnson";
fullName = firstName + " " + lastName;
print("Full name:", fullName);
print("Name length:", len(fullName));

print("");
print("3. Boolean Logic");
age = 25;
isAdult = age >= 18;
canVote = isAdult && age >= 21;
print("Age:", age);
print("Is adult:", isAdult);
print("Can vote:", canVote);

print("");
print("4. Conditional Statements");
score = 87;
if (score >= 90) {
    print("Grade: A");
} else if (score >= 80) {
    print("Grade: B");
} else if (score >= 70) {
    print("Grade: C");
} else {
    print("Grade: F");
}

print("");
print("5. Loops");
print("While loop - countdown:");
countdown = 5;
while (countdown > 0) {
    print("T-minus", countdown);
    countdown = countdown - 1;
}
print("Liftoff!");

print("");
print("For loop - multiplication table:");
for (i = 1; i <= 5; i = i + 1) {
    product = i * 7;
    print(i, "x 7 =", product);
}

print("");
print("6. Lists");
numbers = [10, 20, 30, 40, 50];
mixed = ["hello", 42, true, 3.14];

print("Numbers:", numbers);
print("Mixed types:", mixed);
print("Numbers length:", len(numbers));
print("Mixed length:", len(mixed));

print("");
print("7. Complex Expressions");
a = 5;
b = 10;
c = 15;
complex_result = (a + b) * c / (a - 2);
print("(5 + 10) * 15 / (5 - 2) =", complex_result);

print("");
print("Demo completed successfully!");
