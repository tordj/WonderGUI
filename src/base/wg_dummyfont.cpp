#include "wg_dummyfont.h"



namespace wg 
{
	
	
	const char DummyFont::CLASSNAME[] = {"DummyFont"};
	
	//____ Constructor ____________________________________________________________
	
	DummyFont::DummyFont() : m_glyph(this)
	{
	}
	
	
	//____ Destructor _____________________________________________________________
	
	DummyFont::~DummyFont()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool DummyFont::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Font::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * DummyFont::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	DummyFont_p DummyFont::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return DummyFont_p( static_cast<DummyFont*>(pObject.rawPtr()) );
	
		return 0;
	}


	int DummyFont::nbGlyphs()
	{
		return 0;
	}
	
	bool DummyFont::hasGlyphs()
	{
		return false;
	}
	
	bool DummyFont::isMonospace()
	{
		return true;
	}
	
	bool DummyFont::isMonochrome()
	{
		return true;
	}
	
	bool DummyFont::hasGlyph( uint16_t chr )
	{
		return false;
	}

	bool DummyFont::setSize( int size )
	{
		return false;
	}
	
	int DummyFont::size()
	{
		return 1;
	}

	int DummyFont::kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph )
	{
		return 0;
	}
	
	Glyph_p DummyFont::getGlyph( uint16_t chr )
	{
		return &m_glyph;
	}

	int DummyFont::lineGap()
	{
		return 1;
	}
	
	int DummyFont::whitespaceAdvance()
	{
		return 1;
	}
	
	int DummyFont::maxAdvance()
	{
		return 1;
	}
	
	int DummyFont::maxAscend()
	{
		return 1;
	}
	
	int DummyFont::maxDescend()
	{
		return 0;
	}
	
	DummyFont::DummyGlyph::DummyGlyph( Font * pFont )
	: Glyph( 1, 0, pFont )
	{
			m_src.pSurface	= 0;
			m_src.bearingX	= 0;
			m_src.bearingY	= 0;
	}
}
