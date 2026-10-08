# BasicApkFormat

A small C project for trying out a basic binary package format. It stores text entries in an `.eaft` file, with a header and a table of offsets and sizes.

Build with GCC and Make on Windows:

```sh
make
```

From PowerShell, run it inside `bin`:

```powershell
cd bin
.\BasicAPK.exe --write "hello,world"
.\BasicAPK.exe --run
```

`--write` replaces `example.eaft` with the comma-separated entries. `--run` reads them back and prints them. The file path is currently hardcoded to `../example.eaft`.
