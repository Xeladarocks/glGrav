void centerWindow(GLFWwindow* window, GLFWmonitor* monitor) {
    if (!monitor)
        return;

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode)
        return;

    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    glfwSetWindowPos(window,
        monitorX + (mode->width - windowWidth) / 2,
        monitorY + (mode->height - windowHeight) / 2);
}

#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#define MIN(a, b) (((b) < (a)) ? (b) : (a))
GLFWmonitor* getBestMonitor(GLFWwindow* window) {
    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

    if (!monitors)
        return NULL;

    int windowX, windowY, windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetWindowPos(window, &windowX, &windowY);

    GLFWmonitor* bestMonitor = NULL;
    int bestArea = 0;

    for (int i = 0; i < monitorCount; ++i)
    {
        GLFWmonitor* monitor = monitors[i];

        int monitorX, monitorY;
        glfwGetMonitorPos(monitor, &monitorX, &monitorY);

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (!mode)
            continue;

        int areaMinX = MAX(windowX, monitorX);
        int areaMinY = MAX(windowY, monitorY);

        int areaMaxX = MIN(windowX + windowWidth, monitorX + mode->width);
        int areaMaxY = MIN(windowY + windowHeight, monitorY + mode->height);

        int area = (areaMaxX - areaMinX) * (areaMaxY - areaMinY);

        if (area > bestArea)
        {
            bestArea = area;
            bestMonitor = monitor;
        }
    }

    return bestMonitor;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // resize callback
    glViewport(0, 0, width, height);
}

int glSetup(const unsigned int& WIDTH, const unsigned int& HEIGHT,  GLFWwindow*& window, GLFWmonitor*& monitor) {
    /** SET-UP **/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, (int)4.5);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, (int)4.5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment for Apple OS X support

    window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    monitor = getBestMonitor(window);
    centerWindow(window, monitor);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, processInput);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSwapInterval(0);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /** ------ **/

    return 0;
}