# file_sync

Cross-platform file sync tool (C++17 + CMake).
Monitors source directory and auto-syncs new/modified files to target directory.

## Build (Windows / Linux)
cmake -B build

cmake --build build
## Usage

Edit `config.json`:
{

"source_dir": "./test_source",

"target_dir": "./test_target",

"sync_interval_secs": 5

}
Then run:
## Status

- [x] Windows: polling monitor ✅
- [ ] Linux: inotify monitor (WIP, Phase 2)
- [ ] sendfile zero-copy (Phase 3)
- [ ] CLI args override (Phase 4)

## License

MIT
