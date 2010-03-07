#ifndef _LIN2LOG_PROCESS_HPP_
#define _LIN2LOG_PROCESS_HPP_

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

/**
 * @brief Lin to log process
 */
template<class View>
class Lin2LogProcess : public ImageGilFilterProcessor<View>
{
protected :
    Lin2LogPlugin& _plugin;        ///< Rendering plugin

public :
    Lin2LogProcess( Lin2LogPlugin &instance );

    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}
}

#include "Lin2LogProcess.tcc"

#endif