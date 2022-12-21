---
name: RST Release
about: Checklist for finalizing and testing new RST releases

---

# Release RST *version number*

- [ ] PATCH
- [ ] MINOR
- [ ] MAJOR

## Deadline: *set date*
Please comment in the Pull Request conversation if you need additional time to test the release.

## Testing

- [ ] Mac OSx
- [ ] Linux deb - Debian, Ubuntu, Mint
- [ ] Linux rpm - OpenSuse, Fedora, CentOS

*Do any binaries/libraries require special attention during testing?*

- [ ] `make_fit`
- [ ] `make_grid`
- [ ] map potential
- [ ] Documentation
- [ ] Other: ______

*If yes, please provide information about what testers should look for*

## Checklist

- [ ] Update `radar.dat` (if required)
- [ ] Update hardware files (if required)
- [ ] Update `.rst.version`
- [ ] Update author list in `.zenodo.json`
- [ ] Update author list in `AUTHORS.md`

## Extra Notes

*Example info:*
- *Do any hardware files need updating?*
- *Are there new dependencies needed?*

## Approving the release

After testing, please state which operating system you used and what you tested.

If you found no bugs during testing, please *approve* the PR:

1. Click on `files changed` top right
2. Click on `start review` green top right  button
3. In the comment box, state which operating system you used for testing, and any other helpful information
4. Select `approve` and `submit review`
