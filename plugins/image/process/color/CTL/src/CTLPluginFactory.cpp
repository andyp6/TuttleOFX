#include "CTLPluginFactory.hpp"
#include "CTLPlugin.hpp"
#include "CTLDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle {
namespace plugin {
namespace ctl {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void CTLPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "CTL", "CTL",
		            "ColorTransformationLanguage" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	desc.setDescription(
		"Color Transformation Language"
		"\n\n"
		"The Color Transformation Language, or CTL, is a small programming language that has been designed to serve as a building "
		"block for digital color management systems. CTL allows users to describe color transforms in a concise and unambiguous way "
		"by expressing them as programs."
		"\n"
		"Color transforms can be shared by distributing CTL programs. Two parties with the same CTL program can apply the same "
		"transform to an image. "
		"In addition to the original image, a CTL program can have input parameters whose settings affect how the input image will be "
		"transformed. For example, a transform may have an 'exposure' parameter, such that changing the exposure makes the image "
		"brighter or darker. In order to guarantee identical results, parties that have agreed to use a particular transform must also agree "
		"on the settings for the transform's parameters.\n"
		"A domain-specific programming language such as CTL can be designed to allow only the kinds of operations that are needed to"
		"describe color transforms. This improves the portability of programs, protects users against application software crashes and"
		"malicious code, and permits efficient interpreter implementations."
		"\n" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void CTLPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* chooseInput = desc.defineChoiceParam( kParamChooseInput );
	chooseInput->appendOption( kParamChooseInputCode );
	chooseInput->appendOption( kParamChooseInputFile );

	OFX::StringParamDescriptor* code = desc.defineStringParam( kParamCTLCode );
	code->setLabel( "CTL code" );
	code->setHint( "Your CTL code." );
	code->setStringType( OFX::eStringTypeMultiLine );

	OFX::StringParamDescriptor* file = desc.defineStringParam( kParamCTLFile );
	file->setLabel( "CTL file" );
	file->setHint( "CTL source code file." );
	file->setStringType( OFX::eStringTypeFilePath );
	

}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* CTLPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new CTLPlugin( handle );
}

}
}
}
