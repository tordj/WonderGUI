#include <iostream>

#include <wg_types.h>


class KernelDB
{
public:
	KernelDB();
	~KernelDB();

	bool	generateSource( std::ostream& filename );


private:

	void	_resetDB();

	bool	m_tintModes[wg::TintMode_size];
	bool	m_blendModes[wg::BlendMode_size];
	bool	m_srcFormats[wg::PixelFormat_size];
	bool	m_destFormats[wg::PixelFormat_size];


	// TintMode, BlendMode, SourceFormat, DestFormat, bTile

	bool	m_simpleBlitMap[wg::TintMode_size][wg::BlendMode_size][wg::PixelFormat_size][wg::PixelFormat_size][2];

	// TintMode, BlendMode, SourceFormat, DestFormat, bTile, Normal/Clip/Tile

	bool	m_complexBlitMap[wg::TintMode_size][wg::BlendMode_size][wg::PixelFormat_size][wg::PixelFormat_size][2][3];


};