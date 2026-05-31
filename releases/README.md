# Releases

This directory tracks **release artifacts** and **versioned firmware packages** for the OceanLabz AI Explorer S3 Board.

## Latest Release

| Version | Date | Highlights |
|---------|------|------------|
| v0.1.0 | TBD | Initial examples, pin definitions, documentation |

## Release Process

The project follows [Semantic Versioning](https://semver.org/):

- **MAJOR** — Incompatible API / board-breaking changes
- **MINOR** — New features, examples, or peripherals (backward-compatible)
- **PATCH** — Bug fixes, documentation updates, minor improvements

### Steps

1. Update all version references across files
2. Update [`CHANGELOG.md`](../CHANGELOG.md) with the new version entry
3. Tag the commit with `vX.Y.Z` (signed tag preferred)
4. Push tag to GitHub
5. [Draft a new release](https://github.com/ocean-labz/AI-Explorer-S3-Board/releases/new) on GitHub
6. Attach any binary artifacts (factory firmware, test sketches) to the release
7. Update the GitHub Release description with release notes

## Artifacts

Release artifacts are stored in `releases/tools/`:

| Artifact | Description |
|----------|-------------|
| `flash_tool/` | Bundled ESP32 Flash Download Tool configuration |
| `factory_firmware/` | Pre-compiled test firmware binaries (`.bin`) |
| `bootloader/` | Custom bootloader binaries (if modified from stock) |

> **Note:** Large binary files (>50 MB) should be hosted on GitHub Releases rather than committed to the repo. Use this directory for smaller configuration files and scripts that accompany releases.
