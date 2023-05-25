#pragma once
#define TINYOBJLOADER_USE_DOUBLE
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#define GLM_FORCE_SILENT_WARNINGS
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_WIN32
#define GLM_FORCE_SIZE_T_LENGTH
#define GLM_FORCE_PURE
#define GLM_FORCE_INLINE
#define GLM_FORCE_EXPLICIT_CTOR
#define GLM_FORCE_CXX20
#define GLM_FORCE_UNRESTRICTED_GENTYPE
#define GLM_FORCE_PRECISION_HIGHP_DOUBLE
#define GLM_FORCE_PRECISION_HIGHP_INT
#define GLM_FORCE_PRECISION_HIGHP_FLOAT
#ifdef _MSC_VER
// Microsoft Visual C++ Compiler
#pragma warning(push)
#pragma warning(disable : 6386 6385)
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <spdlog/cfg/env.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <tiny_obj_loader.h>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <format>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numbers>
#include <optional>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan.hpp>

// Restore warning levels.
#ifdef _MSC_VER
// Microsoft Visual C++ Compiler
#pragma warning(pop)
#endif

[[nodiscard]] static constexpr auto calcolaCentro(const unsigned int width, const unsigned int w) noexcept
{
    return (width - w) / 2;
}

#define VKTRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define VKDEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define VKINFO(...) SPDLOG_INFO(__VA_ARGS__)
#define VKWARN(...) SPDLOG_WARN(__VA_ARGS__)
#define VKERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define VKCRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#define CAST_ST(x) static_cast<std::size_t>((x))
#define CAST_CPCU32T(x) static_cast<const uint32_t *>(static_cast<const void *>((x)))
#define CAST_UC(x) static_cast<unsigned char>((x))
#define CAST_32T(x) static_cast<int32_t>((x))
#define CAST_U32T(x) static_cast<uint32_t>((x))
#define CAST_U8(x) static_cast<Uint8>((x))
#define CAST_I(x) static_cast<int>((x))
#define CAST_ULLI(x) static_cast<long long int>((x))
#define CAST_D(x) static_cast<double>((x))
#define CAST_F(x) static_cast<float>((x))
#define CALC_CENTRO(width, w) calcolaCentro((width), (w))
#define PRINT(p, ...) std::cout << std::fixed << std::setprecision(p) << __VA_ARGS__ << std::endl;
#define PRINTNNL(p, ...) std::cout << std::fixed << std::setprecision(p) << __VA_ARGS__;
#define GLWFERR(error, description) VKERROR("GLFW Error ({}): {}", error, description);
#define VKSYSPAUSE()                                                                                                   \
    VKINFO("Press enter to exit...");                                                                                  \
    std::cin.ignore();
#define PRINTVER(version)                                                                                              \
    VKINFO("System can support vulkan Variant: {}, Major: {}, Minor: {}", VK_API_VERSION_VARIANT(version),             \
           VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version))

using ddvector = std::vector<std::vector<double>>;
namespace VKRT
{
    static inline constexpr bool FWDTF = true;
    static inline constexpr bool BCKTF = false;
    static inline constexpr double MAX_COLOR = 255.0;
    static inline constexpr double NO_COLOR = 0.0;
    static inline constexpr double EPSILON = 1e-22;
    static inline constexpr long NANOD = static_cast<long>(std::nano::den);
    static inline constexpr long double pi = std::numbers::pi_v<long double>;
    // Dimensioni della finestra
    static inline constexpr int w = 1024;
    static inline constexpr double aspect_ratio = 16.0 / 9.0;
    static inline constexpr int samples_per_pixel = 100;
    static inline constexpr int max_depth = 50;
    static inline constexpr int h = 576;
    static inline constexpr int nCh = 4;
    static inline constexpr std::size_t dataSize = CAST_ST(w * h * nCh);
    static inline constexpr double scale = 255.99999;
    static inline constexpr std::string_view windowTitle = "Ray Tracer";
}  // namespace VKRT
