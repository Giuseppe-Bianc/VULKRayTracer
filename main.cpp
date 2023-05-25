#include "headers.h"

using std::runtime_error;

class GLFWError : public runtime_error
{
   public:
    explicit GLFWError(std::string_view message) : runtime_error(message.data()) {}
};
void keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
}
void errorCallback(int error, const char *description)
{
    spdlog::error("GLFW Error {}: {}", error, description);
}
void calcolaFPS(GLFWwindow *finestra)
{
    static double tempoPrecedente = 0.0;
    static int numeroFrame = 0;
    long double tempoCorrente = glfwGetTime();
    long double deltaTempo = tempoCorrente - tempoPrecedente;
    numeroFrame++;
    if (deltaTempo >= 1.0) {
        long double fps = numeroFrame / deltaTempo;
        std::string titoloFinestra = std::format("{} FPS: {}", VKRT::windowTitle, std::to_string(fps));
        glfwSetWindowTitle(finestra, titoloFinestra.c_str());
        tempoPrecedente = tempoCorrente;
        numeroFrame = 0;
    }
}

int main()
{
    spdlog::set_pattern(R"(%^[%T] [%l] %v%$)");
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    if (!glfwInit()) throw GLFWError("Failed to initialize GLFW");
    glfwSetErrorCallback(errorCallback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(VKRT::w, VKRT::h, VKRT::windowTitle.data(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw GLFWError("Failed to create GLFW window");
    }

    glfwSetKeyCallback(window, keyCallback);
    GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
    if (primaryMonitor == nullptr) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw GLFWError("Failed to get primary monitor");
    }
    const GLFWvidmode *videoMode = glfwGetVideoMode(primaryMonitor);
    if (videoMode == nullptr) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw GLFWError("Failed to get video mode");
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
        throw GLFWError("Failed to set window position centered");
    }
    VKINFO("creata finestra glfw {}:(lagezza: {}, altezza: {}, posizione(x:{},y:{}))", VKRT::windowTitle.data(), VKRT::w, VKRT::h, centerX, centerY);
    glfwShowWindow(window);
    auto startTime = std::chrono::high_resolution_clock::now();
    auto previousTime = startTime;
    double frameCount = 0.0;
    while (!glfwWindowShouldClose(window)) {
        calcolaFPS(window);
        // Esegui il rendering con Vulkan (puoi aggiungere il codice Vulkan qui)

        // Eventi GLFW
        glfwPollEvents();
    }

    // Pulizia delle risorse
    glfwDestroyWindow(window);
    glfwTerminate();
    VKINFO("Press enter to exit...");
    std::cin.ignore();
    return 0;
}
