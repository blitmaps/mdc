# MarkDownC (MDC)

This simple header only C++ library that generates HTML documents from markdown.

Included is a simple CLI application MDC that will read a markdown text file and
attempt to parse and generate the html.

### It supports

- Headers ```#,##,###```
- Bullets ```(+,-,*, N.)```
- Footnotes ```[^N]```
- Links ```(Like this)[http://likethis.com]```
- Images ```![Im](/Path/to/im.png)```

### Usage

```bash
mdc -i input.md > output.html
```

### Building binary
```
mkdir build
cd build
cmake ..
make
```

### #include "markdown.h"
Using MDC in a project:
```c++
# include "markdown.h"
  
  int main() {  
      std::string markdown_as_string = "#Hi \n";
      auto out = convert_markdown_to_html(markdown_as_string);
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
      return 0;
  }

```