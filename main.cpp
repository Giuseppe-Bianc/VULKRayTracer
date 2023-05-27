#include <vulkan/vulkan.hpp>
#include "headers.h"

using std::runtime_error;

class TriangleApplicationError : public runtime_error
{
   public:
    explicit TriangleApplicationError(std::string_view message) : runtime_error(message.data()) {}
};
void keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            break;
        // Aggiungi altri casi qui se necessario
        default:
            // Caso di default per gestire altre chiavi
            break;
    }
}

void errorCallback(int error, const char *description)
{
    GLWFERR(error, description);
}

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

    void initVulkan() { createInstance(); }

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
        auto appInfo = vk::ApplicationInfo(VKRT::windowTitle.data(), VK_MAKE_VERSION(1, 0, 0), "No Engine",
                                           VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_3);

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        auto createInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo, 0, nullptr,  // enabled layers
                                                 glfwExtensionCount, glfwExtensions                // enabled extensions
        );

        try {
            instance = vk::createInstanceUnique(createInfo, nullptr);
        } catch (vk::SystemError err) {
            throw TriangleApplicationError("failed to create instance!");
        }

        VKINFO("available extensions:");

        for (const auto &extension : vk::enumerateInstanceExtensionProperties()) {
            VKINFO("\t{}", extension.extensionName);
        }
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
