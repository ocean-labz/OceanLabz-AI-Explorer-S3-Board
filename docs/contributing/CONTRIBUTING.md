# 🤝 Contributing to OceanLabz AI Explorer S3 Board

We're thrilled you want to contribute! Whether you're fixing a typo, adding an example, or suggesting a new feature — every contribution helps make the AI Explorer S3 Board better for everyone.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How to Contribute](#how-to-contribute)
- [Reporting Issues](#reporting-issues)
- [Submitting Changes](#submitting-changes)
- [Example Contribution Guidelines](#example-contribution-guidelines)
- [Documentation Guidelines](#documentation-guidelines)
- [Style Guide](#style-guide)
- [Pull Request Process](#pull-request-process)

---

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to [oceanlabz@example.com](mailto:oceanlabz@example.com).

---

## How to Contribute

1. 🍴 **Fork** the repository
2. 🌿 **Create a branch** (`git checkout -b feature/your-feature`)
3. ✏️ **Make your changes**
4. ✅ **Test** your changes
5. 📝 **Commit** using clear commit messages
6. 🚀 **Push** to your fork
7. 🔄 **Open a Pull Request**

---

## Reporting Issues

Use the [GitHub Issue Tracker](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/issues). Choose the right template:

| Issue Type | Template | When to Use |
|------------|----------|-------------|
| 🐛 Bug Report | [bug_report.yml](.github/ISSUE_TEMPLATE/bug_report.yml) | Something doesn't work |
| ✨ Feature Request | [feature_request.yml](.github/ISSUE_TEMPLATE/feature_request.yml) | You have an idea for improvement |
| ❓ Question | [question.yml](.github/ISSUE_TEMPLATE/question.yml) | You need help using the board |
| 📖 Documentation | [documentation.yml](.github/ISSUE_TEMPLATE/documentation.yml) | Docs are unclear or missing |

### Good Bug Reports Include:
- Board setup details (power source, peripherals connected)
- Arduino IDE version and ESP32 core version
- Full error output (paste, don't screenshot)
- Minimal code to reproduce
- What you expected vs. what happened

---

## Submitting Changes

### For Beginners
- **Simple fixes** (typos, links, formatting): Edit directly on GitHub
- **Documentation improvements**: Edit the Markdown files in `docs/`
- **Example bug fixes**: Fix the `.ino` file in the relevant `examples/` folder

### For Experienced Contributors
- Add new examples following the [Example Guidelines](#example-contribution-guidelines)
- Improve tooling in `tools/` or `scripts/`
- Add translation of documentation

---

## Example Contribution Guidelines

When adding a new example sketch:

### Folder Structure
```
examples/
└── XX-category/
    └── NN_example_name/
        ├── example_name.ino         # Main sketch file
        ├── README.md                 # Documentation (required)
        ├── wiring.png                # Fritzing/wiring diagram (optional)
        └── libraries.txt             # Required libraries list (required)
```

### Naming Convention
- **Folder**: `NN_descriptive_name` (zero-padded number + underscore + snake_case)
- **File**: `descriptive_name.ino` (snake_case, matching folder name)
- **Category folders**: `NN-category` e.g., `03-audio`

### Every Example README Must Include:
```markdown
# Example Title

## Description
What does this example do? (~2-3 sentences)

## Required Hardware
- OceanLabz AI Explorer S3 Board
- [list any additional components]

## Required Libraries
- LibraryName by Author (install via Library Manager)

## Wiring
Pin mapping table or reference to board pinout.

## Expected Output
What should the user see/hear/measure?

## Troubleshooting
Common issues specific to this example.
```

### Code Style
- Use `//` comments for explanations
- Use `/* */` block comments only for file headers
- Include a header block with filename, description, author, date
- Pin definitions at the top — use `#define` or `const int`
- All text in `Serial.println()` should be in **English**
- Use `delay(10)` minimum in loops to prevent watchdog issues

---

## Documentation Guidelines

See [Documentation Standards](docs/contributing/documentation-standards.md) for:
- Markdown formatting rules
- Image naming and placement
- Code block formatting
- Link structure
- Translation guidelines

---

## Style Guide

### Git Commits
- Use present tense ("Add camera example" not "Added camera example")
- First line: 50 chars max, summary
- Body (optional): wrap at 72 chars

### Markdown
- Use ATX headers (`##` not underlined)
- One blank line before and after headers
- Code blocks with language tags: ` ```cpp `
- Tables with alignment markers (`:---:` for center)

### Arduino Code
- `camelCase` for variables, functions
- `UPPER_SNAKE_CASE` for pin definitions
- 2-space indentation
- Opening brace on same line (`if (x) {`)

---

## Pull Request Process

1. Ensure your PR addresses a single concern
2. Update the `examples/README.md` index if adding examples
3. Update the main `README.md` feature table if adding new functionality
4. Ensure all CI checks pass (if applicable)
5. Request review from a maintainer
6. A maintainer will merge once approved

---

## Need Help?

Open a [Discussion](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/discussions) or ask in the [Issues](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/issues) section.

Thank you for contributing to the OceanLabz AI Explorer S3 Board! 🌊