#pragma once
#include <functional> // For std::function
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// Define a struct to hold a regex and its replacement logic
struct InlineRule {
  std::regex pattern;
  // A lambda function that takes a std::smatch and returns the replacement
  // string
  std::function<std::string(const std::smatch &)> replacement_formatter;
};

// Helper function to process inline Markdown (images, links, bold, italic,
// inline footnotes)
std::string process_inline_markdown(std::string text) {
  // Define the order of inline processing.
  // Order matters: More specific patterns (like images, inline footnotes)
  // should generally come before more general ones (like links, bold, italic).
  std::vector<InlineRule> rules = {
      // Rule for Inline Footnote References: [^N] -> <sup><a href="#fnN"
      // id="fnrefN">N</a></sup>
      {std::regex(R"(\[\^(\d+)\])"),
       [](const std::smatch &match) {
         std::string num = match[1].str();
         return "<sup><a href=\"#fn" + num + "\" id=\"fnref" + num + "\">" +
                num + "</a></sup>";
       }},
      // Rule for Images: ![alt text](url) -> <img src="url" alt="alt text">
      {std::regex(R"(!\[(.*?)\]\((.+?)\))"),
       [](const std::smatch &match) {
         std::string alt = match[1].str();
         std::string url = match[2].str();
         return "<img src=\"" + url + "\" alt=\"" + alt + "\">";
       }},
      // Rule for Links: [link text](url) -> <a href="url">link text</a>
      {std::regex(R"(\[(.+?)\]\((.+?)\))"),
       [](const std::smatch &match) {
         std::string text = match[1].str();
         std::string url = match[2].str();
         return "<a href=\"" + url + "\">" + text + "</a>";
       }},
      // Rule for Bold: **text** -> <strong>text</strong>
      // Note: For bold/italic with regex, ensure non-greedy matching.
      // Also, manually doing find/replace like before can sometimes be simpler
      // for these.
      // The regex `\*\*([^\*]+?)\*\*` is problematic if `*` can be inside.
      // A more robust regex for **bold** would be `\*\*([^\*]+)\*\*` or more
      // complex to handle nesting/escapes.
      // For simple cases:
      {std::regex(
           R"(\*\*([^\*]+?)\*\*)"), // Matches **text** but not **text*more**
       [](const std::smatch &match) {
         return "<strong>" + match[1].str() + "</strong>";
       }},
      // Rule for Italic: *text* -> <em>text</em>
      {std::regex(R"(\*([^\*]+?)\*)"), // Matches *text* but not *text**more*
       [](const std::smatch &match) {
         return "<em>" + match[1].str() + "</em>";
       }}};

  // Apply each rule sequentially
  for (const auto &rule : rules) {
    std::string current_text = "";
    auto words_begin =
        std::sregex_iterator(text.begin(), text.end(), rule.pattern);
    auto words_end = std::sregex_iterator();

    size_t last_pos = 0;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
      std::smatch match = *i;
      current_text += text.substr(last_pos, match.position() - last_pos);
      current_text +=
          rule.replacement_formatter(match); // Use the formatter lambda
      last_pos = match.position() + match.length();
    }
    current_text += text.substr(last_pos); // Add remaining text
    text = current_text;                   // Update text for the next rule
  }

  return text;
}

// Rest of the convert_markdown_to_html function remains the same
std::string convert_markdown_to_html(const std::string &markdownContent) {
  // --- Footnote Extraction and Storage ---
  std::map<std::string, std::string> footnotes;
  std::string contentWithoutFootnoteDefs;

  std::regex footnote_def_regex(R"(^\[\^(\d+)\]:\s*(.*)$)");

  std::istringstream initialIss(markdownContent);
  std::string line;

  while (std::getline(initialIss, line)) {
    std::smatch matches;
    if (std::regex_match(line, matches, footnote_def_regex)) {
      footnotes[matches[1].str()] = matches[2].str();
    } else {
      contentWithoutFootnoteDefs += line + "\n";
    }
  }

  // --- Heading, List, and General Content Processing ---
  std::string processedContent;
  std::istringstream iss(contentWithoutFootnoteDefs);

  bool inUnorderedList = false;
  bool inOrderedList = false;

  std::regex ordered_list_item_regex(R"(^(\d+)\.\s(.+)$)");

  while (std::getline(iss, line)) {
    // --- Close any open lists if the current line is a block element ---
    if (line.rfind("# ", 0) == 0 || line.rfind("## ", 0) == 0 ||
        line.rfind("### ", 0) == 0) {
      if (inUnorderedList) {
        processedContent += "</ul>\n";
        inUnorderedList = false;
      }
      if (inOrderedList) {
        processedContent += "</ol>\n";
        inOrderedList = false;
      }
      if (line.rfind("# ", 0) == 0)
        processedContent += "<h1>" + line.substr(2) + "</h1>\n";
      else if (line.rfind("## ", 0) == 0)
        processedContent += "<h2>" + line.substr(3) + "</h2>\n";
      else if (line.rfind("### ", 0) == 0)
        processedContent += "<h3>" + line.substr(4) + "</h3>\n";
    }
    // --- Ordered List Conversion ---
    else if (std::regex_match(line, ordered_list_item_regex)) {
      std::smatch ol_matches;
      std::regex_match(line, ol_matches, ordered_list_item_regex);

      if (inUnorderedList) {
        processedContent += "</ul>\n";
        inUnorderedList = false;
      }
      if (!inOrderedList) {
        processedContent += "<ol>\n";
        inOrderedList = true;
      }
      processedContent += "<li>" + ol_matches[2].str() + "</li>\n";
    }
    // --- Unordered List Conversion ---
    else if (line.rfind("* ", 0) == 0 || line.rfind("- ", 0) == 0 ||
             line.rfind("+ ", 0) == 0) {
      if (inOrderedList) {
        processedContent += "</ol>\n";
        inOrderedList = false;
      }
      if (!inUnorderedList) {
        processedContent += "<ul>\n";
        inUnorderedList = true;
      }
      processedContent += "<li>" + line.substr(2) + "</li>\n";
    }
    // --- General Content ---
    else {
      if (inUnorderedList) {
        processedContent += "</ul>\n";
        inUnorderedList = false;
      }
      if (inOrderedList) {
        processedContent += "</ol>\n";
        inOrderedList = false;
      }
      processedContent += line + "\n";
    }
  }
  if (inUnorderedList) {
    processedContent += "</ul>\n";
  }
  if (inOrderedList) {
    processedContent += "</ol>\n";
  }

  std::string htmlContent = processedContent;

  // --- Apply Inline Markdown to the main content ---
  htmlContent = process_inline_markdown(htmlContent);

  // --- Paragraph wrapping ---
  std::istringstream paragraphIss(htmlContent);
  std::string finalHtmlMainContent = "";
  std::string currentLine;
  while (std::getline(paragraphIss, currentLine)) {
    if (currentLine.empty())
      continue;
    if (currentLine.rfind("<h1>", 0) != 0 &&
        currentLine.rfind("<h2>", 0) != 0 &&
        currentLine.rfind("<h3>", 0) != 0 &&
        currentLine.rfind("<ul>", 0) != 0 &&
        currentLine.rfind("</ul>", 0) != 0 &&
        currentLine.rfind("<li>", 0) != 0 &&
        currentLine.rfind("<ol>", 0) != 0) {
      finalHtmlMainContent += "<p>" + currentLine + "</p>\n";
    } else {
      finalHtmlMainContent += currentLine + "\n";
    }
  }

  // --- Append Footnotes Section ---
  if (!footnotes.empty()) {
    finalHtmlMainContent += "<div class=\"footnotes\">\n";
    finalHtmlMainContent += "<hr>\n";
    finalHtmlMainContent += "<ol>\n";
    for (const auto &pair : footnotes) {
      std::string fn_num = pair.first;
      std::string fn_content = pair.second;
      std::string processed_fn_content = process_inline_markdown(fn_content);
      finalHtmlMainContent +=
          "<li id=\"fn" + fn_num + "\">" + processed_fn_content +
          " <a href=\"#fnref" + fn_num +
          "\" class=\"footnote-backref\">&#8617;</a></li>\n";
    }
    finalHtmlMainContent += "</ol>\n";
    finalHtmlMainContent += "</div>\n";
  }

  return finalHtmlMainContent;
}
