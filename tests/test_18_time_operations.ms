// Test 18: Time Operations
print("=== Test 18: Time Operations ===");

// Test 1: Get current time
print("Test 1: Getting current time");
now = time_now();
print("Current timestamp:", now);

// Test 2: Format current time
print("Test 2: Formatting current time");
formatted_date = time_format(now, "%Y-%m-%d");
formatted_datetime = time_format(now, "%Y-%m-%d %H:%M:%S");
formatted_time = time_format(now, "%H:%M:%S");

print("Date:", formatted_date);
print("DateTime:", formatted_datetime);
print("Time:", formatted_time);

// Test 3: Extract time components
print("Test 3: Extracting time components");
year = time_year(now);
month = time_month(now);
day = time_day(now);
hour = time_hour(now);
minute = time_minute(now);
second = time_second(now);
weekday = time_weekday(now);

print("Year:", year);
print("Month:", month);
print("Day:", day);
print("Hour:", hour);
print("Minute:", minute);
print("Second:", second);
print("Weekday:", weekday, "(0=Sunday, 1=Monday, etc.)");

// Test 4: Time parsing (basic format)
print("Test 4: Time parsing");
test_date = "2025-12-25";
parsed_timestamp = time_parse(test_date, "%Y-%m-%d");
if (parsed_timestamp > 0) {
    print("Parsed date:", test_date);
    print("Timestamp:", parsed_timestamp);
    formatted_back = time_format(parsed_timestamp, "%Y-%m-%d");
    print("Formatted back:", formatted_back);
} else {
    print("Failed to parse date:", test_date);
}

// Test 5: Time calculations
print("Test 5: Time calculations");
one_hour = 3600;
one_day = 86400;

future_hour = time_add(now, one_hour);
future_day = time_add(now, one_day);

print("Current time:", time_format(now, "%H:%M:%S"));
print("One hour later:", time_format(future_hour, "%H:%M:%S"));
print("Tomorrow:", time_format(future_day, "%Y-%m-%d %H:%M:%S"));

// Test 6: Time differences
print("Test 6: Time differences");
diff_hour = time_diff(future_hour, now);
diff_day = time_diff(future_day, now);

print("Difference (1 hour):", diff_hour, "seconds");
print("Difference (1 day):", diff_day, "seconds");

// Test 7: Specific date operations
print("Test 7: Working with specific dates");
new_year_2026 = time_parse("2026-01-01", "%Y-%m-%d");
if (new_year_2026 > 0) {
    ny_year = time_year(new_year_2026);
    ny_month = time_month(new_year_2026);
    ny_day = time_day(new_year_2026);
    ny_weekday = time_weekday(new_year_2026);
    
    print("New Year 2026:");
    print("  Date:", ny_year, "/", ny_month, "/", ny_day);
    print("  Weekday:", ny_weekday);
    
    days_until = time_diff(new_year_2026, now);
    print("  Days until:", days_until / 86400);
}

// Test 8: Time formatting variations
print("Test 8: Different time formats");
sample_time = now;
formats = [
    "%Y-%m-%d",
    "%Y-%m-%d %H:%M:%S", 
    "%H:%M:%S",
    "%m/%d/%Y"
];

// Since we can't iterate over lists easily, test a few manually
format1 = time_format(sample_time, "%Y-%m-%d");
format2 = time_format(sample_time, "%Y-%m-%d %H:%M:%S");
format3 = time_format(sample_time, "%H:%M:%S");
format4 = time_format(sample_time, "%m/%d/%Y");

print("Format 1 (%Y-%m-%d):", format1);
print("Format 2 (%Y-%m-%d %H:%M:%S):", format2);
print("Format 3 (%H:%M:%S):", format3);
print("Format 4 (%m/%d/%Y):", format4);

// Test 9: Edge cases and validation
print("Test 9: Edge cases");
zero_time = 0;
if (zero_time == 0) {
    zero_formatted = time_format(zero_time, "%Y-%m-%d %H:%M:%S");
    print("Unix epoch (0):", zero_formatted);
}

// Test 10: Sleep function (short duration for testing)
print("Test 10: Sleep function");
print("Sleeping for 1 second...");
before_sleep = time_now();
sleep(1);
after_sleep = time_now();
sleep_duration = time_diff(after_sleep, before_sleep);
print("Sleep completed. Duration:", sleep_duration, "seconds");

print("Test 18: PASSED");
print("");
