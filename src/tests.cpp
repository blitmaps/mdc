//
// Created by Bradley Pearce on 08/07/2025.
//
#include "markdown.h"
#include <functional> // For std::function
#include <iostream>
#include <iostream> // For std::cout
#include <map> // Potentially useful for internal parsing in your converter, but not directly used by tests
#include <string> // For std::string
#include <vector> // For std::vector

// Colour codes in Tui
inline void set_foreground_rgb(int r, int g, int b) {
  printf("\033[38;2;%d;%d;%dm", r, g, b);
}

// Global counters for test results
static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

// Structure to hold a test case
struct TestCase {
  std::string name;
  std::function<void()> func; // Using std::function to store the test function
};

// Vector to store all registered test cases
static std::vector<TestCase> g_test_cases;

// Function to register a test
void register_test(const std::string &name, std::function<void()> test_func) {
  g_test_cases.push_back({name, test_func});
}

// Simple assertion macro
#define ASSERT_EQ(actual, expected, message)                                   \
  do {                                                                         \
    g_tests_run++;                                                             \
    if ((actual) == (expected)) {                                              \
      g_tests_passed++;                                                        \
      set_foreground_rgb(0, 0, 255);                                           \
      std::cout << "[PASSED] " << (message) << std::endl;                      \
      set_foreground_rgb(255, 255, 255);                                       \
    } else {                                                                   \
      g_tests_failed++;                                                        \
      set_foreground_rgb(255, 0, 0);                                           \
      std::cout << "[FAILED] " << (message) << "\n";                           \
      set_foreground_rgb(255, 255, 255);                                       \
      std::cout << "  Expected: \"" << (expected) << "\"\n";                   \
      std::cout << "  Actual:   \"" << (actual) << "\"\n";                     \
    }                                                                          \
  } while (0)

// Function to run all registered tests
void run_all_tests() {
  std::cout << "--- Running Simple Tests ---\n";
  g_tests_run = 0;
  g_tests_passed = 0;
  g_tests_failed = 0;

  for (const auto &test_case : g_test_cases) {
    // We'll wrap each test in a try-catch block to handle potential crashes,
    // though for simple assertions, this might be overkill.
    // For more robust tests, you'd want proper exception handling.
    try {
      test_case.func();
    } catch (const std::exception &e) {
      g_tests_failed++;
      std::cout << "[FAILED] " << test_case.name << " (Exception: " << e.what()
                << ")\n";
    } catch (...) {
      g_tests_failed++;
      std::cout << "[FAILED] " << test_case.name << " (Unknown Exception)\n";
    }
  }

  set_foreground_rgb(0, 255, 255);
  std::cout << "\n--- Test Summary ---\n";
  std::cout << "Total Tests Run: " << g_tests_run << "\n";
  std::cout << "Tests Passed:    " << g_tests_passed << "\n";
  std::cout << "Tests Failed:    " << g_tests_failed << "\n";
  std::cout << "--------------------\n";
  set_foreground_rgb(255, 255, 255);
}

// --- Define your test functions here ---

// Existing Tests (from previous iteration)
void test_BasicParagraph() {
  std::string markdown_input = "This is a paragraph.";
  std::string expected_html = "<p>This is a paragraph.</p>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Basic Paragraph Test");
}

void test_Heading1() {
  std::string markdown_input = "# Heading 1";
  std::string expected_html = "<h1>Heading 1</h1>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Heading 1 Test");
}

void test_Heading2() {
  std::string markdown_input = "## Heading 2";
  std::string expected_html = "<h2>Heading 2</h2>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Heading 2 Test");
}

void test_BoldText() {
  std::string markdown_input = "This is **bold** text.";
  std::string expected_html = "<p>This is <strong>bold</strong> text.</p>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Bold Text Test");
}

void test_ItalicText() {
  std::string markdown_input = "This is *italic* text.";
  std::string expected_html = "<p>This is <em>italic</em> text.</p>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Italic Text Test");
}

void test_UnorderedList() {
  std::string markdown_input = "- Item 1\n- Item 2";
  std::string expected_html = "<ul>\n<li>Item 1</li>\n<li>Item 2</li>\n</ul>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Unordered List Test");
}

void test_MixedElements() {
  std::string markdown_input = "# Title\n\nThis is a **paragraph** with "
                               "*emphasis*.\n\n- List Item 1\n- List Item 2";
  std::string expected_html =
      "<h1>Title</h1>\n<p>This is a <strong>paragraph</strong> with "
      "<em>emphasis</em>.</p>\n<ul>\n<li>List Item 1</li>\n<li>List Item "
      "2</li>\n</ul>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Mixed Elements Test");
}

void test_EmptyInput() {
  std::string markdown_input = "";
  std::string expected_html =
      ""; // Adjust based on your function's behavior for empty input
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Empty Input Test");
}

void test_MultipleNewlines() {
  std::string markdown_input = "Paragraph 1\n\n\nParagraph 2";
  std::string expected_html = "<p>Paragraph 1</p>\n<p>Paragraph 2</p>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Multiple Newlines Test");
}

// --- NEW TESTS FOR LINKS AND FOOTNOTES ---

// Test case for inline link
void test_InlineLink() {
  std::string markdown_input = "Visit [Google](https://www.google.com).";
  std::string expected_html =
      "<p>Visit <a href=\"https://www.google.com\">Google</a>.</p>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Inline Link Test");
}

// Test case for footnote definition and reference
void test_Footnote() {
  std::string markdown_input =
      "Here is some text with a footnote[^1].\n\n[^1]: This is the content of "
      "the footnote.";
  // The expected HTML for footnotes is quite specific and includes IDs and
  // back-links. This assumes a common output format. Adjust if your converter
  // generates different HTML.
  std::string expected_html =
  "<p>Here is some text with a footnote<sup><a href=\"#fn1\" id=\"fnref1\">1</a></sup>.</p>"
  "\n<div class=\"footnotes\">"
  "\n<hr>"
  "\n<ol>"
  "\n<li id=\"fn1\">This is the content of the footnote. <a href=\"#fnref1\" class=\"footnote-backref\">&#8617;</a></li>"
  "\n</ol>"
  "\n</div>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Single Footnote Test");
}

// Test case for multiple footnotes
void test_MultipleFootnotes() {
  std::string markdown_input =
      "First footnote[^11]. Second footnote[^12].\n\n[^11]: Content for footnote "
      "'11'.\n[^12]: Content for footnote '12'.";
  std::string expected_html =
    "<p>First footnote<sup><a href=\"#fn11\" id=\"fnref11\">11</a></sup>. Second footnote<sup><a href=\"#fn12\" id=\"fnref12\">12</a></sup>.</p>"
    "\n<div class=\"footnotes\">"
    "\n<hr>"
    "\n<ol>"
    "\n<li id=\"fn11\">Content for footnote '11'. <a href=\"#fnref11\" class=\"footnote-backref\">&#8617;</a></li>"
    "\n<li id=\"fn12\">Content for footnote '12'. <a href=\"#fnref12\" class=\"footnote-backref\">&#8617;</a></li>"
    "\n</ol>"
    "\n</div>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Multiple Footnotes Test");
}

// Test case for link and footnote interaction
void test_LinkAndFootnoteInteraction() {
  std::string markdown_input =
      "Check out [my website](http://example.com)[^1].\n\n[^1]: This is "
      "a note about the website.";
  std::string expected_html =
    "<p>Check out <a href=\"http://example.com\">my website</a><sup><a href=\"#fn1\" id=\"fnref1\">1</a></sup>.</p>"
    "\n<div class=\"footnotes\">"
    "\n<hr>"
    "\n<ol>"
    "\n<li id=\"fn1\">This is a note about the website. <a href=\"#fnref1\" class=\"footnote-backref\">&#8617;</a></li>"
    "\n</ol>"
    "\n</div>\n";
  std::string actual_html = convert_markdown_to_html(markdown_input);
  ASSERT_EQ(actual_html, expected_html, "Link and Footnote Interaction Test");
}

// Function to register all tests. Call this from your main test runner.
void register_all_markdown_tests() {
  // Existing tests
  register_test("BasicParagraph", test_BasicParagraph);
  register_test("Heading1", test_Heading1);
  register_test("Heading2", test_Heading2);
  register_test("BoldText", test_BoldText);
  register_test("ItalicText", test_ItalicText);
  register_test("UnorderedList", test_UnorderedList);
  register_test("MixedElements", test_MixedElements);
  register_test("EmptyInput", test_EmptyInput);
  register_test("MultipleNewlines", test_MultipleNewlines);

  // New tests for links and footnotes
  register_test("InlineLink", test_InlineLink);
  register_test("Footnote", test_Footnote);
  register_test("MultipleFootnotes", test_MultipleFootnotes);
  register_test("LinkAndFootnoteInteraction", test_LinkAndFootnoteInteraction);
}

int main() {
  register_all_markdown_tests();
  run_all_tests();
}
