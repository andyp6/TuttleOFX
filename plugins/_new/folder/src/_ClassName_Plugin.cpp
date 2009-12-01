/**
 * @file %CLASSNAME%Plugin.cpp
 * @brief
 * @author
 * @date    %DATE%
 *
 */

#include "%CLASSNAME%Plugin.hpp"
#include "%CLASSNAME%Process.hpp"
#include "%CLASSNAME%Definitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace % NAMESPACE % {
	namespace plugin {

	using namespace boost::gil;

	% CLASSNAME % Plugin::% CLASSNAME % Plugin( OfxImageEffectHandle handle )
		: ImageEffect( handle )
	{
		_srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
		_dstClip = fetchClip( kOfxImageEffectOutputClipName );
	}

	OFX::Clip* % CLASSNAME % Plugin::getSrcClip() const
	{
		return _srcClip;
	}

	OFX::Clip* % CLASSNAME % Plugin::getDstClip() const
	{
		return _dstClip;
	}

	/**
	 * @brief The overridden render function
	 * @param[in]   args     Rendering parameters
	 */
	void % CLASSNAME % Plugin::render( const OFX::RenderArguments & args )
	{
		// instantiate the render code based on the pixel depth of the dst clip
		OFX::BitDepthEnum dstBitDepth         = _dstClip->getPixelDepth();
		OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents();

		// do the rendering
		if( dstComponents == OFX::ePixelComponentRGBA )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					% CLASSNAME % Process<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					% CLASSNAME % Process<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					% CLASSNAME % Process<rgba32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthNone:
					COUT_FATALERROR( "BitDepthNone not recognize." );
					return;
				case OFX::eBitDepthCustom:
					COUT_FATALERROR( "BitDepthCustom not recognize." );
					return;
			}
		}
		else if( dstComponents == OFX::ePixelComponentAlpha )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					% CLASSNAME % Process<gray8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					% CLASSNAME % Process<gray16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					% CLASSNAME % Process<gray32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthNone:
					COUT_FATALERROR( "BitDepthNone not recognize." );
					return;
				case OFX::eBitDepthCustom:
					COUT_FATALERROR( "BitDepthCustom not recognize." );
					return;
			}
		}
	}

	void % CLASSNAME % Plugin::changedParam( const OFX::InstanceChangedArgs & args, const std::string & paramName )
	{
		if( paramName == kMergeHelpButton )
		{
			sendMessage( OFX::Message::eMessageMessage,
			             "", // No XML resources
			             k % CLASSNAME % HelpString );
		}
	}

	}
}
