//
// Created by brad on 04/07/25.
//
#include "libmain.h"
#include "markdown.h"
#include "read_lines.h"
#include <algorithm>
#include <complex>
#include <map>


void print_help() {
  std::cout << "mdc -i markdown.md > out.html" << std::endl;
}

void my_main(Arguments &args){

  // Show help if there are not enough arguments or help flag requested
  int min_args = 2;
  bool show_help = (args.help_flag || (args.arguments.size() < min_args));
  auto &s_args = args.arguments;

  std::string input_arg = "-i";

  std::map<std::string, int> indexes;

  // Here we iterate to check we have all the args
  std::string must_have_args[] = {input_arg};
  for (auto &m : must_have_args) {
    auto pos = std::find(s_args.begin(), s_args.end(), m);
    auto array_pos = pos - s_args.begin();
    // Check we are not at the end of the array
    show_help |= (((array_pos)+1) == min_args);
    // Check we are there at all
    show_help |= pos == s_args.end();
    // save pos
    indexes[m] = array_pos;
  }

  // Check and return
  if (show_help) {
    print_help();
    return;
  }

  // Main

  // Read in all the lines
  auto markdown = TextFileParser::read_lines(args.arguments[indexes[input_arg]+1]);
  if (markdown.status != TextFileParser::SUCCESSFUL) {
    std::cout << "Markdown file not read successfully." << std::endl;
    return;
  }

  std::string superstring = "";
  // Print out the file, this will do nothing if the file did not read as output will be empty.
  for (auto &line : *markdown.output) {
    superstring += line + '\n';
  }
  auto out = convert_markdown_to_html(superstring);
  // Write the basic HTML boilerplate to the output file
  std::cout << "<!DOCTYPE html>\n";
  std::cout << "<html>\n";
  std::cout << "<head>\n";
  std::cout << "    <title>Converted Markdown</title>\n";
  std::cout << "</head>\n";
  std::cout << "<body>\n";

  // Convert the entire Markdown content
  std::cout << out; // Write the converted content

  // Close the basic HTML boilerplate
  std::cout << "</body>\n";
  std::cout << "</html>\n";


}