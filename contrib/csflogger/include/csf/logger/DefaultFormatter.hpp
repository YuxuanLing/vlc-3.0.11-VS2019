#pragma once

#include "csf/logger/CSFLog.h"
#include "csf/logger/Formatter.hpp"
#include "csf/logger/SmartPointer.hpp"

namespace CSF
{
    namespace csflogger
    {
        class CSF_LOGGER_API DefaultFormatter : public Formatter
        {
        public:
            DefaultFormatter();
            virtual ~DefaultFormatter();

            virtual std::string formatMessage(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage);
            virtual void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn);

        private:
            class Impl;
            SMART_PTR_NS::shared_ptr<Impl> pimpl;
        };

    }
}

