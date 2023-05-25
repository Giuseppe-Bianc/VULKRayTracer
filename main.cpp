#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "headers.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
using std::runtime_error;

class TriangleApplicationError : public runtime_error
{
   public:
    explicit TriangleApplicationError(std::string_view message) : runtime_error(message.data()) {}
};
void keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
}
void errorCallback(int error, const char *description)
{
    GLWFERR(error, description);
}

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() { return graphicsFamily.has_value(); }
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication
{
   public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

   private:
    GLFWwindow *window;

    vk::UniqueInstance instance;
    vk::UniqueDebugUtilsMessengerEXT debugUtilsMessenger;

    vk::PhysicalDevice physicalDevice;
    void calcolaFPS()
    {
        static double tempoPrecedente = 0.0;
        static int numeroFrame = 0;
        long double tempoCorrente = glfwGetTime();
        long double deltaTempo = tempoCorrente - tempoPrecedente;
        numeroFrame++;
        if (deltaTempo >= 1.0) {
            long double fps = numeroFrame / deltaTempo;
            std::string titoloFinestra = std::format("{} FPS: {}", VKRT::windowTitle, std::to_string(fps));
            glfwSetWindowTitle(window, titoloFinestra.c_str());
            tempoPrecedente = tempoCorrente;
            numeroFrame = 0;
        }
    }

    void initWindow()
    {
        if (!glfwInit()) throw TriangleApplicationError("Failed to initialize GLFW");
        glfwSetErrorCallback(errorCallback);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        window = glfwCreateWindow(VKRT::w, VKRT::h, VKRT::windowTitle.data(), nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw TriangleApplicationError("Failed to create GLFW window");
        }

        glfwSetKeyCallback(window, keyCallback);
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        if (primaryMonitor == nullptr) {
            glfwDestroyWindow(window);
            glfwTerminate();
            throw TriangleApplicationError("Failed to get primary monitor");
        }
        const GLFWvidmode *videoMode = glfwGetVideoMode(primaryMonitor);
        if (videoMode == nullptr) {
            glfwDestroyWindow(window);
            glfwTerminate();
            throw TriangleApplicationError("Failed to get video mode");
        }

        static const auto centerX = CALC_CENTRO(videoMode->width, VKRT::w);
        static const auto centerY = CALC_CENTRO(videoMode->height, VKRT::h);
        glfwSetWindowPos(window, centerX, centerY);

        int posX = 0;
        int posY = 0;
        glfwGetWindowPos(window, &posX, &posY);
        if (posX != centerX || posY != centerY) {
            glfwDestroyWindow(window);
            glfwTerminate();
            throw TriangleApplicationError("Failed to set window position centered");
        }
        VKINFO("creata finestra glfw {}:(lagezza: {}, altezza: {}, posizione(x:{},y:{}))", VKRT::windowTitle.data(),
               VKRT::w, VKRT::h, centerX, centerY);
        glfwShowWindow(window);
    }

    void initVulkan()
    {
        createInstance();
        setupDebugMessenger();
        pickPhysicalDevice();
    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(window)) {
            calcolaFPS();
            // Esegui il rendering con Vulkan (puoi aggiungere il codice Vulkan qui)

            // Eventi GLFW
            glfwPollEvents();
        }
    }

    void cleanup()
    {
        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance()
    {
        // get the instance independent function pointers
        static vk::DynamicLoader dl;
        auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        vk::ApplicationInfo appInfo(VKRT::windowTitle.data(), VK_MAKE_VERSION(1, 0, 0), "No Engine",
                                    VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_3);

        auto extensions = getRequiredExtensions();

        if (enableValidationLayers) {
            // in debug mode, use the debugUtilsMessengerCallback
            vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

            vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                               vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                               vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

            vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> createInfo(
                {{}, &appInfo, validationLayers, extensions},
                {{}, severityFlags, messageTypeFlags, &debugUtilsMessengerCallback});
            instance = vk::createInstanceUnique(createInfo.get<vk::InstanceCreateInfo>());
        } else {
            // in non-debug mode
            vk::InstanceCreateInfo createInfo({}, &appInfo, {}, extensions);
            instance = vk::createInstanceUnique(createInfo, nullptr);
        }

        // get all the other function pointers
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);
    }

    void setupDebugMessenger()
    {
        if (!enableValidationLayers) return;

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        debugUtilsMessenger = instance->createDebugUtilsMessengerEXTUnique(
            vk::DebugUtilsMessengerCreateInfoEXT({}, severityFlags, messageTypeFlags, &debugUtilsMessengerCallback));
    }

    void pickPhysicalDevice()
    {
        std::vector<vk::PhysicalDevice> devices = instance->enumeratePhysicalDevices();

        for (const auto &device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (!physicalDevice) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool isDeviceSuitable(vk::PhysicalDevice device)
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        return indices.isComplete();
    }

    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device)
    {
        QueueFamilyIndices indices;

        std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    std::vector<const char *> getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport()
    {
        std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

        for (const char *layerName : validationLayers) {
            bool layerFound = false;
            for (const auto &layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                return false;
            }
        }
        return true;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData, void *pUserData)
    {
        VKERROR("validation layer: {}", pCallbackData->pMessage);

        return VK_FALSE;
    }
};

int main()
{
    spdlog::set_pattern(R"(%^[%T] [%l] %v%$)");
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    VKSYSPAUSE();
    return EXIT_SUCCESS;
}
