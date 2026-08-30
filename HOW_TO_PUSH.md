# Publish Лох Universal on GitHub

The **project root** (`LohUniversal/`) is the Git repository. This `github/` folder is only the publishing notes — keep it in the repo so you have the checklist, but you do not copy it out.

Do **not** wrap the project in another folder. After `git push`, visitors should see `README.md`, `build.bat`, `CMakeLists.txt`, and `Source/` at the root.

## 1. Create an empty repo

1. [github.com/new](https://github.com/new)
2. Name it `LohUniversal` (not “Vox” anything — see [DISCLAIMER.md](DISCLAIMER.md))
3. **Public** or private, your choice
4. Leave it **empty**: no README, no .gitignore, no license (those are already in this project)
5. Copy the HTTPS URL, e.g. `https://github.com/YOURUSER/LohUniversal.git`

## 2. First push

Install [Git for Windows](https://git-scm.com/download/win). Open Git Bash or PowerShell **inside** `LohUniversal`:

```bash
git init
git add .
git status
git commit -m "Initial release: Loh Universal transistor combo organ"
git branch -M main
git remote add origin https://github.com/YOURUSER/LohUniversal.git
git push -u origin main
```

Replace `YOURUSER`. GitHub will ask you to sign in with a **Personal Access Token**, not your account password.

`git status` must **not** list `build/`, `JUCE/`, `logs/`, `*.vst3`, `*.exe`, or `ThirdParty/asiosdk/`. Those are in `.gitignore` on purpose.

## 3. Never commit

| Path | Why |
| --- | --- |
| `build/` | Compiled objects, hundreds of MB |
| `JUCE/` `_deps/` | CMake fetches these |
| `logs/` `build.log` `BUILD_STATUS.txt` | Your local compile logs |
| `*.vst3` `*.exe` `*.clap` | Binaries belong on a **Release**, not in git |
| Steinberg ASIO SDK | Redistribution is not allowed; JUCE 9 already bundles the headers |

## 4. GitHub Release (the download people actually want)

After the source is on `main`:

1. Build once with `build.bat` (Release).
2. Zip **only** the artefacts people run, for example:
   - `LohUniversal.vst3` (the bundle folder)
   - `LohUniversal.exe` (standalone)
   - Name the zip `LohUniversal-1.0.0-Windows-x64.zip`
3. GitHub → **Releases** → **Draft a new release**
4. Tag `v1.0.0`, title `Лох Universal 1.0.0`
5. Paste [RELEASE_NOTES.md](RELEASE_NOTES.md) into the description
6. Attach the zip. Do **not** attach `build/` or a JUCE tree.

Optional: also attach a source zip (`git archive` or GitHub’s own “Source code” asset is enough).

## 5. Later commits

```bash
git add -A
git commit -m "Describe the change"
git push
```

## 6. If push is rejected

- GitHub repo was not empty → don’t tick “Add a README” next time; for a brand-new empty-ish repo you can `git push --force` **once**.
- Auth failed → Fine-grained or Classic PAT with `repo` scope.
- Filename too long on Windows → enable long paths, or keep the clone close to `C:\`.
