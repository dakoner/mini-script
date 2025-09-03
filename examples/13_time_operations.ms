// Time Operations Demo
// Demonstrates date and time capabilities of Mini Script Language

print("=== Time Operations Demonstration ===");
print("");

// Import the time library documentation
import "lib/time_library.ms";

print("");
print("=== Practical Time Operations Demo ===");

// Get current time and display it in multiple formats
print("Current Time Information:");
now = time_now();
print("Timestamp:", now);
print("Full date/time:", time_format(now, "%Y-%m-%d %H:%M:%S"));
print("Date only:", time_format(now, "%Y-%m-%d"));
print("Time only:", time_format(now, "%H:%M:%S"));
print("US format:", time_format(now, "%m/%d/%Y"));

// Extract and display time components
print("");
print("Time Components:");
year = time_year(now);
month = time_month(now);
day = time_day(now);
hour = time_hour(now);
minute = time_minute(now);
second = time_second(now);
weekday = time_weekday(now);

print("Year:", year);
print("Month:", month, "(1=January, 12=December)");
print("Day of month:", day);
print("Hour:", hour, "(24-hour format)");
print("Minute:", minute);
print("Second:", second);

weekday_names = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
print("Day of week:", weekday, "- This is a", time_format(now, "%A"));

// Calculate future and past dates
print("");
print("Time Calculations:");
one_minute = 60;
one_hour = 3600;
one_day = 86400;
one_week = 604800;

future_minute = time_add(now, one_minute);
future_hour = time_add(now, one_hour);
future_day = time_add(now, one_day);
future_week = time_add(now, one_week);

print("In 1 minute:", time_format(future_minute, "%H:%M:%S"));
print("In 1 hour:", time_format(future_hour, "%H:%M:%S"));
print("Tomorrow:", time_format(future_day, "%Y-%m-%d"));
print("Next week:", time_format(future_week, "%Y-%m-%d"));

// Work with specific dates
print("");
print("Working with Specific Dates:");

// New Year's Day
new_year = time_parse("2026-01-01", "%Y-%m-%d");
if (new_year > 0) {
    days_until_ny = time_diff(new_year, now) / one_day;
    print("New Year 2026:", time_format(new_year, "%A, %B %d, %Y"));
    print("Days until New Year:", days_until_ny);
}

// Christmas
christmas = time_parse("2025-12-25", "%Y-%m-%d");
if (christmas > 0) {
    days_until_xmas = time_diff(christmas, now) / one_day;
    print("Christmas 2025:", time_format(christmas, "%A, %B %d, %Y"));
    print("Days until Christmas:", days_until_xmas);
}

// Log file example with timestamps
print("");
print("Creating a timestamped log file:");
log_filename = "demo_log.txt";
log_handle = fopen(log_filename, "w");

timestamp_str = time_format(now, "%Y-%m-%d %H:%M:%S");
fwriteline(log_handle, "[" + timestamp_str + "] Application started");

// Simulate some operations with timestamps
sleep(1);
event1_time = time_now();
event1_str = time_format(event1_time, "%Y-%m-%d %H:%M:%S");
fwriteline(log_handle, "[" + event1_str + "] Processing data...");

sleep(1);
event2_time = time_now();
event2_str = time_format(event2_time, "%Y-%m-%d %H:%M:%S");
fwriteline(log_handle, "[" + event2_str + "] Operation completed successfully");

fclose(log_handle);

print("Log file created:", log_filename);
print("Log contents:");
log_read = fopen(log_filename, "r");
log_content = fread(log_read);
print(log_content);
fclose(log_read);

// Age calculator example
print("");
print("Age Calculator Example:");
birth_date = time_parse("1990-01-01", "%Y-%m-%d");
if (birth_date > 0) {
    age_seconds = time_diff(now, birth_date);
    age_days = age_seconds / one_day;
    age_years = age_days / 365;
    
    print("Birth date: January 1, 1990");
    print("Age in seconds:", age_seconds);
    print("Age in days:", age_days);
    print("Age in years (approx):", age_years);
}

print("");
print("Time operations demonstration completed successfully!");
print("Files created: demo_log.txt");
