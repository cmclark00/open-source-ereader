# How to Use These Auto Run Documents

This guide explains how to execute the phase documents with an AI coding assistant to build your e-reader project.

## What Are Auto Run Documents?

Auto Run documents are task-based instructions designed for AI coding assistants (like Claude Code, Cursor, GitHub Copilot, etc.). Each document contains a list of tasks that the AI can execute autonomously to build your project.

## Document Structure

Each phase document follows this format:

```markdown
# Phase XX: [Title]

[Description of what this phase accomplishes]

## Tasks

- [ ] First task with clear objectives:
  - Sub-item detailing specific work
  - Another sub-item
  - More details

- [ ] Second task:
  - More specific work
  - And more

...
```

## How to Execute a Phase

### Option 1: With Claude Code (Recommended)

1. **Open your terminal** in the project directory
2. **Start Claude Code**: Run `claude` in your terminal
3. **Load the phase document**: Say "Please execute the tasks in Phase-01-Foundation-and-Working-Display.md"
4. **Let it work**: Claude will read the document and execute tasks one by one
5. **Review progress**: Claude will report completion and ask for verification
6. **Move to next phase**: When Phase 1 is complete, load Phase 2

### Option 2: With Other AI Tools

1. **Copy the phase document** content
2. **Paste into your AI tool** (Cursor, Copilot Chat, etc.)
3. **Say**: "Please execute these tasks in order, checking off each one as you complete it"
4. **Monitor progress**: The AI should work through tasks autonomously
5. **Intervene if needed**: If AI gets stuck, provide guidance

### Option 3: Manual Execution

You can also use these documents as a human checklist:

1. **Read each task carefully**
2. **Complete the work** described in the task and sub-items
3. **Check it off** when done
4. **Move to next task**
5. **Verify phase completion** before moving on

## Task Execution Guidelines

### What the AI Should Do

For each task, the AI should:
- **Read all sub-items** to understand the full scope
- **Create/modify files** as specified
- **Write code** with proper error handling
- **Document decisions** in progress logs
- **Test when possible** (though hardware testing requires your intervention)
- **Report completion** with summary of what was done

### What You Should Do

As the human:
- **Verify critical steps** (kernel configs, device tree overlays)
- **Perform hardware tasks** (wiring, soldering, SD card flashing)
- **Test on real hardware** (AI can't test on Pi Zero W)
- **Provide feedback** if something doesn't work
- **Make decisions** if AI encounters ambiguity

## Checkpoints and Verification

### After Each Task

Check that:
- [ ] Files were created in correct locations
- [ ] Code compiles without errors (if applicable)
- [ ] Documentation is clear and complete
- [ ] No obvious mistakes or omissions

### After Each Phase

Verify:
- [ ] All tasks are checked off
- [ ] Phase deliverables are present
- [ ] Progress log is updated
- [ ] Ready to move to next phase

### Hardware Testing Points

You must manually test:
- **Phase 1**: Boot Pi, verify display shows "Hello E-Reader"
- **Phase 2**: Press buttons, verify events are detected
- **Phase 3**: Navigate menu, open books, turn pages
- **Phase 4**: Open EPUB and PDF files, verify rendering
- **Phase 5**: Test bookmarks, settings, sleep mode
- **Phase 6**: Connect WiFi, download book from internet

## Common Issues and Solutions

### "I don't have Buildroot installed"

Phase 1 includes downloading Buildroot. The AI should do this in the appropriate task. If it skips this, remind it: "Please download Buildroot 2024.02.x as specified in the task."

### "The build takes too long"

Buildroot compilation can take 1-2 hours on first build. This is normal. The AI will start the build but can't wait for it to complete. You'll need to:
1. Let the build run in background
2. Check on it periodically
3. Continue with documentation tasks in parallel
4. Verify build output when complete

### "I don't know the exact pin numbers"

Some tasks require specific technical details (GPIO pin numbers, SPI settings). The AI should:
1. Research in the task's documentation links
2. Make reasonable defaults based on standard Pi Zero W pinouts
3. Document the choice clearly
4. Note that it may need adjustment based on hardware testing

If AI can't decide, it should ask you.

### "The AI is stuck or making mistakes"

If the AI:
- Skips sub-items → Remind it to complete all sub-items
- Creates wrong files → Point out the error, ask it to fix
- Doesn't understand the task → Rephrase the task in your own words
- Gives up → Encourage it to try different approaches

## Phase Progression

### Recommended Flow

```
Start → Phase 1 → Test Display → Phase 2 → Test Buttons →
Phase 3 → Test Reading → Phase 4 → Test Formats →
Phase 5 → Test Features → Phase 6 → Test WiFi →
Phase 7 → Release! 🎉
```

### Can I Skip Phases?

**Don't skip Phases 1-3** - They're the foundation.

You can:
- Skip Phase 4 if you only need TXT files
- Skip Phase 5 if you don't need bookmarks/settings
- Skip Phase 6 if you don't need WiFi
- Skip Phase 7 if you're not releasing publicly

But each phase builds on the previous, so skipping makes later phases harder.

## Tips for Success

### For AI Execution

- **Be specific**: If output isn't what you want, describe exactly what should change
- **Provide context**: Remind AI of previous decisions if it forgets
- **Verify critical files**: Double-check device tree overlays, kernel configs
- **Save progress**: Commit to git after each phase

### For Hardware Work

- **Test incrementally**: Don't wire everything at once
- **Document actual pins**: The AI guesses, you verify and document reality
- **Take photos**: Document your wiring for troubleshooting
- **Label wires**: Future-you will thank you

### For Debugging

- **Serial console is your friend**: Connect USB-to-TTL adapter for boot logs
- **Kernel messages**: `dmesg` shows hardware initialization
- **Check connections**: 90% of issues are wiring/pinout errors
- **Start simple**: If complex thing fails, try simpler test first

## Working Folder

Each phase can use `Auto Run Docs/Initiation/Working/` for:
- Temporary notes and research
- Downloaded files (datasheets, examples)
- Build logs and error messages
- Experimental code before moving to `src/`

This folder is your scratch space. Clean it up when phase is complete.

## Documentation as You Go

The phases create structured documentation in `docs/`:

- **Front matter**: YAML metadata for each doc
- **Wiki links**: `[[Other-Doc]]` connects related docs
- **Tags**: Make docs searchable
- **Types**: research, architecture, testing, etc.

This creates a knowledge graph you can explore later with Obsidian or similar tools.

## When Phase 7 is Complete

You'll have:
- ✅ Working e-reader device
- ✅ Complete source code
- ✅ Hardware designs and schematics
- ✅ Comprehensive documentation
- ✅ Bootable SD card image
- ✅ Ready for open source release

At that point:
1. **Test everything one more time**
2. **Create GitHub repository**
3. **Upload release image**
4. **Announce to community**
5. **Celebrate!** 🎉

## Questions?

If you're unsure about anything:
1. Check `00-PROJECT-OVERVIEW.md` for big picture
2. Read the specific phase document carefully
3. Check `docs/progress/` logs for past decisions
4. Ask your AI assistant to explain
5. Document your question for future reference

## Ready to Start?

Open `Phase-01-Foundation-and-Working-Display.md` and let's build an e-reader! 🚀

---

**Remember**: These documents are a guide, not gospel. Adapt as needed for your specific hardware, preferences, and constraints. The goal is a working e-reader, not perfect adherence to a plan.

Good luck! 📚
