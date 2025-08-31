// Time Library for Mini Script Language - Namespace Version
// Provides date and time operations

// Define wrapper functions that will be accessible as time.now(), time.format(), etc.

function now() {
    return time_now();
}

function format(timestamp, fmt) {
    return time_format(timestamp, fmt);
}

function parse(date_string, fmt) {
    return time_parse(date_string, fmt);
}

function year(timestamp) {
    return time_year(timestamp);
}

function month(timestamp) {
    return time_month(timestamp);
}

function day(timestamp) {
    return time_day(timestamp);
}

function hour(timestamp) {
    return time_hour(timestamp);
}

function minute(timestamp) {
    return time_minute(timestamp);
}

function second(timestamp) {
    return time_second(timestamp);
}

function weekday(timestamp) {
    return time_weekday(timestamp);
}

function add(timestamp, seconds) {
    return time_add(timestamp, seconds);
}

function diff(timestamp1, timestamp2) {
    return time_diff(timestamp1, timestamp2);
}

// Module metadata
version = "1.1";
name = "Time Library";

print("Time library v2.0 loaded with namespace support!");
