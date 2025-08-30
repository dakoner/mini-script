// Time Library for Mini Script Language - Namespace Version
// Provides date and time operations

// Define wrapper functions that will be accessible as time.now(), time.format(), etc.

function now() {
    return time_now();
}

function format(int timestamp, string fmt) {
    return time_format(timestamp, fmt);
}

function parse(string date_string, string fmt) {
    return time_parse(date_string, fmt);
}

function year(int timestamp) {
    return time_year(timestamp);
}

function month(int timestamp) {
    return time_month(timestamp);
}

function day(int timestamp) {
    return time_day(timestamp);
}

function hour(int timestamp) {
    return time_hour(timestamp);
}

function minute(int timestamp) {
    return time_minute(timestamp);
}

function second(int timestamp) {
    return time_second(timestamp);
}

function weekday(int timestamp) {
    return time_weekday(timestamp);
}

function add(int timestamp, int seconds) {
    return time_add(timestamp, seconds);
}

function diff(int timestamp1, int timestamp2) {
    return time_diff(timestamp1, timestamp2);
}

// Module metadata
version = "2.0";
name = "TimeLibrary";

print("Time library v2.0 loaded with namespace support!");
