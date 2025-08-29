// Example Mini Script Program
// This demonstrates the basic features of the language

// Variable declarations and basic arithmetic
x = 10;
y = 20;
z = x + y;
print("Basic arithmetic:");
print("x =", x);
print("y =", y);
print("x + y =", z);
print();

// String operations
firstName = "John";
lastName = "Doe";
fullName = firstName + " " + lastName;
print("String operations:");
print("Full name:", fullName);
print("Length:", len(fullName));
print();

// Conditional statements
age = 25;
print("Conditional logic:");
if (age >= 18) {
    print("You are an adult");
} else {
    print("You are a minor");
}
print();

// While loop
print("While loop counting:");
counter = 1;
while (counter <= 5) {
    print("Count:", counter);
    counter = counter + 1;
}
print();

// For loop
print("For loop counting:");
for (i = 1; i <= 5; i = i + 1) {
    print("Index:", i);
}
print();

// List operations
print("List operations:");
numbers = [1, 2, 3, 4, 5];
print("Numbers:", numbers);
print("List length:", len(numbers));
print();

// Boolean operations
print("Boolean operations:");
a = true;
b = false;
print("a =", a);
print("b =", b);
print("a && b =", a && b);
print("a || b =", a || b);
print("!a =", !a);
print();

// More complex expressions
print("Complex expressions:");
result = (x + y) * 2 - z;
print("(x + y) * 2 - z =", result);

isPositive = result > 0;
print("Result is positive:", isPositive);
