// Time Library for Mini Script Language
// Provides date and time operations

print("=== Time Library ===");
print("This library provides date and time operations:");
print("");

print("Available functions:");
print("- time_now() - Get current timestamp as Unix epoch seconds");
print("- time_format(timestamp, format) - Format timestamp as string");
print("- time_parse(date_string, format) - Parse date string to timestamp");
print("- time_year(timestamp) - Extract year from timestamp");
print("- time_month(timestamp) - Extract month from timestamp (1-12)");
print("- time_day(timestamp) - Extract day from timestamp (1-31)");
print("- time_hour(timestamp) - Extract hour from timestamp (0-23)");
print("- time_minute(timestamp) - Extract minute from timestamp (0-59)");
print("- time_second(timestamp) - Extract second from timestamp (0-59)");
print("- time_weekday(timestamp) - Extract weekday (0=Sunday, 1=Monday, etc.)");
print("- time_add(timestamp, seconds) - Add seconds to timestamp");
print("- time_diff(timestamp1, timestamp2) - Calculate difference in seconds");
print("- sleep(seconds) - Pause execution for specified seconds");
print("");

print("Common format strings:");
print("- '%Y-%m-%d' - Date format: 2025-08-29");
print("- '%Y-%m-%d %H:%M:%S' - DateTime format: 2025-08-29 14:30:45");
print("- '%A, %B %d, %Y' - Full format: Thursday, August 29, 2025");
print("- '%H:%M:%S' - Time only: 14:30:45");
print("- '%m/%d/%Y' - US date format: 08/29/2025");
print("");

print("Usage examples:");
print("  now = time_now();");
print("  date_str = time_format(now, '%Y-%m-%d %H:%M:%S');");
print("  print('Current time:', date_str);");
print("");
print("  year = time_year(now);");
print("  month = time_month(now);");
print("  day = time_day(now);");
print("  print('Date:', year, '/', month, '/', day);");
print("");
print("  # Parse a date string");
print("  parsed = time_parse('2025-12-25 09:00:00', '%Y-%m-%d %H:%M:%S');");
print("  christmas = time_format(parsed, '%A, %B %d');");
print("");
print("  # Time calculations");
print("  future = time_add(now, 3600);  # Add 1 hour");
print("  diff = time_diff(future, now); # Calculate difference");
print("");

print("Time library loaded successfully!");
