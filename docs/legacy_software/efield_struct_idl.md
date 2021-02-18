<!--
(C) copyright VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# IDL Data Structure for Electric Field Data

## Summary

A description of the IDL data structure used to store electric field data.

## Description

SuperDARN electric field data is derived from the global convection data. The data consists of a regular grid of derived electric field values. Within IDL a structure is used to store the parameters associated with the electric field data.

### Electric field parameters data structure `EFieldPrm`
```
prm={EFieldPrm, stme: $
         {EFieldTime,yr:0, $        ; start year
                   mo:0, $          ; start month
                   dy:0, $          ; start day
                   hr:0, $          ; start hour
                   mt:0, $          ; start minute
                   sc:0D}, $        ; start second
         etme: $
         {EFieldTime,yr:0, $        ; end year
                   mo:0, $          ; end month
                   dy:0, $          ; end day
                   hr:0, $          ; end hour
                   mt:0, $          ; end minute
                   sc:0D}, $        ; end second
         mlt:0.0, $                 ; magnetic local time
         mltflg:0B, $               ; position given in MLT not longitude
         pflg:0B, $                 ; potential value included
         vflg:0B}                   ; velocity measurements included
```

## References

This information is sourced form the RFC: 0025 previously in the RST RFC documentation that was written by R.J. Barnes.

## History

2004/11/22  Initial Revision.
