<div align="center">

# Ray Tracer

A lightweight, multithreaded ray tracer that renders built-in scenes or imports STL meshes from disk.

</div>

## Features

- Renders built-in demo scenes with spheres and triangles
- Loads STL files and renders them as triangle meshes
- Writes the final image to Render.ppm
- Supports multithreaded rendering for faster output

## Quickstart

If you do not want to build from source, you can use the binaries provided in the GitHub Releases section for your platform.

## Building from source

The project uses CMake.

```bash
cmake -B build
cmake --build build
```

This will produce the RayTracer executable in the build directory.

## Usage

Run the program with:

```bash
./build/RayTracer --help
```

### Built-in scenes

If you run the program without an STL file, it will prompt you to choose one of the built-in scenes.

### Rendering an STL file

To render a mesh from an STL file:

```bash
./build/RayTracer -p model.stl
```

### Common options

- -t, --threads: number of threads to use
- -w, --width: output image width
- -s, --samples_per_pixel: antialiasing samples per pixel
- -d, --depth: maximum ray bounce depth
- -p, --path: path to an STL file
- -o, --offset: camera distance offset for STL rendering
- -f, --fov: camera field of view

Example:

```bash
./build/RayTracer -p model.stl -w 1600 -s 64 -d 50 -t 8 -f 60
```

## STL export note

Important: exported STL files must use the coordinate convention Up = Y and Forward = -Z. This renderer assumes that orientation when reading triangle data, so meshes exported with a different axis convention may appear rotated or misplaced.

If you are exporting from Blender or another 3D tool, make sure the export settings match:

- Up: Y
- Forward: -Z

## Output

The rendered image is written to Render.ppm in the project root / working directory.

## Credits
- Most of the concepts in this code if from a book called Ray [Tracing in a Weekend](https://raytracing.github.io/)
- Also CLI parameters are parsed with a header library called [popl](https://github.com/badaix/popl)
