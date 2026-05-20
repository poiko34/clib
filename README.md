# clib

A lightweight C utility library I use across my projects. Includes safe memory allocation with diagnostics, a dynamic string type, and a hash-map based config parser.

## Modules

### `util.h` — Safe allocator & logging

Wrappers around `malloc`, `calloc`, `realloc` and `free` that print the file and line on failure and exit immediately. Also provides colored log macros.

```c
void *buf = s_malloc(1024);
void *buf2 = s_calloc(16, sizeof(int));
buf = s_realloc(buf, 2048);
s_free(buf); // sets pointer to NULL
```

Log macros:
```c
print_info("Server started on port %d", port);
print_warning("Buffer almost full");
print_error("Failed to open %s", filename); // includes file:line
```

### `String.h` — Dynamic string

A heap-allocated string with automatic growth. Depends on `util.h`.

```c
String s;
string_init(&s, "hello");
string_append(&s, ", world");
string_add_c(&s, '!');
printf("%s\n", s.data); // hello, world!
string_free(&s);
```

| Function | Description |
|---|---|
| `string_init(str, data)` | Initialize, `data` can be `NULL` |
| `string_free(str)` | Free memory |
| `string_append(str, suffix)` | Append C string |
| `string_add_c(str, ch)` | Append single char |
| `string_insert(str, index, data)` | Insert at position |
| `string_erase(str, index, count)` | Remove characters |
| `string_replace(str, index, ch)` | Replace character |
| `string_find(str, target)` | Returns index or -1 |
| `string_cmp(s1, s2)` | Compare two strings |
| `string_copy(dest, src)` | Copy string |
| `string_clear(str)` | Set length to 0 |
| `string_full_clear(str)` | Zero out memory (for secrets) |
| `string_reserve(str, cap)` | Pre-allocate capacity |
| `string_shrink_to_fit(str)` | Release unused memory |
| `string_at(str, index)` | Get char at index |

### `Config.h` — INI-style config parser

Key-value config file parser backed by a hash map (xxhash32, chaining). Depends on `String.h` and `util.h`.

Config file format:
```ini
# This is a comment
; This too

host = 127.0.0.1
port = 8080
name = my app
```

```c
Config cfg;
config_init(&cfg);
config_load(&cfg, "app.cfg");

const char *host = config_get(&cfg, "host");
config_set(&cfg, "port", "9090");

config_free(&cfg);
```

| Function | Description |
|---|---|
| `config_init(cfg)` | Initialize config |
| `config_free(cfg)` | Free all memory |
| `config_load(cfg, filename)` | Load from file, returns `errno` on failure |
| `config_get(cfg, key)` | Get value or `NULL` |
| `config_set(cfg, key, value)` | Set or update value |

## Usage

All modules are header-only. Copy the files you need into your project and include them:

```c
#include "util.h"
#include "String.h"
#include "Config.h"
```

> **Note:** Since the modules are header-only, include each header in only one `.c` file per translation unit, or wrap the implementation in a dedicated `lib.c` that includes all headers.

## Dependencies

No external dependencies. Standard C99 or later.
