# INC Material Format (.imt) - Version 1.0

## Overview

The `.imt` (INC Material Type) is a human-readable, Source-inspired text format that defines how a material behaves in the INC Engine. It is parsed at runtime and supports shader selection, texture assignment, and rendering parameters.

---

## Header

```text
"INC_Material" "1.0"
```

The first line must define the format ID and version. This is used for future compatibility.

---

## Body Syntax

The body is a block using brace syntax (`{}`), containing quoted key-value pairs:

```imt
"shader"        "VertexLit"
"basetexture"   "models/props_c17/oildrum001"
"phong"         "1"
```

Keys and values must be quoted. Whitespace and indentation are ignored.

---

## Required Fields

| Key      | Type   | Description                               |
| -------- | ------ | ----------------------------------------- |
| `shader` | string | The shader type to use (e.g. `VertexLit`) |

---

## Optional Fields

| Key           | Type   | Description                                       |
| ------------- | ------ | ------------------------------------------------- |
| `basetexture` | string | Path to the base texture (relative to materials/) |
| `normalmap`   | string | Optional normal map texture path                  |
| `phong`       | int    | Enable phong shading (1/0)                        |
| `roughness`   | float  | Material surface roughness (0.0–1.0)              |
| `metalness`   | float  | Optional PBR metalness value                      |
| `envmap`      | string | Cubemap or environment map path                   |

---

## File Location

`.imt` files should be placed in the `materials/` directory relative to the mod:

```
sp/hl2/materials/models/props_c17/oildrum001.imt
```

---

## Example

```imt
"INC_Material" "1.0"
{
    "shader"        "VertexLit"
    "basetexture"   "models/props_c17/oildrum001"
    "phong"         "1"
    "roughness"     "0.2"
}
```

---

## Notes

- Keys beginning with `$` (like `$basetexture`) are allowed and stylistically compatible with Source, but INC recommends plain names.
- Future versions may support nested blocks or multi-pass shaders.

---

## Version

**1.0** — First stable release

Copyright (c) INC Engine

