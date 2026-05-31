# 📐 Documentation Standards

This document defines the standards for all documentation and examples in the OceanLabz AI Explorer S3 Board repository. Following these standards ensures consistency, readability, and maintainability.

---

## 1. Example README Files

Every example folder must contain a [`README.md`](examples/README.md) with the following structure:

```markdown
# Example Title (matches folder name, human-readable)

## Description
2-3 sentences explaining what the example does and what the user will learn.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- [Additional components, if any]

## Required Libraries
- LibraryName by Author (Library Manager installable)
- Version requirement if applicable

## Wiring
Pin mapping table referencing the board's fixed pinout.
For external components, include a wiring diagram reference.

## Steps
1. Step one
2. Step two
3. ...

## Expected Output
What the user should see in Serial Monitor, on the display, or hear.

## Troubleshooting
1-3 common issues specific to this example.

## References
Links to relevant docs, datasheets, or video tutorials.
```

### Example README Requirements
| Element | Required? | Notes |
|---------|:---------:|-------|
| Description | ✅ | Concise, beginner-friendly |
| Required Hardware | ✅ | List all components |
| Required Libraries | ✅ | Include exact names for Library Manager |
| Wiring | ✅ | Reference board pinout or external diagram |
| Steps | ✅ | Numbered list |
| Expected Output | ✅ | What success looks like |
| Troubleshooting | ✅ | At least 2 common issues |
| Wiring Diagram | ❌ | Optional but recommended for advanced examples |

---

## 2. Code Comments

### Arduino Sketch (.ino) Header
Every `.ino` file must start with this header:

```cpp
/**
 * @file    filename.ino
 * @author  OceanLabz
 * @date    2024-MM-DD
 * @brief   Short description of what this sketch does.
 *
 * @details Optional longer description (2-3 sentences max).
 *          What the user will learn or observe.
 */
```

### Inline Comments
| Type | Style | Example |
|------|-------|---------|
| Section | `// ===== SECTION =====` | `// ===== SETUP =====` |
| Explanation | `// Explain what this does` | `// Initialize I2C at 400kHz` |
| Warning | `// WARNING: ...` | `// WARNING: These pins are shared with TFT!` |
| TODO | `// TODO: ...` | `// TODO: Add error handling for SD failure` |

### Comment Frequency
- **Every `#define` or `const int` pin**: one-line comment explaining signal
- **Every block of 3+ lines**: a brief comment above
- **Complex logic**: inline or block comment explaining *why*, not *what*

---

## 3. Folder Naming

### Repository Top-Level
| Folder | Case | Example |
|--------|------|---------|
| `docs/` | lowercase | `docs/getting-started/` |
| `examples/` | lowercase | `examples/03-audio/` |
| `assets/` | lowercase | `assets/images/` |
| `tools/` | lowercase | `tools/pin_configuration/` |
| `scripts/` | lowercase | `scripts/windows/` |
| `releases/` | lowercase | `releases/firmware/` |
| `variants/` | lowercase | `variants/ai-explorer-s3/` |
| `.github/` | dot-prefixed | `.github/ISSUE_TEMPLATE/` |

### Example Category Folders
`NN-category` — zero-padded two-digit number + hyphen + category name:
- `01-basics/`
- `02-display/`
- `03-audio/`
- `04-storage/`
- `05-camera/`
- `06-advanced/`

### Example Sketches
`NN_sketch_name` — zero-padded two-digit number + underscore + snake_case:
- `01_blink/`
- `02_serial_test/`
- `01_oled_hello/`

### Documentation Folders
`kebab-case` for multi-word folder names:
- `getting-started/`
- `troubleshooting/`

---

## 4. Image Naming and Placement

### Naming Convention
```
[category]_[subject]_[description].[ext]
```

Examples:
| File | Description |
|------|-------------|
| `board_top_view.png` | Top-down photo of the board |
| `pinout_diagram.png` | Official pinout diagram |
| `wiring_oled_i2c.png` | Wiring diagram: OLED via I²C |
| `camera_ov2640_connected.jpg` | Camera module attached to board |
| `tft_hello_display.jpg` | TFT showing Hello World output |
| `logo_oceanlabz.png` | OceanLabz logo |
| `screenshot_serial_monitor.png` | Serial Monitor output screenshot |

### Placement
- **Example wiring diagrams**: Store in the example's own folder: `examples/XX-category/NN_example/wiring.png`
- **Board documentation images**: Store in `assets/images/`
- **Diagrams**: Store in `assets/diagrams/`
- **Screenshots**: Store in `assets/images/`

### Image Guidelines
| Property | Recommendation |
|----------|---------------|
| Format | PNG for diagrams/screenshots, JPG for photos |
| Max Width | 1200px for full-width, 600px for inline |
| Max File Size | 500KB per image |
| Resolution | 72 DPI minimum, 150 DPI preferred for print |
| Alt Text | Always include descriptive alt text in Markdown: `![Alt text](path)` |

---

## 5. Video Tutorial References

### Referencing in Markdown
```markdown
▶️ **Video Tutorial:** [Title of Video](https://youtube.com/watch?v=XXXX)
```

### Playlist Structure
All video tutorials should be organized into a YouTube playlist named:
```
OceanLabz AI Explorer S3 — Getting Started
OceanLabz AI Explorer S3 — Display Guides
OceanLabz AI Explorer S3 — Audio Guides
OceanLabz AI Explorer S3 — Camera Guides
OceanLabz AI Explorer S3 — Advanced Projects
```

### Video-to-Docs Correlation
Every major guide document should link to the corresponding video at the top:
```markdown
> ▶️ **Watch the video guide:** [Getting Started with AI Explorer S3](https://youtube.com/...)
```

### Thumbnail Standards
- Use consistent thumbnail template (OceanLabz branding)
- Include board image in thumbnail
- Use large readable text for titles

---

## 6. Markdown Formatting Rules

| Element | Rule |
|---------|------|
| Headers | ATX style (`##`, not underlined); space after `#` |
| Line breaks | One blank line before/after headers |
| Code blocks | Fenced with language tag: ` ```cpp ` |
| Tables | Use alignment markers (`:---`, `:---:`, `---:`) |
| Bold | `**text**` for emphasis |
| Italic | `*text*` for secondary emphasis |
| Inline code | `` `code` `` for pins, filenames, commands |
| Links | `[text](url)` — descriptive link text |
| Images | `![Alt text](path)` — always include alt text |
| Lists | `- ` for unordered, `1. ` for ordered |
| Line length | No hard wrap; use one sentence per line for git diffs |

---

## 7. Language and Tone

| Guideline | Standard |
|-----------|----------|
| Tone | Friendly, encouraging, professional |
| Audience | Beginner → Intermediate |
| Language | English (US) spelling |
| Pronouns | "You" (second person) for user actions |
| Voice | Active voice preferred ("Connect the wire" not "The wire should be connected") |
| Abbreviations | Spell out on first use: "Inter-IC Sound (I²S)" |

### Prohibited Terms
- "Easy" / "Just" / "Simply" — these dismiss the user's effort
- "Obviously" / "Of course" — assumes prior knowledge
- "Click here" — use descriptive link text instead

---

## 8. File Encoding

| Property | Standard |
|----------|----------|
| Encoding | UTF-8 (without BOM) |
| Line endings | LF (`\n`) — Unix-style |
| File extension | `.md` for Markdown, `.ino` for Arduino sketches |
| Max line length | No hard limit, but prefer ~80 chars for readability |

---

## 9. Translation Guidelines (Future)

When adding translations:

1. Create language folder: `docs/<lang-code>/` (e.g., `docs/zh-CN/`, `docs/es/`)
2. Translate only the documentation, not code comments
3. Maintain the same folder structure as `docs/en/`
4. Add language badge to the translated document:
   ```markdown
   > 🌐 This document is available in [English](README.md) | [简体中文](README.zh-CN.md)
   ```
5. Update the main `README.md` language switcher

---

> ⚡ **Remember:** Documentation is a product. Treat it with the same care as firmware code. Well-documented projects have fewer support tickets, happier users, and more contributors.