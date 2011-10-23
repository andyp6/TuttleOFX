#ifndef _TUTTLE_PLUGIN_THINNING_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_THINNING_ALGORITHM_HPP_

#include <terry/channel.hpp>
#include <terry/numeric/pixel_numeric_operations.hpp>

namespace tuttle {
namespace plugin {
namespace thinning {

static const bool lutthin1[512] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, false, true, true, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, false, false, true, true, false, false, true, true, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, false, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, false, false, false, true, false, false, true, true, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, true, false, false, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
static const bool lutthin2[512] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, true, true, true, false, false, true, true, false, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, true, true, true, false, false, true, true, false, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, false, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, true, true, true, false, false, true, true, false, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, true, true, true, false, false, true, true, false, true, true, true };

template<class SView, class DView=SView>
struct pixel_locator_thinning_t
{
	typedef typename SView::locator SLocator;
	typedef typename DView::locator DLocator;
	typedef typename SView::value_type SPixel;
	typedef typename DView::value_type DPixel;
	typedef typename SLocator::cached_location_t SCachedLocation;

	const bool* _lut;
	SPixel _sWhite;
	DPixel _dWhite;
	DPixel _dBlack;
	const SLocator _loc_ref;
	// LT CT RT
	// LC    RC
	// LB CB RB
	const SCachedLocation LT;
	const SCachedLocation CT;
	const SCachedLocation RT;
	const SCachedLocation LC;
	const SCachedLocation RC;
	const SCachedLocation LB;
	const SCachedLocation CB;
	const SCachedLocation RB;

	pixel_locator_thinning_t( const SView& src, const bool* lut )
	: _lut(lut)
	, _loc_ref(src.xy_at(0,0))
	, LT(_loc_ref.cache_location(-1,-1))
	, CT(_loc_ref.cache_location( 0,-1))
	, RT(_loc_ref.cache_location( 1,-1))

	, LC(_loc_ref.cache_location(-1, 0))
	, RC(_loc_ref.cache_location( 1, 0))

	, LB(_loc_ref.cache_location(-1, 1))
	, CB(_loc_ref.cache_location( 0, 1))
	, RB(_loc_ref.cache_location( 1, 1))
	{
		using namespace terry;
		pixel_assigns_max( _sWhite );
		pixel_assigns_min( _dBlack );
		pixel_assigns_max( _dWhite );
	}

	DPixel operator()( const SLocator& src ) const
	{
		using namespace terry;

		if( *src != _sWhite )
		{
			return _dBlack;
		}
		
		std::size_t id =  ( src[LT] == _sWhite )       |
			             (( src[LC] == _sWhite ) << 1) |
			             (( src[LB] == _sWhite ) << 2) |
			             (( src[CT] == _sWhite ) << 3) |
			             (( *src    == _sWhite ) << 4) |
			             (( src[CB] == _sWhite ) << 5) |
			             (( src[RT] == _sWhite ) << 6) |
			             (( src[RC] == _sWhite ) << 7) |
			             (( src[RB] == _sWhite ) << 8);
		
		if( _lut[id] )
		{
			return _dWhite;
		}
		return _dBlack;
	}
};



}
}
}

#endif
