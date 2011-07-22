#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerProcess.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/algorithm.hpp>
#include <boost/smart_ptr/scoped_array.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

/*
 * fecth plugin IHM parameters with plugin parameters pointer
 */
HistogramKeyerPlugin::HistogramKeyerPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramGlobalDisplaySelection = fetchBooleanParam( kGlobalDisplay ); //global display

	_paramColorRGBSelection = fetchParametricParam( kParamRGBColorSelection ); //curve RGB
	_paramOverlayRSelection = fetchBooleanParam( kBoolRed ); //R (is channel selected?)
	_paramOverlayGSelection = fetchBooleanParam( kBoolGreen ); //G (is channel selected?)
	_paramOverlayBSelection = fetchBooleanParam( kBoolBlue ); //B (is channel selected?)
	_resetRGB = fetchPushButtonParam( kButtonResetRGB ); //reset RGB button

	_paramColorHSLSelection = fetchParametricParam( kParamHSLColorSelection ); //curve HSL
	_paramOverlayHSelection = fetchBooleanParam( kBoolHue ); //H (is channel selected?)
	_paramOverlaySSelection = fetchBooleanParam( kBoolSaturation ); //S (is channel selected?)
	_paramOverlayLSelection = fetchBooleanParam( kBoolLightness ); //L (is channel selected?)
	_resetHSL = fetchPushButtonParam( kButtonResetHSL ); //Reset HSL button

	_paramDisplayTypeSelection = fetchChoiceParam( kHistoDisplayListParamLabel ); //histogram display list (Histogram overlay group)
	_paramResetAll = fetchPushButtonParam( kButtonResetAll ); //Reset all button (Histogram overlay group)

	_paramDisplaySelection = fetchBooleanParam( kBoolSelection ); //display selection on source clip (Selection group)
	_paramSelectionMode = fetchChoiceParam( kSelectionModeListParamLabel ); //selection mode (Selection group)

	_paramNbStepSelection = fetchIntParam( knbStepRange ); //nb step range (Advanced group)
	
	_paramSelectionFromCurve = fetchIntParam( kprecisionCurveFromSelection ); //precision of selection to curve (selection group)
	
	_paramSelectionMultiplierSelection = fetchDoubleParam( kselectionMultiplier ); //selection multiplier (Advanced group)
	_paramRefreshOverlaySelection = fetchPushButtonParam( kButtonRefreshOverlay ); //refresh overlay (Advanced group)

	_paramOutputSettingSelection = fetchChoiceParam( kOutputListParamLabel ); //output type (BW/alpha)
	_paramReverseMaskSelection = fetchBooleanParam( kBoolReverseMask ); //reverse mask

	//Reset param booleans
	_isCleaned = false;
	_isNbStepChanged = false;
	_isHistogramRefresh = false;

	//Initialize scoped pointer
	_overlayDataCount = 0;

	/// @todo: HACK to display curves default position
	OFX::InstanceChangedArgs changed( this->timeLineGetTime( ) );
	changedParam( changed, kButtonResetAll );
	changedClip( changed, kOfxImageEffectSimpleSourceClipName );
}

/*
 * create and return HistogramKeyerProcessParams structure for process
 */
HistogramKeyerProcessParams<HistogramKeyerPlugin::Scalar> HistogramKeyerPlugin::getProcessParams( const OfxTime time, const OfxPointD& renderScale ) const
{
	HistogramKeyerProcessParams<Scalar> params;

	params._paramColorRGB = _paramColorRGBSelection; //curve RGB
	params._boolRGB[0] = _paramOverlayRSelection; //R (is channel selected?)
	params._boolRGB[1] = _paramOverlayGSelection; //G (is channel selected?)
	params._boolRGB[2] = _paramOverlayBSelection; //B (is channel selected?)

	params._paramColorHSL = _paramColorHSLSelection; //curve HSL
	params._boolHSL[0] = _paramOverlayHSelection; //H (is channel selected?)
	params._boolHSL[1] = _paramOverlaySSelection; //S (is channel selected?)
	params._boolHSL[2] = _paramOverlayLSelection; //L (is channel selected?)

	params._paramOutputSetting = _paramOutputSettingSelection; //output selection (alpha channel or BW)
	params._boolReverseMask = _paramReverseMaskSelection; //reverse mask check box

	return params;
}

/*
 * a plugin parameter as been changed
 */
void HistogramKeyerPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{

	/*Clean buttons*/
	if( paramName == kButtonResetRGB || paramName == kButtonResetHSL || paramName == kButtonResetAll ) //HSL or RGB or both
	{
		//get nb points for each curve (RGB)
		std::vector<std::size_t> nbControlPointsRGB( nbCurvesRGB ); //initialize vector
		for( std::size_t i = 0; i < nbCurvesRGB; ++i ) //fill up vector
		{
			nbControlPointsRGB[i] = _paramColorRGBSelection->getNControlPoints( i, args.time );
		}

		//get nb points for each curve (HSL)
		std::vector<std::size_t> nbControlPointsHSL( nbCurvesHSL ); //initialize vector
		for( std::size_t i = 0; i < nbCurvesHSL; ++i ) //fill up vector
		{
			nbControlPointsHSL[i] = _paramColorHSLSelection->getNControlPoints( i, args.time );
		}

		//reset RGB curves
		if( paramName == kButtonResetRGB || paramName == kButtonResetAll )//RGB or Clean all
		{
			for( std::size_t channel = 0; channel < nbCurvesRGB; ++channel )
			{
				if(		paramName == kButtonResetAll ||									// if button is Reset all curves
						(channel == 0 && _paramOverlayRSelection->getValue()) ||		// else if button is Reset RGB and Red is selected
						(channel == 1 && _paramOverlayGSelection->getValue()) ||		// else if button is Reset RGB and Green is selected
						(channel ==	2 && _paramOverlayBSelection->getValue()) )			// else if button is Reset RGB and Blue is selected
				{
					_paramColorRGBSelection->deleteControlPoint( channel );
				}
			}
			for( std::size_t i = 0; i < nbCurvesRGB; ++i )//replace default points
			{
				if(		paramName == kButtonResetAll ||									// if button is Reset all curves
						(i == 0 && _paramOverlayRSelection->getValue()) ||		// else if button is Reset RGB and Red is selected
						(i == 1 && _paramOverlayGSelection->getValue()) ||		// else if button is Reset RGB and Green is selected
						(i == 2 && _paramOverlayBSelection->getValue()) )			// else if button is Reset RGB and Blue is selected
				{
					_paramColorRGBSelection->addControlPoint( i, args.time, 0.0, 0.0, false );
					_paramColorRGBSelection->addControlPoint( i, args.time, 0.2, 1.0, false );
					_paramColorRGBSelection->addControlPoint( i, args.time, 0.6, 1.0, false );
					_paramColorRGBSelection->addControlPoint( i, args.time, 1.0, 0.0, false );
				}
			}
		}
		//reset HSL curves
		if( paramName == kButtonResetHSL || paramName == kButtonResetAll )//HSL or Clean all
		{
			for( std::size_t channel = 0; channel < nbCurvesHSL; ++channel )
			{
				if(		paramName == kButtonResetAll ||									// if button is Reset all curves
						(channel == 0 && _paramOverlayHSelection->getValue()) ||		// else if button is Reset HSL and Hue is selected
						(channel == 1 && _paramOverlaySSelection->getValue()) ||		// else if button is Reset HSL and Saturation is selected
						(channel ==	2 && _paramOverlayLSelection->getValue()) )			// else if button is Reset HSL and Lightness is selected
				{
					_paramColorHSLSelection->deleteControlPoint( channel );
				}
			}
			for( std::size_t i = 0; i < nbCurvesHSL; ++i )//replace default points
			{
				if(		paramName == kButtonResetAll ||							// if button is Reset all curves
				(		i == 0 && _paramOverlayHSelection->getValue()) ||		// else if button is Reset HSL and Hue is selected
				(		i == 1 && _paramOverlaySSelection->getValue()) ||		// else if button is Reset HSL and Saturation is selected
				(		i == 2 && _paramOverlayLSelection->getValue()) )		// else if button is Reset HSL and Lightness is selected
				{
					_paramColorHSLSelection->addControlPoint( i, args.time, 0.0, 0.0, false );
					_paramColorHSLSelection->addControlPoint( i, args.time, 0.2, 1.0, false );
					_paramColorHSLSelection->addControlPoint( i, args.time, 0.6, 1.0, false );
					_paramColorHSLSelection->addControlPoint( i, args.time, 1.0, 0.0, false );
				}
			}
		}
		/// @todo How to request a redraw on ParametricParameters?
		//	_paramColorHSLSelection->getProps().propGetPointer( kOfxParamPropParametricInteractBackground );
	}

		/*refresh histogram overlay*/
	else if( paramName == kButtonRefreshOverlay )
	{
		//Draw forced
		OFX::InstanceChangedArgs changed( args.time, args.renderScale );
		this->changedClip( changed, this->_clipSrc->name( ) );
	}
		/*nbStep changed*/
	else if( paramName == knbStepRange )
	{
		if( this->hasOverlayData( ) ) //if there is overlay value
		{
			getOverlayData( ).setNbStep( _paramNbStepSelection->getValue( ) ); //change nbStep value
			getOverlayData( ).computeFullData( this->_clipSrc, args.time, args.renderScale ); //reset buffer and compute them
		}
	}
	/*Clear user selection*/
	else if( paramName == kButtonResetSelection )
	{
		if( this->hasOverlayData( ) )//if there is overlay value
		{
			this->getOverlayData( ).clearSelection( ); //clear selection
		}
	}
	/*Selection to curve */
	else if(paramName == kButtonSelectionToCurveHSL || paramName == kButtonSelectionToCurveRGB)
	{
		//prepare buffer 
		getOverlayData()._vNbStepCurveFromSelection = _paramSelectionFromCurve->getValue();	//change precision step
		getOverlayData().computeCurveFromSelectionData( this->_clipSrc,args.time, args.renderScale);

		//getOverlayData().resetCurvesFromSelectionData();	//reset curve from selection buffer data
		//RGB
		if(paramName == kButtonSelectionToCurveRGB)
		{			
			//get nb points for each curve (RGB)
			std::vector<std::size_t> nbControlPointsRGB( nbCurvesRGB ); //initialize vector
			for( std::size_t i = 0; i < nbCurvesRGB; ++i ) //fill up vector
			{
				nbControlPointsRGB[i] = _paramColorRGBSelection->getNControlPoints( i, args.time );
			}
			
			//Delete all points on selected RGB curves
			for( std::size_t channel = 0; channel < nbCurvesRGB; ++channel )
			{
				if(		paramName == kButtonResetAll ||									// if button is Reset all curves
						(channel == 0 && _paramOverlayRSelection->getValue()) ||		// else if button is Reset RGB and Red is selected
						(channel == 1 && _paramOverlayGSelection->getValue()) ||		// else if button is Reset RGB and Green is selected
						(channel ==	2 && _paramOverlayBSelection->getValue()) )			// else if button is Reset RGB and Blue is selected
				{
					_paramColorRGBSelection->deleteControlPoint( channel );
				}
			}
			//For all selected channel create new curves from selection
			
			for( std::size_t i = 0; i < nbCurvesRGB; ++i )
			{
				if(		paramName == kButtonResetAll ||							// if button is Reset all curves
						(i == 0 && _paramOverlayRSelection->getValue()) ||		// else if button is Reset RGB and Red is selected
						(i == 1 && _paramOverlayGSelection->getValue()) ||		// else if button is Reset RGB and Green is selected
						(i == 2 && _paramOverlayBSelection->getValue()) )		// else if button is Reset RGB and Blue is selected
				{
					//getMax of current selection buffer
					double maxChannel; //will contain max of the current channel (RGB)
					if(i == 0)	//working on Red channel
						maxChannel = *(std::max_element(getOverlayData()._curveFromSelection._bufferRed.begin(),getOverlayData()._curveFromSelection._bufferRed.end()));
					else if(i == 1)//working on Green channel
						maxChannel = *(std::max_element(getOverlayData()._curveFromSelection._bufferGreen.begin(),getOverlayData()._curveFromSelection._bufferGreen.end()));
					else //working on Blue channel
						maxChannel = *(std::max_element(getOverlayData()._curveFromSelection._bufferBlue.begin(),getOverlayData()._curveFromSelection._bufferBlue.end()));
					
					//Add points to current channel
					double xPosition = 0.0;
					double yPosition = 0.0;
					double step =  (double)(1/(double)(getOverlayData()._vNbStepCurveFromSelection-1));	//compute step for curve display
					
					//for each point
					for(unsigned int x=0; x<getOverlayData()._vNbStepCurveFromSelection; ++x)
					{
						if(i == 0)	//working on Red channel
							yPosition = (double)((double)(getOverlayData()._curveFromSelection._bufferRed[x]));		//get current Y if Red selection buffer
						else if(i == 1)	//working on Green channel
							yPosition = (double)((double)(getOverlayData()._curveFromSelection._bufferGreen[x]));		//get current Y if Green selection buffer
						else	//working on Blue channel
							yPosition = (double)((double)(getOverlayData()._curveFromSelection._bufferBlue[x]));		//get current Y if Blue selection buffer
						
						yPosition/=maxChannel; //yPosition between 0 and 1
						
						if(x==0 || x == getOverlayData()._vNbStepCurveFromSelection-1)	//current point is last of first
							_paramColorRGBSelection->addControlPoint( i, args.time,xPosition, yPosition, false );	//place the current point
						else //filter 
						{
							double value;
							double nextValue;
							double previousValue;
							if(i == 0)//Red channel
							{
								value= (double)getOverlayData()._curveFromSelection._bufferRed[x];				//get actual value
								nextValue = (double)getOverlayData()._curveFromSelection._bufferRed[x+1];		//get next value
								previousValue = (double)getOverlayData()._curveFromSelection._bufferRed[x-1];	//get previous value
							}
							else if(i == 1)//Green channel
							{
								value= (double)getOverlayData()._curveFromSelection._bufferGreen[x];			//get actual value
								nextValue = (double)getOverlayData()._curveFromSelection._bufferGreen[x+1];		//get next value
								previousValue = (double)getOverlayData()._curveFromSelection._bufferGreen[x-1];	//get previous value
							}
							else //Blue channel
							{
								value= (double)getOverlayData()._curveFromSelection._bufferBlue[x];				//get actual value
								nextValue = (double)getOverlayData()._curveFromSelection._bufferBlue[x+1];		//get next value
								previousValue = (double)getOverlayData()._curveFromSelection._bufferBlue[x-1];	//get previous value
							}
							
							value/=maxChannel;				//set value between 0 and 1
							nextValue/=maxChannel;			//set next value between 0 and 1
							previousValue/=maxChannel;		//set previous value between 0 and 1
							
							double betweenNextAndValue = (double)(nextValue-value);			//get interval between value and next  (0<= interval <=1)
							double betweenPreviousAndValue = (double)(value-previousValue);	//get interval between value and previous (0<= interval <=1)
							//get absolute values
							if(betweenNextAndValue<0)		//change betweenNextAndValue negative to positive
								betweenNextAndValue*=-1;
							if(betweenPreviousAndValue<0)	//change betweenPreviousAndValue negative to positive
								betweenPreviousAndValue*=-1;
							
							if(betweenNextAndValue> 0.01 || betweenPreviousAndValue > 0.01)
								_paramColorRGBSelection->addControlPoint(i,args.time,xPosition,yPosition,false);
						}
						xPosition += step; //add step to Xposition
					}
					
				}
			}
		}
		
		//HSL
		if(paramName == kButtonSelectionToCurveHSL)
		{
			//get nb points for each curve (RGB)
			std::vector<std::size_t> nbControlPointsHSL( nbCurvesHSL ); //initialize vector
			for( std::size_t i = 0; i < nbCurvesHSL; ++i ) //fill up vector
			{
				nbControlPointsHSL[i] = _paramColorHSLSelection->getNControlPoints( i, args.time );
			}
			
			//Delete all points on selected RGB curves
			for( std::size_t channel = 0; channel < nbCurvesHSL; ++channel )
			{
				if(		paramName == kButtonResetAll ||									// if button is Reset all curves
						(channel == 0 && _paramOverlayHSelection->getValue()) ||		// else if button is Reset HSL and Hue is selected
						(channel == 1 && _paramOverlaySSelection->getValue()) ||		// else if button is Reset HSL and Saturation is selected
						(channel ==	2 && _paramOverlayLSelection->getValue()) )			// else if button is Reset HSL and Lightness is selected
				{
					_paramColorHSLSelection->deleteControlPoint( channel );
				}
			}
			//For all selected channel create new curves from selection
			for( std::size_t i = 0; i < nbCurvesHSL; ++i )
			{
				if(		paramName == kButtonResetAll ||							// if button is Reset all curves
						(i == 0 && _paramOverlayHSelection->getValue()) ||		// else if button is Reset HSL and Hue is selected
						(i == 1 && _paramOverlaySSelection->getValue()) ||		// else if button is Reset HSL and Saturation is selected
						(i == 2 && _paramOverlayLSelection->getValue()) )		// else if button is Reset HSL and Lightness is selected
				{
					//getMax of current selection buffer
					double maxChannel; //will contain max of the current channel (HSL)
					if(i == 0)	//working on Hue channel
						maxChannel = *(std::max_element(getOverlayData()._curveFromSelection._bufferHue.begin(),getOverlayData()._curveFromSelection._bufferHue.end()));
					else if(i == 1)//working on Saturation channel
						maxChannel = *(std::max_element(getOverlayData()._curveFromSelection._bufferSaturation.begin(),getOverlayData()._curveFromSelection._bufferSaturation.end()));
					else //working on Lightness channel
						maxChannel = *(std::max_element(getOverlayData()._curveFromSelection._bufferLightness.begin(),getOverlayData()._curveFromSelection._bufferLightness.end()));
					
					//Add points to current channel
					double xPosition = 0.0;
					double yPosition = 0.0;
					double step =  (double)(1/(double)(getOverlayData()._vNbStepCurveFromSelection-1));	//compute step for curve display
					
					//for each point
					for(unsigned int x=0; x<getOverlayData()._vNbStepCurveFromSelection; ++x)
					{
						if(i == 0)	//working on Hue channel
							yPosition = (double)((double)(getOverlayData()._curveFromSelection._bufferHue[x]));		//get current Y if Hue selection buffer
						else if(i == 1)	//working on Saturation channel
							yPosition = (double)((double)(getOverlayData()._curveFromSelection._bufferSaturation[x]));	//get current Y if Saturation selection buffer
						else	//working on Lightness channel
							yPosition = (double)((double)(getOverlayData()._curveFromSelection._bufferLightness[x]));		//get current Y if Lightness selection buffer
						
						yPosition/=maxChannel; //yPosition between 0 and 1
						
						if(x==0 || x == getOverlayData()._vNbStepCurveFromSelection-1)	//current point is last of first
							_paramColorHSLSelection->addControlPoint( i, args.time,xPosition, yPosition, false );	//place the current point
						else //filter 
						{
							double value;
							double nextValue;
							double previousValue;
							if(i == 0)//Hue channel
							{
								value= (double)getOverlayData()._curveFromSelection._bufferHue[x];				//get actual value
								nextValue = (double)getOverlayData()._curveFromSelection._bufferHue[x+1];		//get next value
								previousValue = (double)getOverlayData()._curveFromSelection._bufferHue[x-1];	//get previous value
							}
							else if(i == 1)//Saturation channel
							{
								value= (double)getOverlayData()._curveFromSelection._bufferSaturation[x];				//get actual value
								nextValue = (double)getOverlayData()._curveFromSelection._bufferSaturation[x+1];		//get next value
								previousValue = (double)getOverlayData()._curveFromSelection._bufferSaturation[x-1];	//get previous value
							}
							else //Lightness channel
							{
								value= (double)getOverlayData()._curveFromSelection._bufferLightness[x];			//get actual value
								nextValue = (double)getOverlayData()._curveFromSelection._bufferLightness[x+1];		//get next value
								previousValue = (double)getOverlayData()._curveFromSelection._bufferLightness[x-1];	//get previous value
							}
							
							value/=maxChannel;				//set value between 0 and 1
							nextValue/=maxChannel;			//set next value between 0 and 1
							previousValue/=maxChannel;		//set previous value between 0 and 1
							
							double betweenNextAndValue = (double)(nextValue-value);			//get interval between value and next  (0<= interval <=1)
							double betweenPreviousAndValue = (double)(value-previousValue);	//get interval between value and previous (0<= interval <=1)
							//get absolute values
							if(betweenNextAndValue<0)		//change betweenNextAndValue negative to positive
								betweenNextAndValue*=-1;
							if(betweenPreviousAndValue<0)	//change betweenPreviousAndValue negative to positive
								betweenPreviousAndValue*=-1;
							
							if(betweenNextAndValue> 0.01 || betweenPreviousAndValue > 0.01)
								_paramColorHSLSelection->addControlPoint(i,args.time,xPosition,yPosition,false);
						}
						xPosition += step; //add step to Xposition
					}
					
				}
			}
		}
	}
}

/*
 * the source clip has been changed
 */
void HistogramKeyerPlugin::changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName )
{
	if( clipName == kOfxImageEffectSimpleSourceClipName )
	{
		if( this->hasOverlayData( ) )
		{
			this->getOverlayData().clearAll( this->_clipSrc->getPixelRodSize( args.time, args.renderScale ) );
			this->getOverlayData().computeFullData( this->_clipSrc, args.time, args.renderScale );
		}
	}
}

/*
 * does plugin do something
 */
bool HistogramKeyerPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	/// @todo HACK: nuke doesn't call changedClip when the time is modified.
	//OFX::InstanceChangedArgs changed( args.time, args.renderScale );
	//this->changedClip(changed,this->_clipSrc->name());
	//	HistogramKeyerProcessParams<Scalar> params = getProcessParams();
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void HistogramKeyerPlugin::render( const OFX::RenderArguments &args )
{
	if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")	/// @todo: HACK Nuke doesn't call changeClip function when time is changed
	{
		if(getOverlayData().isCurrentTimeModified(args.time)) //if time is changed
		{
			getOverlayData()._currentTime = args.time;
			getOverlayData().computeFullData(this->_clipSrc,args.time,args.renderScale);
		}
	}
	doGilRender<HistogramKeyerProcess > ( *this, args );
}

/// @brief Overlay data
/// @{

/*Overlay data management*/
void HistogramKeyerPlugin::addRefOverlayData( )
{
	if( _overlayDataCount == 0 )
	{
		const OfxPointI imgSize = this->_clipSrc->getPixelRodSize( 0 ); ///@todo set the correct time !
		_overlayData.reset( new OverlayData( imgSize, this->_paramNbStepSelection->getValue( ),this->_paramSelectionFromCurve->getValue()) );
	}
	++_overlayDataCount;
}

void HistogramKeyerPlugin::releaseOverlayData( )
{
	--_overlayDataCount;
	if( _overlayDataCount == 0 )
	{
		_overlayData.reset( NULL );
	}
}

bool HistogramKeyerPlugin::hasOverlayData( ) const
{
	return _overlayDataCount != 0;
}

OverlayData& HistogramKeyerPlugin::getOverlayData( )
{
	return *_overlayData.get( );
}

const OverlayData& HistogramKeyerPlugin::getOverlayData( ) const
{
	return *_overlayData.get( );
}
/// @}
}
}
}
