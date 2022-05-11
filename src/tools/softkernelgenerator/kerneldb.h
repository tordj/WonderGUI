#include <iostream>
#include <vector>

#include <wg_types.h>


class KernelDB
{
public:
	KernelDB();
	~KernelDB();

	struct StraightBlitSpec
	{
		StraightBlitSpec();

		bool		tintModes[wg::TintMode_size];
		bool		blendModes[wg::BlendMode_size];
		bool		sourceFormats[wg::PixelFormat_size];
		bool		destFormats[wg::PixelFormat_size];
		bool		blitAndTile[2];
	};

	struct TransformBlitSpec
	{
		TransformBlitSpec();

		bool			tintModes[wg::TintMode_size];
		bool			blendModes[wg::BlendMode_size];
		bool			sourceFormats[wg::PixelFormat_size];
		bool			destFormats[wg::PixelFormat_size];
		bool			sampleMethods[wg::SampleMethod_size];
		bool			blitClipAndTile[3];
	};

	struct KernelCount
	{
		int	plot = 0;
		int fill = 0;
		int line = 0;
		int clipLine = 0;
		int plotList = 0;
		int segment = 0;

		int	pass1blits = 0;
		int pass1blits_fast8 = 0;

		int pass2blits = 0;
		int pass2blits_fast8 = 0;

		int	optimizedStraightBlits = 0;
		int optimizedTransformBlits = 0;
	};


	void	reset();
	void	clear();

	bool	generateSource( std::ostream& filename );
	KernelCount	countKernels();

	void	setTintMode(wg::TintMode mode, bool bOn);
	void	setBlendMode(wg::BlendMode mode, bool bOn);
	void	setSrcFormat(wg::PixelFormat format, bool bOn);
	void	setDestFormat(wg::PixelFormat format, bool bOn);

	bool	tintMode(wg::TintMode mode) { return m_tintModes[int(mode)]; }
	bool	blendMode(wg::BlendMode mode) { return m_blendModes[int(mode)]; }
	bool	srcFormat(wg::PixelFormat format) { return m_srcFormats[int(format)]; }
	bool	destFormat(wg::PixelFormat format) { return m_destFormats[int(format)]; }


private:

	bool	m_tintModes[wg::TintMode_size];
	bool	m_blendModes[wg::BlendMode_size];
	bool	m_srcFormats[wg::PixelFormat_size];
	bool	m_destFormats[wg::PixelFormat_size];

	std::vector<StraightBlitSpec>	m_straightBlits;
	std::vector<TransformBlitSpec>	m_transformBlits;



/*
	// TintMode, BlendMode, SourceFormat, DestFormat, bTile

	bool	m_simpleBlitMap[wg::TintMode_size][wg::BlendMode_size][wg::PixelFormat_size][wg::PixelFormat_size][2];

	// TintMode, BlendMode, SourceFormat, DestFormat, SampleMethod, Normal/Clip/Tile

	bool	m_complexBlitMap[wg::TintMode_size][wg::BlendMode_size][wg::PixelFormat_size][wg::PixelFormat_size][2][3];
*/


};