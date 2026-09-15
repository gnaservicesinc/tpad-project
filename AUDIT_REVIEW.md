# Audit review for 7.3.0.5

Reviewed 2026-09-15 against the current GTK 4, GTK 3, and Qt sources.  A
finding is marked **fixed** only where its claimed behavior was reproduced or
confirmed from an active code path.  **Intentional** means the report described
real code but misclassified established tpad behavior as a defect.  **Rejected**
means the claimed failure is unreachable, contradicted by the implementation,
or merely hypothetical.

In particular, tpad is intentionally a one-document-per-window editor.  Bare
Home and End intentionally move to the beginning and end of the document, a
missing command-line path intentionally creates a named empty document, zero is
the documented unlimited recent-file count, and dangling symlinks are refused
as unsafe save targets.  This remediation does not change those behaviors.

## Anthropic audit

| # | Disposition | Evidence / resolution |
|---:|---|---|
| 1 | Fixed | The duplicate line-number default was a 2018 copy/paste regression; the preceding implementation and Qt frontend both default to visible line numbers. |
| 2 | Fixed | `_TPAD_STACKS_MAX` now guards the same macro it defines in both frontend headers. |
| 3 | Fixed | `_SEARCH_FOR` now has the matching guard in both frontend headers. |
| 4 | Fixed | `POS_AUTO_TAB` is now defined only when `AUTO_TAB_TOGGLE` is enabled and not already supplied. |
| 5 | Fixed | `_FIND_AND_REPLACE` aliases the already translated label instead of translating it twice. |
| 6 | Fixed by removal | The unwired hex entry point and method constant were incomplete dead code. They were removed rather than inventing a new transformation. |
| 7 | Partly fixed | All identified user-visible spelling errors were fixed. Internal misspellings were retained because renaming persistent fields and long-standing constants has no user benefit and creates compatibility churn. |
| 8 | Rejected | Watch/modbal sources are deliberately excluded from `tpad_SOURCES` and shipped only in `EXTRA_DIST`; no active feature calls these stubs. |
| 9 | Rejected | The report itself establishes that `output` is freed exactly once. No double-free exists. |
| 10 | Rejected | `tpad_recent_files_load()` always constructs and returns a `GPtrArray`; GLib allocation failure is fatal rather than a nullable result. |
| 11 | Rejected | The short-lived launcher exits, orphaning active children to the system reaper; it does not leave persistent zombies. This preserves historical non-blocking launch behavior. |
| 12 | Rejected | A redundant defensive NULL comparison is not a defect. |
| 13 | Rejected | An inaccurate comment in an excluded legacy source has no program impact. |
| 14 | Rejected | A filename typo in a copyright comment has no program impact. |
| 15 | Fixed | `str_size()` and its debug caller now tolerate NULL. |
| 16 | Intentional | `--\n` is the legacy conversion-failure marker; the audit supplied no evidence that copying it is a regression. |
| 17 | Intentional | Bare Home/End document navigation is a long-standing tpad usage quirk and is preserved. Modified variants remain available to GTK. |
| 18 | Rejected | The seven-`int` file has a stable size on supported ABIs and is size/range checked with an explicit legacy migration. Replacing the format was not justified by a demonstrated failure. |
| 19 | Cleaned up | `FALSE` was numerically correct for the documented unlimited undo value; it is now spelled `0` to reflect that the field is a count. |
| 20 | Rejected | The self-binary hash helper has no active UI or caller, and the GTK frontend is Linux-targeted. |
| 21 | Intentional | Opening another document in another window is tpad's one-document-per-window model. |
| 22 | Fixed | Invalid settings are now printed as readable fields only in debug builds, never as raw binary. |
| 23 | Rejected | The guard's purpose is to require one user decision when any known marker exists; enumerating every simultaneous marker would repeat the same warning. |
| 24 | Intentional | The compact horizontal find/replace layout is established tpad UI, not a correctness issue. |
| 25 | Fixed | Replace now consumes a matching current selection and searches only when the selection does not match. |
| 26 | Intentional | Translation catalogs deliberately map `en_US` to locale-specific spell dictionaries (for example `fr` and `de`). |
| 27 | Rejected | Guarded duplicate system includes do not change runtime behavior; broad cleanup is outside defect remediation. |
| 28 | Rejected | The activation data remains owned until `g_application_run()` returns, and the non-unique application is not reused for remote activation. |
| 29 | Accepted risk | In-place truncation can lose preferences after an interrupted write, but startup falls back safely. The mature writer was retained to avoid disproportionate complexity for a tiny settings file. |
| 30 | Fixed | Clipboard selection text is escaped as a complete COVT round trip, including literal backslashes. |
| 31 | Rejected | The editor and insertion APIs require valid UTF-8 text and file loading rejects NUL bytes; the report did not identify an active path that inserts embedded NULs. |
| 32 | Fixed | The two unreachable duplicate MD5 label definitions were removed. |
| 33 | Rejected | The display is explicitly referenced when stored and unreferenced after provider removal; ownership is balanced. |

## Google audit

| # | Disposition | Evidence / resolution |
|---:|---|---|
| 1 | Fixed | Existing paths are now opened and inspected with `fstat()` on that descriptor instead of `stat()` after an `EEXIST` race. |
| 2 | Rejected | Directory dispatch was disabled long ago; an untitled Save As intentionally falls back to the home directory. |
| 3 | Accepted risk | Same as Anthropic 29; the failure loses preferences, not document data, and safe defaults recover startup. |
| 4 | Fixed | Replace no longer skips a matching current selection in GTK 4, GTK 3, or Qt. |
| 5 | Fixed | Replace All honors Match Case in all three frontends. Literal regex metacharacters remain literal. |
| 6 | Intentional | Zero is explicitly documented and tested as unlimited; history has a separate enable/disable setting. |
| 7 | Intentional | A dangling symlink is an ambiguous and unsafe save destination and remains rejected. |
| 8 | Intentional | The save decision precedes every Open action in tpad's one-document-per-window workflow. |
| 9 | Overstated; superseded | Two linear scans were not a severe bottleneck, but the helper was replaced as part of the case-correct literal-regex implementation. |
| 10 | Rejected | `get_file_type()` belongs to bypassed legacy dispatch and is not called by the active open path. |
| 11 | Rejected | The watch source is excluded from the executable. |
| 12 | Rejected | The watch source is excluded from the executable. |
| 13 | Fixed | A failed guard-file claim now clears the untracked candidate path immediately. |
| 14 | Rejected | Buffer iterator traversal is the supported Unicode word-boundary API; no measured regression was supplied. |
| 15 | Fixed | One replacement is enclosed in one GTK user action; Replace All is likewise one undo step. |
| 16 | Intentional | A missing supplied path creates a named empty document, matching tpad's historical command-line behavior and the Qt frontend. |
| 17 | Fixed | Active paths now have an explicit invariant: UI/state strings are UTF-8 and every filesystem/spawn boundary converts to or from the platform filename encoding. |
| 18 | Rejected | Frequency analysis needs the document snapshot it analyzes; the report showed no leak or incorrect output. |
| 19 | Intentional | Same as Anthropic 17. |
| 20 | Fixed by removal | Same dead hex entry point as Anthropic 6. |
| 21 | Fixed | Integer `GDK_CURRENT_TIME` values now use an integer format in debug output. |
| 22 | Fixed | Match Case is wired into GTK 4, GTK 3, and Qt find/replace dialogs. |

## GPT-OSS audit

| # | Disposition | Evidence |
|---:|---|---|
| 1 | Rejected | `HAVE_LIBUNITY` is optional, disabled code; a FIXME comment cannot cause a runtime crash. |
| 2 | Rejected | `open()` was checked and its failure returned an error before any write. |
| 3 | Rejected | Directory `fsync()` is explicitly best effort after the file has been fully synced and renamed. |
| 4 | Rejected | The file-dialog wrapper returns a nullable result and every caller checks it. |
| 5 | Rejected | Both configuration `fopen()` results were checked before use. |
| 6 | Rejected | Same as 5. |
| 7 | Rejected | The `fdopen()` result is checked before use and its failure follows the configuration error path. |
| 8 | Rejected | The recent-file lock descriptor and `flock()` result are both checked and errors propagate. |
| 9 | Rejected | `resolved` had one free on each mutually exclusive path. |
| 10 | Rejected | Temporary-save cleanup centralizes one free after mutually exclusive control flow. |
| 11 | Rejected | `output` had one owning free and no earlier free on the success path. |
| 12 | Rejected | Missing paths are an intentional named-document creation workflow, not undefined UI state. |
| 13 | Rejected | The async completion callback stores either the result or `GError`; the synchronous wrapper reports non-dismissal errors. |
| 14 | Rejected | Directory dispatch is explicitly bypassed, while the active read rejects directories cleanly. |
| 15 | Rejected | Pango's free function accepts NULL and the pointer is not used afterward. |
| 16 | Rejected | `initial_path` is freed once and GLib free functions accept NULL. |
| 17 | Rejected | A comment naming Unity and the `HAVE_LIBUNITY` feature macro are consistent. |
| 18 | Rejected | Runtime and repository notices both specify GPL v3-or-later. |
| 19 | Rejected | Whole-file text loading is the editor's deliberate model, with binary input rejected; no correctness failure was shown. |
| 20 | Rejected | Save errors, including `EACCES`, are propagated through `GError` and shown by `gerror_warn()`. |

## Additional defects found and fixed

- The Qt frontend had the same current-selection skip and hard-coded
  case-insensitive behavior as GTK; its implementation and regression test now
  cover selected replacement, one-step undo, and both Replace All modes.
- Qt's COVT-enabled initial selection had no inverse escaping step, so literal
  backslash sequences could be decoded as control characters.  It now uses the
  same byte-preserving escape contract as GTK.
- The filename-encoding defect extended beyond the one audited `open()` call to
  reads, saves, hashing, recent files, guard files, dialogs, and child-process
  arguments.  Those active boundaries now consistently convert while stored
  paths remain valid UTF-8 for GTK and GVariant use.
- The GTK 4 whole-buffer Replace All path used an irreversible set-text action;
  it now performs an explicit delete/insert inside one undoable user action,
  matching GTK 3 and Qt.
