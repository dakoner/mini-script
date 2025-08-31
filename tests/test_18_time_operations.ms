// Test 18: Time Operations
print("=== Test 18: Time Operations ===");

// Note: These tests depend on time functions which may not be fully implemented.
// This script verifies the syntax and expected behavior based on a fixed time.

// Test 1: Use a fixed time for determinism
var fixed_time_str = "2025-08-30 12:30:45";
var now = time_parse(fixed_time_str, "%Y-%m-%d %H:%M:%S");
assert now > 0, "time_parse should return a valid timestamp";

// Test 2: Format current time
assert time_format(now, "%Y-%m-%d") == "2025-08-30", "Date formatting check";
assert time_format(now, "%H:%M:%S") == "12:30:45", "Time formatting check";
assert time_format(now, "%Y-%m-%d %H:%M:%S") == fixed_time_str, "Datetime formatting check";

// Test 3: Extract time components
assert time_year(now) == 2025, "Year extraction check";
assert time_month(now) == 8, "Month extraction check";
assert time_day(now) == 30, "Day extraction check";
assert time_hour(now) == 12, "Hour extraction check";
assert time_minute(now) == 30, "Minute extraction check";
assert time_second(now) == 45, "Second extraction check";
// Assuming 2025-08-30 is a Saturday (weekday 6)
assert time_weekday(now) == 6, "Weekday extraction check";

// Test 4: Time parsing (basic format)
var test_date = "2026-12-25";
var parsed_timestamp = time_parse(test_date, "%Y-%m-%d");
assert parsed_timestamp > 0, "Parsing a different date should be valid";
assert time_format(parsed_timestamp, "%Y-%m-%d") == test_date, "Round-trip parse/format check";

// Test 5: Time calculations
var one_hour = 3600;
var one_day = 86400;

var future_hour = time_add(now, one_hour);
var future_day = time_add(now, one_day);

assert time_format(future_hour, "%H:%M:%S") == "13:30:45", "One hour later check";
assert time_format(future_day, "%Y-%m-%d") == "2025-08-31", "One day later check";

// Test 6: Time differences
assert time_diff(future_hour, now) == 3600, "One hour difference check";
assert time_diff(future_day, now) == 86400, "One day difference check";

// Test 7: Invalid parsing
var invalid_time = time_parse("not a date", "%Y-%m-%d");
assert invalid_time == null, "Parsing an invalid date should return null";

print("Test 18: PASSED");
