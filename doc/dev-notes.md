# Dev Notes

## Makefile — adding a new series

Series binaries are driven by a single list variable. To add `seriesN`, change one line:

```makefile
SERIES = 1 2 3 4 5 6   # add N here
```

The generic link rule `$(BINDIR)/%: $(OBJDIR)/%.o` handles all series and other
single-source binaries automatically. The `console` binary is the only explicit
target because its source file (`main.cpp`) doesn't match its output name.

---

## Bella SDK — Vec3 scaling

`dl::math::VecT<3, double>` (a.k.a. `Vec3`) does **not** have a `.scaled()` method.
Use `operator*`:

```cpp
Vec3 result = v * 2.0;   // or  2.0 * v
```

---

## Blend texture UV alignment

When generating a per-vertex blend HDR for a `blendMaterial`, the texture dimensions
must match the **vertex** grid, not the face grid.

**Bug:** `blendW = nU - 1`, `blendH = nV - 1`

With texture width `nU-1` and mesh UV `ui/(nU-1)`, standard full-pixel sampling
maps vertex `ui` to texel `ui*(nU-2)/(nU-1)` — short by up to **1 full texel** at
the far edge. For a texture function with ~17 cycles (like `series5`'s `texture()`),
that 1-texel error equals ~40° of phase, easily enough to flip which material sits
on a ridge vs. a trough.

**Fix:** `blendW = nU`, `blendH = nV`. Loop `ui` in `[0, nU)` and `vi` in `[0, nV)`.
Then vertex `ui` with UV `ui/(nU-1)` maps to texel `ui*(nU-1)/(nU-1) = ui` exactly.

**Open question — V-flip:** Bella may follow the OpenGL UV convention where V=0 is
the bottom of the image. The HDR format stores rows top-to-bottom (`-Y`), so if
Bella flips V on load, the V axis of the blend texture would be inverted relative
to the mesh UVs. Symptom: correct correlation after the dimension fix above, but
material pattern mirrored in V. Fix if needed: invert uvV in `generateMeshData`
(`1.0 - vi/(nV-1)`) or reverse row order when writing the HDR.

---

## series144Mitsuba.cpp — Go-to-C++ port reference

`series144Mitsuba.cpp` is a straight C++ port of `~/dev/fm-animations/series144.go`.
It contains the math/geometry/texture functions from that file plus the full
`renderSurfaces` pipeline (binary PLY, RGBE env/blend maps, Mitsuba `sensor.xml`)
and a `main` with `--flag value` argument parsing matching Go's `flag` defaults.

Useful as a copy-paste source for math functions when working in the `fm-animations`
context. The `Mat4` used by `SLR2` is a stub — `invisible()` returns `false`
unconditionally (dead code in the original Go too), so the matrix inverse is never
called.

Standard math functions (`sin`, `cos`, `pow`, etc.) are brought in with
`using std::sin;` etc. so call sites look identical to the Go original.
