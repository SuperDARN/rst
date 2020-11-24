<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

This information is sourced from the RFC: 0010 previously in the RST RFC documentation that was written by R.J. Barnes.

# rPlot XML vector graphics specification

## Summary

A description of the rPlot, XML based, vector graphics specification.

## Introduction
The rPlot format is an XML based specification for describing vector graphics. It's purpose is similar to that of the W3C <a href="http://www.w3.org/TR/SVG/">Scalable Vector Graphics (SVG)</a>specification. However, rPlot was developed independently and with slightly different aims.
The original concept of the rPlot format was to solve the problem of producing data plots that looked good both on the printed page and on the screen. For many years the SuperDARN analysis software has included plotting tools that manage to accomplish this. The tools rely on two graphics libraries, one for producing PostScript output and the other for bitmap images; both libraries had a similar programming interface. The problem was that every plotting program had to have duplicate calls to each library to produce either PostScript or bitmap output. The obvious solution was to create a new interface library that would have a single interface that in turn would call the PostScript or bitmap library depending on the specified output.
In designing this interface library it quickly became apparent that it would be a good idea to somehow capture and store the graphics commands before they were passed onto the output libraries. By doing this, PostScript or bitmap files could be recreated without having to re-run the plotting tool. The idea of an interface library was replaced by the concept of a vector based graphics file format; rPlot.
One of the first requirements of the rPlot specification was that it must be extendible so that new features could be added as they were required, the second requirement was that the files should be in plain text so that a simple text editor could be used to either create them or edit them. At this point it became obvious that the format should be based on the eXtensible Markup Language (XML). 

### rPlot Concepts
- An rPlot file must be a fully conforming XML document.
- A file is comprised of graphics primitives, such as a line or curve.
- A primitive is an XML tag
- A primitive is defined by the content of a the tag. For example, the contents of an ellipse tag defines its width and height.
- A primitve can have attributes that define its properties such as color, line thickness, fill value and position on the page.
- The rPlot document is parsed by a renderer that extracts graphics primitives and renders them one by one.
- Graphics primitives are rendered sequentially from the start of the document. If commands overlap, the current command is painted over the top of the previous.
- rPlot documents can be embedded inside other rPlot documents. This allows complicated plots to be constructed from simpler ones.
- rPlot documents have arbitrary dimensions, the renderer determines how big the final plot is.

## rPlot Graphic Primitive Tags
The rPlot specification defines a number of simple graphical primitives as listed below:

| Tag Name       | Definition                |
| :=======       | :==========               |
| `bezier`         | Simple Bezier curve.      |
| `ellipse`        | Ellipse.                  |
| `image`          | Inline bitmap image.      |
| `imagen`         | Bitmap image by name reference. |
| `line`           | Simple line.              |
| `polygon`        | Polygon.                  |
| `rectangle`      | Rectangle.                |
| `text`           | Text string.              |

## Other Tags
In addition to the graphic primitive tags, the rPlot specification includes some addition tags that are not rendered but provide useful features:


| Tag Name      | Definition                  |
| :========     | :========                   |
| `clip`        | Define a clipping polygon. Subsequent graphic primitives will be clipped to this polygon. |
| `embed`       | Embed another rPlot document within the current document. |
| `include`     | Temporarily redirect input to another location. |
| `info`        | Meta-data container.        |
| `rplot`       | The root element.           |

## Tag Contents and Attributes
The contents of a tag define the primitive, its size and other characteristics. A tag can also have attributes that define physical properties of the primitive:</p>
```
<ellipse color="ffff0000" mask="0f" width="0" 
        position="270 270" fill="1">
  <s>
    <d>
      <w>220</w>
      <h>150</h>
    </d>
  </s>
</ellipse>
```
In the above example a filled ellipse of width 220 units and height 150 units is plotted in red (color="ffff0000"), at position x=270, y=270.

## A basic rPlot document "Hello World"
```
<rplot name="hellow.rp.xml" width="540" height="540" depth="24">
<info>
<created>Fri Nov  5 17:55:46 2004</created>
<author>R.J.Barnes</author>
<version><major>1</major><minor>0</minor></version>
</info>
<ellipse color="ffff0000" mask="0f" width="0" position="270 270" fill="1">
<s><d><w>220</w><h>150</h></d></s>
</ellipse>
<text color="ff00ff00" mask="0f" font="Helvetica" 
       size="34" position="180 280" smooth="1">
<t>Hello World</t>
</text>
</rplot>
```
The root element of the rPlot document is the <code>rplot</code> tag. This defines document name, size and color depth. All subsequent tags are nested within this one. The next tag, <code>info</code> is the meta-data container. The meta-data for rPlot documents is not yet defined, so this tag can be used to contain any information about the plot that you want. rPlot renderers currently ignore the <code>info</code> tag and any nested sub-tags within it. In this example a simple set of meta-data defining the date the plot was generated, the author and version number are included. The remaining two tag pairs define a simple ellipse in red and the "Hello World" text in green.

## Common Attributes
- `position="*x y*"`
The <code>position</code> attribute defines the position of an element on the page or screen. The position is given in terms of the X and Y offset from the top left-hand corner of the plot.

- `color="*aarrggbb*"`
The <code>color</code> tag defines attribute defines the color of a primitive in terms of the red,green,blue and alpha (transparency) channels. The attribute value *aarrggbb* is in hex, *aa* gives the value of the alpha channel, *rr* gives the value of the red channel, *gg* gives the value of the green channel and *bb* gives the value of the blue channel. All values range between 0 (off) and ff  (fully on).
The alpha channel is implemented by the renderer and some output devices (PostScript), do not support it.

- `mask="*m*"`
The <code>mask</code> attribute defines the RGBA color mask. The mask value *x* is a hexadecimal number and represents which color channels are enabled. The top (4th) bit of *x* controls the alpha channel, the third bit controls the red channel, the second bit controls the green channel and bottom (1st) bit controls the blue channel. If a bit is set, the channel is enabled otherwise the channel is masked out. A value of <code>f</code> enables all channels.
The <code>mask</code> attribute is closely coupled with the <code>color</code> attribute and allows you to control the output channels of a plot without having to modify the color directly.
 
- `width="*w*"`
The <code>width</code> attribute defines the line width of the primitive. A value of zero will produce hairlines - lines with the smallest width for designated output type.

- `fill="*f*"`
The <code>fill</code> attribute defines whether the primitive should be filled or not. A value of <code>1</code> will produce filled polygons and shapes. If this attribute is omitted then only object outlines will be drawn.

- `dash="*phase dash*"`
The <code>dash</code> attribute defines a dot-dash pattern to use when drawing the primitive. The dash pattern is defined in the same way that PostScript dot-dash patterns are defined. The space seperated list of dash lengths given by *dash* is cycled through, the first element give the length of line to draw, the next element gives the length of the gap to leave. The renderer will continue alternating lines and gaps until all elements of *dash* are exhausted, it will the repeat at the beginning. The phase value given by *phase* is used as the initial offset into *dash* that the renderer starts from.

- `matrix="*a b c d*"`
The <code>matrix</code> attribute applies a matrix transformation on the graphics primitive. The tranformation takes the form:
<!--
Let's see if this table works?
-->
<center><table>
<tr><td rowspan="2"><font size="+3">(</font></td><td>x'</td>
<td rowspan="2"><font size="+3">)</font></td><td rowspan="2">=</td>
<td rowspan="2"><font size="+3">(</font></td><td>a</td><td>b</td>
<td rowspan="2"><font size="+3">)</font></td>
<td rowspan="2"><font size="+3">(</font></td><td>x</td>
<td rowspan="2"><font size="+3">)</font></td>
</tr>
<tr><td>y'</td><td>c</td><td>d</td><td>y</td></tr>
</table></center>

## Embed and Include
The great power of the rPlot specification is the ability to embed one document inside another This seemingly simple feature allows you to create very complicated plots. The <code>embed</code> has attributes that let you position and transform the embedded rPlot document, this allows you to easily create multi-panel plots. As rPlot documents do not have a solid background color, you can use the embed feature to create multi-layer plots or overlays to existing plots.
The <code>include</code> tag is used to include another file in the current rplot document. When the renderer encounters the <code>include</code> tag it will temporarily stop reading the rPlot document and instead read input from the file named in the tag. Once the new file has been parsed, the renderer will continue reading from the original file.
By combining the <code>embed</code> and <code>include</code> tags you can create complicated multi-panel or multi-layer plots with just a few lines. The example below demonstrates how a multi-layer plot can be constructed:
``` 
<rplot width="540" height="540" depth="24">
<rectangle position="0 0" color="ffffffff" fill="1" mask="0f">
<s><d><w>540</w><h>540</h></d></s>
</rectangle>
<imagen mask="0f" position="0 0" matrix="1 0 0 1">
<name>background.ppm</name>
</imagen>
<embed position="0 0">
<include><name>overlay.rp.xml</name></include>
</embed>
</rplot>
```
The example overlays the rplot document "overlay.rp.xml" onto the image "backround.ppm".

## rPlot Tags

### rplot
```
<pre><rplot name="*name*" width="*w*" height="*h*" depth="*d*">
.
</rplot>
```
__Attributes__

| name="*name*" | assigns the string *name* as the document name or title. The renderer may or may not use this to reference the document.
| width="*w*"   | sets the width of the plot to *w* graphics units. |
| height="*h*"  | sets the height of the plot to *h* graphics units. |
| depth="*d*"   | sets the color-depth of the plot to *d* bits. Currently only the 24-bit color depth is supported. |

__Description__

The top level rPlot document tag. All subsequent tags are nested within this tag.

### info
```
<info>
.
</info>
````

__Description__

This is the meta-data tag. The content of this tag is comprised of XML formatted meta-data. Currently there is no standard for the meta-data and the user is free to populate this tag as they see fit.

### embed
```
<embed position="*x* *y*" matrix="*a* *b* *c* *d*">
  <rplot>
   .
  </rplot>
</embed>
```
__Attributes__

| position="*x y*" | positions the top left of the embedded document at the coordinates given by *x* and *y*. |
| matrix="*a b c d*" | applies a two dimensional transform to embedded document. The tranformation matrix is constructed from *a*,*b*,*c* and *d*. | 

__Description__

Embeds an rplot document. The content of this tag must be a valid rPlot document including the `rplot` root tag pair.

### include
```
<include>
   <name>*filename*</name>
</include>
```

__Description__

Temporarily redirects input to the file *filename*. The <code>include</code> tag can be placed anywhere in the rPlot document, consequently the tag can be used to either include whole plots or just import a single value.

### text
```
<text color="*aarrggbb*" mask="*m*" font="*fontname*" size="*fontsize*" 
       smooth="*s*" position="*x* *y*" matrix="*a* *b* *c* *d*"&gt;
*text*
</text>
```

__Attributes__

| color="*aarrggbb*" | color the text string according to the hexadecimal number *aarrggbb*. The alpha value is given by *aa*, the red value by *rr*, the green value by *gg*, and the blue value by *bb*.  |
| mask="*m*"         | set the color channel mask to *m* |
| font="*font*"      | use the font *fontname*.                 |
| font="*size*"      | use a font size of *fontsize* points.    |
| smooth="*n*"       | If n is set to 1, the renderer will attempt to smooth the font.  |
| position="*x y*"   | positions the top left of the text at the coordinates given by *x* and *y*.  |
| matrix="*a b c d*" | applies a two dimensional transform to the text string. The tranformation matrix is constructed from *a*, *b*, *c* and *d*.  |

__Description__

Plots the text string *text*. Special characters such as "<" and ">" must be replaced by their equivalent XML entities.

### line
```
<line color="*aarrggbb*" mask="*m*" 
       width="*w*" dash="*phase dash....*">
<s><p><x>*startx*</x><y>*starty*</y></p></s>
<e><p><x>*endx*</x><y>*endy*</y></p></e>
</line>
```

__Attributes__

| color="*aarrggbb*"    | color the line according to the hexadecimal number *aarrggbb*. The alpha value is given by *aa*, the red value by *rr*, the green value by *gg*, and the blue value by *bb*.
| mask="*m*"            | set the color channel mask to *m* |
| width="*w*"           | set the line thickness to *w*. A value of zero will produce hairlines.  |
| dash="*phase dash...*"  | set the dash pattern for the line using the phase value *phase* and the space separated list of dash lengths *dash..* | 

__Description__

Draw a simple line between *startx*,*starty* and *endx*,*endy*.</p>


### bezier 
```
<bezier color="*aarrggbb*" mask="*m*" 
       width="*w*" dash="*phase* *dash....*">
<a><p><x>*x1*</x><y>*y1*</y></p></a>
<b><p><x>*x2*</x><y>*y2*</y></p></b>
<c><p><x>*x3*</x><y>*y3*</y></p></c>
<d><p><x>*x4*</x><y>*y4*</y></p></d>
</bezier>
```

__Attributes__

| color="*aarrggbb*"    | color the line according to the hexadecimal number *aarrggbb*. The alpha value is given by *aa*, the red value by *rr*, the green value by *gg*, and the blue value by *bb*.  |
| mask="*m*"            | set the color channel mask to *m*   |
| width="*w*"           | set the line thickness to w. A value of zero will produce hairlines.   |
| dash="*phase dash...*" | set the dash pattern for the line using the phase value *phase* and the space separated list of dash lengths *dash..*  |

__Description__

Draw a bezier line between *x1*,*x2* and *x4*,*x4*, with control points at *x2*,*y2*, and *x3*,*y3*.

### image
```
<image mask="*m*" smooth="*s*" position="*x y*" matrix="*a b c d*">
*imagedata*
</image>
```

__Attributes__

| mask="*m*"         | set the color channel mask to *m*    |
| smooth="*n*"       | If n is set to 1, the renderer will attempt to smooth the bitmap image.    |
| position="*x y*"   | positions the top left of the bitmap at the coordinates given by *x* and *y*. |
| matrix="*a b c d*" | applies a two dimensional transform to the bitmap string. The tranformation matrix is constructed from *a*,*b*,*c* and *d*. |

__Description__

Plots the bitmap image *imagedata*. This must be a valid XML format bitmap.

### imagen
```
<imagen mask="*m*" smooth="*s*" position="*x y*" matrix="*a b c d*">
<name>*imagename*</name>
</imagen>
```

__Attributes__

|-|-|
| mask="*m*"         | set the color channel mask to *m*  |
| smooth="*n*"       | If n is set to 1, the renderer will attempt to smooth the bitmap image.   |
| position="*x y*"   | positions the top left of the bitmap at the coordinates given by *x* and *y*.  |
| matrix="*a b c d*" | applies a two dimensional transform to the bitmap string. The tranformation matrix is constructed from *a*,*b*,*c* and *d*.   |

__Description__

Plots the bitmap image named *imagename*. The Renderer attempts to resolve *imagename* to a known bitmap. This may be translated to a filename or used to search a database of named bitmaps depending on the renderer.

### ellipse
```
<ellipse color="*aarrggbb*" mask="*m*" 
       width="*w*" dash="*phase dash....*" fill="*f*">
<s><d><w>*width*</w><h>*height*</h></d></s>
</ellipse>
```

__Attributes__

|-|-|
| color="*aarrggbb*" | color the ellipse according to the hexadecimal number *aarrggbb*. The alpha value is given by *aa*, the red value by *rr*, the green value by *gg*, and the blue value by *bb*. |
| mask="*m*" | set the color channel mask to *m* |
| width="*w*" | set the line thickness to *w*. A value of zero will produce hairlines. |
| dash="*phase dash...*" | set the dash pattern for the ellipse using the phase value *phase* and the space separated list of dash lengths *dash..* |
| fill="*f*" | fill the ellipse. |

__Description__
Draw an ellipse of width *width* and height *height*.


### rectangle
```
<rectangle color="*aarrggbb*" mask="*m*" 
       width="*w*" dash="*phase* *dash....*" fill="*f*">
<s><d><w>*width*</w><h>*height*</h></d></s>
</rectangle>
```

__Attributes__

|-|-|
| color="*aarrggbb*" | color the rectangle according to the hexadecimal number *aarrggbb*. The alpha value is given by *aa*, the red value by *rr*, the green value by *gg*, and the blue value by *bb*. |
| mask="*m*"  | set the color channel mask to *m* |
| width="*w*" | set the line thickness to *w*. A value of zero will produce hairlines. |
| dash="*phase dash...*" | set the dash pattern for the rectangle using the phase value *phase* and the space separated list of dash lengths *dash..* | 
| fill="*f*" | fill the rectangle. |

__Description__
Draw an rectangle of width *width* and height *height*.

### polygon
```
<polygon color="*aarrggbb*" mask="*m*" 
       width="*w*" dash="*phase* *dash....*" fill="*f*">
<pp><x>*pntx*</x><y>*pnty*</y><t>type</t></pp>
<pp><x>*pntx*</x><y>*pnty*</y><t>type</t></pp>
<pp><x>*pntx*</x><y>*pnty*</y><t>type</t></pp>
.
.
</polygon>
```

__Attributes__

|-|-|
| color="*aarrggbb*" | color the polygon according to the hexadecimal number *aarrggbb*. The alpha value is given by *aa*, the red value by *rr*, the green value by *gg*, and the blue value by *bb*. |
| mask="*m*" | set the color channel mask to *m* |
| width="*w*" | set the line thickness to *w*. A value of zero will produce hairlines. |
| dash="*phase dash...*" | set the dash pattern for the polygon using the phase value *phase* and the space separated list of dash lengths *dash..* |
| fill="*f*" | fill the polygon. |

__Description__
Draw a polygon. The vertices of the polygon are given by the set of sub-tags named <code>pp</code>. Each <code>pp</code> tag contains the X and Y coordinates of the vertex given by *pntx* and *pnty*, and a type code *type*. If the type code is zero (0), then the line linking this vertex to the previous one in straight. If the type code is one (1), then the previous three points make up the start point and control points for a bezier curve linked to this one.

### clip
```
<clip>
<pp><x>*pntx*</x><y>*pnty*</y><t>type</t></pp>
<pp><x>*pntx*</x><y>*pnty*</y><t>type</t></pp>
<pp><x>*pntx*</x><y>*pnty*</y><t>type</t></pp>
.
.
</clip>
```
__Description__
Defines a clipping polygon. The vertices of the polygon are given by the set of sub-tags named <code>pp</code>. Each <code>pp</code> tag contains the X and Y coordinates of the vertex given by *pntx* and *pnty*, and a type code *type*. If the type code is zero (0), then the line linking this vertex to the previous one in straight. If the type code is one (1), then the previous three points make up the start point and control points for a bezier curve linked to this one.

All subsequent operations will be clipped to this polygon. To remove the clipping polygon, use an empty <code>clip</code> tag.

## References

None

## History

2004/11/05  Initial Revision.


