#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <regex>

// Helper function to process inline Markdown (links, bold, italic, inline footnotes)
std::string process_inline_markdown(std::string text) {
    // --- Inline Footnote Reference Conversion ---
    // Note: This helper might be called on content that doesn't have footnotes.
    // If inline footnotes are primarily in main content, this step can be removed from here
    // and kept only in the main processing loop where 'htmlContent' is processed.
    // For now, keeping it here for robustness if footnotes could reference other footnotes.
    std::regex inline_footnote_regex(R"(\[\^(\d+)\])");
    std::string tempText = "";
    auto words_begin = std::sregex_iterator(text.begin(), text.end(), inline_footnote_regex);
    auto words_end = std::sregex_iterator();

    size_t last_pos = 0;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string footnote_num = match[1].str();
        tempText += text.substr(last_pos, match.position() - last_pos);
        tempText += "<sup><a href=\"#fn" + footnote_num + "\" id=\"fnref" + footnote_num + "\">" + footnote_num + "</a></sup>";
        last_pos = match.position() + match.length();
    }
    tempText += text.substr(last_pos);
    text = tempText;


    // --- Link Conversion ---
    std::regex link_regex(R"(\[(.+?)\]\((.+?)\))");

    tempText = ""; // Reuse tempText for new replacements
    words_begin = std::sregex_iterator(text.begin(), text.end(), link_regex);
    words_end = std::sregex_iterator();
    last_pos = 0;

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string link_text = match[1].str();
        std::string link_url = match[2].str();

        tempText += text.substr(last_pos, match.position() - last_pos);
        tempText += "<a href=\"" + link_url + "\">" + link_text + "</a>";
        last_pos = match.position() + match.length();
    }
    tempText += text.substr(last_pos);
    text = tempText;

    // --- Bold and Italic Conversion ---
    size_t pos = 0;
    while ((pos = text.find("**", pos)) != std::string::npos) {
        size_t end_pos = text.find("**", pos + 2);
        if (end_pos != std::string::npos) {
            text.replace(end_pos, 2, "</strong>");
            text.replace(pos, 2, "<strong>");
            pos = end_pos + 9;
        } else {
            break;
        }
    }

    pos = 0;
    while ((pos = text.find("*", pos)) != std::string::npos) {
        size_t end_pos = text.find("*", pos + 1);
        if (end_pos != std::string::npos) {
            if (end_pos != pos + 1 && (pos == 0 || text[pos - 1] != '*') && (end_pos + 1 == text.length() || text[end_pos + 1] != '*')) {
                text.replace(end_pos, 1, "</em>");
                text.replace(pos, 1, "<em>");
                pos = end_pos + 5;
            } else {
                pos++;
            }
        } else {
            break;
        }
    }

    return text;
}


// Function to convert the entire Markdown content to HTML
std::string convert_markdown_to_html(const std::string& markdownContent) {
    // --- Footnote Extraction and Storage ---
    std::map<std::string, std::string> footnotes;
    std::string contentWithoutFootnoteDefs; // Renamed for clarity: this holds content without *definitions*

    std::regex footnote_def_regex(R"(^\[\^(\d+)\]:\s*(.*)$)");

    std::istringstream initialIss(markdownContent);
    std::string line;

    while (std::getline(initialIss, line)) {
        std::smatch matches;
        if (std::regex_match(line, matches, footnote_def_regex)) {
            // Store raw footnote content for now
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
        if (line.rfind("# ", 0) == 0 || line.rfind("## ", 0) == 0 || line.rfind("### ", 0) == 0) {
            if (inUnorderedList) {
                processedContent += "</ul>\n";
                inUnorderedList = false;
            }
            if (inOrderedList) {
                processedContent += "</ol>\n";
                inOrderedList = false;
            }
            if (line.rfind("# ", 0) == 0) processedContent += "<h1>" + line.substr(2) + "</h1>\n";
            else if (line.rfind("## ", 0) == 0) processedContent += "<h2>" + line.substr(3) + "</h2>\n";
            else if (line.rfind("### ", 0) == 0) processedContent += "<h3>" + line.substr(4) + "</h3>\n";
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
        else if (line.rfind("* ", 0) == 0 || line.rfind("- ", 0) == 0 || line.rfind("+ ", 0) == 0) {
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

    std::string htmlContent = processedContent; // Now htmlContent holds processed lines without raw footnotes

    // --- Apply Inline Markdown to the main content ---
    htmlContent = process_inline_markdown(htmlContent);

    // --- Paragraph wrapping ---
    std::istringstream paragraphIss(htmlContent);
    std::string finalHtmlMainContent = "";
    std::string currentLine;
    while (std::getline(paragraphIss, currentLine)) {
        if (!currentLine.empty()) {
            if (currentLine.rfind("<h1>", 0) != 0 &&
                currentLine.rfind("<h2>", 0) != 0 &&
                currentLine.rfind("<h3>", 0) != 0 &&
                currentLine.rfind("<ul>", 0) != 0 &&
                currentLine.rfind("<li>", 0) != 0 &&
                currentLine.rfind("<ol>", 0) != 0) {
                finalHtmlMainContent += "<p>" + currentLine + "</p>\n";
            } else {
                finalHtmlMainContent += currentLine + "\n";
            }
        } else {
            finalHtmlMainContent += "\n";
        }
    }

    // --- Append Footnotes Section ---
    if (!footnotes.empty()) {
        finalHtmlMainContent += "<div class=\"footnotes\">\n";
        finalHtmlMainContent += "<hr>\n";
        finalHtmlMainContent += "<ol>\n";
        for (const auto& pair : footnotes) {
            std::string fn_num = pair.first;
            std::string fn_content = pair.second;
            // IMPORTANT: Process the footnote content for inline markdown here!
            std::string processed_fn_content = process_inline_markdown(fn_content);
            finalHtmlMainContent += "<li id=\"fn" + fn_num + "\">" + processed_fn_content + " <a href=\"#fnref" + fn_num + "\" class=\"footnote-backref\">&#8617;</a></li>\n";
        }
        finalHtmlMainContent += "</ol>\n";
        finalHtmlMainContent += "</div>\n";
    }

    return finalHtmlMainContent;
}