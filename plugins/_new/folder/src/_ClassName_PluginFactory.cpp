/**
 * @file %CLASSNAME%PluginFactory.cpp
 * @brief
 * @author
 * @date    %DATE%
 *
 */

#include "%CLASSNAME%Plugin.hpp"
#include "%CLASSNAME%Definitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace % NAMESPACE % {
	namespace plugin {

	static const bool kSupportTiles              = false;
	static const bool kSupportTemporalClipAccess = false;

	mDeclarePluginFactory( % CLASSNAME % PluginFactory, {}, {}
	                       );

	/**
	 * @brief Function called to describe the plugin main features.
	 * @param[in, out]   desc     Effect descriptor
	 */
	using namespace OFX;
	void
	% CLASSNAME % PluginFactory::describe( OFX::ImageEffectDescriptor & desc )
	{
		// basic labels
		desc.setLabels( "%PLUGIN_LABEL%", "%PLUGIN_LABEL%",
		                "%PLUGIN_LONG_LABEL%" );
		desc.setPluginGrouping( "%PLUGIN_GROUP%" );

		// add the supported contexts, only filter at the moment
		//    desc.addSupportedContext(eContextGenerator);
		desc.addSupportedContext( eContextGeneral );

		// add supported pixel depths
		desc.addSupportedBitDepth( eBitDepthUByte );
		desc.addSupportedBitDepth( eBitDepthUShort );
		desc.addSupportedBitDepth( eBitDepthFloat );

		// set a few flags
		desc.setSingleInstance( false );
		desc.setHostFrameThreading( false );
		desc.setSupportsMultiResolution( false );
		desc.setSupportsTiles( kSupportTiles );
		desc.setTemporalClipAccess( kSupportTemporalClipAccess );
		desc.setRenderTwiceAlways( false );
		desc.setSupportsMultipleClipPARs( false );
	}

	/**
	 * @brief Function called to describe the plugin controls and features.
	 * @param[in, out]   desc       Effect descriptor
	 * @param[in]        context    Application context
	 */
	void
	% CLASSNAME % PluginFactory::describeInContext( OFX::ImageEffectDescriptor & desc,
	                                                OFX::ContextEnum context )
	{
		OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

		srcClip->addSupportedComponent( ePixelComponentRGBA );
		srcClip->addSupportedComponent( ePixelComponentAlpha );
		srcClip->setSupportsTiles( kSupportTiles );

		// Create the mandated output clip
		OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
		dstClip->addSupportedComponent( ePixelComponentRGBA );
		dstClip->addSupportedComponent( ePixelComponentAlpha );
		dstClip->setSupportsTiles( kSupportTiles );

		OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( k % CLASSNAME % HelpButton );
		helpButton->setScriptName( "&Help" );
	}

	/**
	 * @brief Function called to create a plugin effect instance
	 * @param[in] handle  effect handle
	 * @param[in] context    Application context
	 * @return  plugin instance
	 */
	OFX::ImageEffect*
	% CLASSNAME % PluginFactory::createInstance( OfxImageEffectHandle handle,
	                                             OFX::ContextEnum context )
	{
		return new % CLASSNAME % Plugin( handle );
	}

	}
}

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static % NAMESPACE % ::plugin::% CLASSNAME % PluginFactory p( "fr.%COMPANY_NAME%.%PROJECT_NAME%.%L_CLASSNAME%", 1, 0 );
	ids.push_back( &p );
}

}
}
