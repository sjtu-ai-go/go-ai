//
// Created by lz on 10/13/16.
//

#ifndef GO_AI_LOGGER_HPP
#define GO_AI_LOGGER_HPP
#include <memory>
#include <spdlog/spdlog.h>

struct LoggerSingleton
{
private:
    static std::shared_ptr<spdlog::logger> logger;
public:
    static auto get() -> std::shared_ptr<spdlog::logger>
    {
        return logger;
    }
};

inline std::shared_ptr<spdlog::logger> getGlobalLogger()
{
    return LoggerSingleton::get();
}


#endif //GO_AI_LOGGER_HPP
