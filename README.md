# MarkDownC (MDC)

This simple C++ program generates HTML documents from markdown. 

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

### Building
```
mkdir build
cd build
cmake ..
make
```