
General
-------
- Add methods drawHorrLine() and drawVertLine() (no-clip versions).
- Better, combined lock methods.
- Surface creation hints.
- Surface conversion with correct upscaling of RGBA components.


Software specific
-----------------
- plotPixels()/clipPlotPixels() support blendmodes.
- _drawLineSegments()/_clipDrawLineSegments() support blendmodes.

- Optimizations
	- Invert with white can be replaced with XOR
	- Multiply with 0 can be replaced with clear.
	- Add white can be replaced with replace white.
	- Sub white can be replaced with replace black.


OpenGL specific
---------------
- Lock region ska kunna mappa och uppdatera endast regionen.
- Use surface creation hints to not make a PixelBufferObject or just save alpha.



Utbyggnad
---------
- Support for Index8 surfaces.
- Support for Index8_A8 surfaces.
- Support for A8 surfaces.




