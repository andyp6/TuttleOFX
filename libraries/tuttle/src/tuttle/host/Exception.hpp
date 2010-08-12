#ifndef _TUTTLE_HOST_CORE_EXCEPTION_HPP
#define _TUTTLE_HOST_CORE_EXCEPTION_HPP

#include <tuttle/host/ofx/OfxhUtilities.hpp>

#include <ofxCore.h>

#include <boost/throw_exception.hpp>
#include <boost/exception/exception.hpp>
//#include <boost/exception.hpp>

#include <sstream>
#include <stdexcept>

namespace tuttle {
namespace host {
namespace exception {

class LogicError : public std::logic_error, virtual public boost::exception
{
protected:
	OfxStatus _status;

public:
	explicit LogicError( const std::string& msg = "" )
	: boost::exception(),
	std::logic_error( msg ),
	_status( kOfxStatErrUnknown )
	{}

	explicit LogicError( const OfxStatus& status, const std::string& msg = "" )
	: boost::exception(),
	std::logic_error( ofx::mapStatusToString( status ) + " : " + msg ),
	_status( status )
	{}

	LogicError( const LogicError& other )
	: boost::exception(),
	std::logic_error( other ),
	_status( other._status )
	{}

	virtual ~LogicError() throw() {}

	const OfxStatus   ofxStatus() const    { return _status; }
	const std::string ofxStatusStr() const { return ofx::mapStatusToString( _status ); }
};

}
}
}

#endif