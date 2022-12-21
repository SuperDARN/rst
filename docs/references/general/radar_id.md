<!-- Copyright (C) 2020 VT SuperDARN, Virginia Polytechnic Institute and State University 
author(s): Kevin Sterne

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->

This information is sourced from the RFC: 0003 and 0004 previously in the RST RFC documentation that was written by R.J. Barnes.

# Radar Identifier

## Summary

As the number of radars increased, the use of a single letter as a radar identifier became both confusing and eventually impractical.  The single letter identifier was replaced with a short text string.

## Introduction

SupeDARN radars have always been identified by two methods.  Within the software, each radar is identified by a unique station identifier number (site ID).  These numbers are difficult to remember and are very confusing to a normal human operator; consequently each radar is also assigned a single character station ID code.

When the number of radars was comparatively small, this approach worked well with each radar receiving an appropriate ID letter such as "g" for Goose Bay and "k" for Kapuskasing.  However as the number of radars has increased the number of available letters has decreased and the codes have become less logical and harder to remember.  Before the number of radars exceeded 26, the radar identifier was changed to be a short text string that is easy to remember.

## Format for the Radar Identifier

The summary for format in files after July 1, 2006 is:

- The radar identifier (single character) was changed to a short three letter text string for all existing radars
- Software should be capable of handling arbitrary length strings, for future expansion.  Though the standard for now is 3 characters.
- The three letter identifier uses lower case characters
- Software should treat the identifier as case sensitive

### Aliases

Radar identifiers will be allowed to have aliases.  For existing radars, the principal alias will be the existing single character identifier code.  This approach will allow backwards compatibility as a radar can be referenced either by the new identifier or the old.

An alias map for radars that transitioned from the single character to the three character identifier is found below.

| Station Number  | Radar Identifier | Single Character Alias | Name |
| :-------------  | :--------------: | :--------------------: | :--- |
| 01              |  gbr             |  g                     | Goose Bay |
| 02              |  sch             |  s                     | Schefferville |
| 03              |  kap             |  k                     | Kapuskasing |
| 04              |  hal             |  h                     | Halley |
| 05              |  sas             |  t                     | Saskatoon |
| 06              |  pgr             |  b                     | Prince George |
| 07              |  kod             |  a                     | Kodiak |
| 08              |  sto             |  w                     | Stokkseyri |
| 09              |  pyk             |  e                     | Pykkvibaer |
| 10              |  han             |  f                     | Hankasalmi |
| 11              |  san             |  d                     | Sanae |
| 12              |  sys             |  j                     | Syowa South |
| 13              |  sye             |  n                     | Syowa East |
| 14              |  tig             |  r                     | TIGER |
| 15              |  ker             |  p                     | Kerguelen |
| 16              |  ksr             |  c                     | King Salmon |
| 18              |  unw             |  u                     | Unwin |
| 32              |  wal             |  i                     | Wallops Island |


## References

- [Filename Format Definition](filename.md)

## History (from RFC)

- 2004/06/02 Initial Revision, RJB

