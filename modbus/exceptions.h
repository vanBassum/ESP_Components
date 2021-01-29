#pragma once

namespace Modbus
{
    enum class Exception
    {
        NoError = 0,
        IlligalFunction = 1,
        IlligalDataAddress = 2,
        IllegalDataValue = 3,
        SlaveDeviceFailure = 4,
        InvalidArguments = 5,
        InvalidReplyLength = 6,
        UnknownException = 7,
        InvalidReplyFunctionCode = 8,
        Timeout = 9,
		MutexFailed = 10,
    };
}

