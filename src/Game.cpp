#define STB_IMAGE_IMPLEMENTATION
#include "headers/stb_image.h"
#include "headers/Game.h" // bu yazim kendi yazdigim kodlar icin

Game::Game()
{
    window = nullptr;
    gameState = GameState::PLAY;
    screenWidth = 800;
    screenHeight = 600;

    player = new Player(100, 100, 50, 60);

    deltaTime = 0.0f;
    lastFrame = SDL_GetTicks();

    lastX = screenWidth / 2;
    lastY = screenHeight / 2;
    firstMouse = true;
    fov = 45.0f;
    yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    pitch = 0.0f;
}

Game::~Game()
{
    delete player;

    SDL_GL_DeleteContext(glContext);

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

void Game::run()
{
    init("MyCraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    gameLoop();
}

void Game::init(const char *title, int x, int y, int w, int h, Uint32 flags)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    // OpenGL için SDL pencere ayarları
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(title, x, y, w, h, flags | SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) // biz GLFW kullanmadigimiz icin "glfwGetProcAddress" degilde SDL ile olani cagiriyoruz
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        exit(-1);
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);
    setupOpenGL();
}

GLuint Game::loadShaders(const char *vertexPath, const char *fragmentPath)
{
    // 1. Shader dosyalarını oku
    ifstream vertexFile(vertexPath);
    ifstream fragmentFile(fragmentPath);
    stringstream vertexStream, fragmentStream;

    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();

    string vertexCode = vertexStream.str();
    string fragmentCode = fragmentStream.str();

    vertexFile.close();
    fragmentFile.close();

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

UVRange Game::getUVRange(int rowSize, int colSize, int targetIndex)
// in my texture atlas height is 48, width is 16. now totalRows means how many texture block we have in texture atlas, and rowIndex means witch block we want to have with the index number of it.
{
    int currRow = targetIndex / colSize;
    int currCol = targetIndex % colSize;

    float uMin = 1.0f / colSize * currCol;
    float uMax = 1.0f / colSize + uMin;
    float vMin = 1.0f / rowSize * currRow;
    float vMax = 1.0f / rowSize + vMin;

    return {uMin, uMax, vMin, vMax};
}

void Game::checkShaderErrors()
{
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex Shader Compilation Failed: " << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment Shader Compilation Failed: " << infoLog << std::endl;
    }
}

void Game::checkErrors()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error: " << error << std::endl;
    }
}

unsigned int Game::loadTexture(const char *path)
{
    stbi_set_flip_vertically_on_load(true);

    unsigned int tempTexture;
    // shaderlerden sonra textureler geliyor biz ilk bunlari OpenGL'e tanitiyoruz
    // 1️⃣ Create a texture object on the GPU
    glGenTextures(1, &tempTexture);

    // 2️⃣ Bind the texture (Tell OpenGL we are working on this texture now)
    glBindTexture(GL_TEXTURE_2D, tempTexture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // bu texturenin x ekseni ve 0-1 disinda olan yerleri nasil render edecegini belirler.
    // in here we say that make the x or y axis GL_REPEAT or any other
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // bu texturenin y ekseni ve 0-1 disinda olan yerleri nasil render edecegini belirler.
    // but if we want to use GL_CLAMP_TO_BORDER we need to deine color and use glTexParameterfv for float array.

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // texture'yi uzaklastirdigimizda. Also here we use mipmaps becouse we scale this down with minifiction
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);               // texture'yi yakinlastirdigimizda. And here we dont have to use mipmaps since they primarily used for when textures get downscaled

    // 3️⃣ Load the image data from file (CPU RAM)
    int width, height, nrChannels; // in rbChanneles there can be RGBA and RGB only so it depends
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    // 4️⃣ Transfer the image data to GPU memory
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB; // 4 kanal varsa RGBA, yoksa RGB
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // 🔹 After this, the image is stored inside the GPU, and the CPU doesn’t need it anymore.
    // 🔹 You can now delete the original data from RAM because the GPU has its own copy.
    stbi_image_free(data);

    return tempTexture;
}

void Game::mouseCallback(SDL_Window *window, double xRel, double yRel)
{
    float sensitivity = 0.1f;
    float xOffset = xRel * sensitivity;
    float yOffset = yRel * sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void Game::scrollCallback(SDL_Window *window, double xOffset, double yOffset)
{
    fov -= (float)yOffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Game::setupOpenGL()
{
    glViewport(0, 0, screenWidth, screenHeight);
    // Üçgenin köşe verileri (x, y, z)
    // Vertex ve UV koordinatları (u, v)
    float u0 = 0.0f;
    float u1 = 1.0f;

    UVRange topUV = getUVRange(3, 1, 2); // Top view
    UVRange MidUV = getUVRange(3, 1, 1); // Side view
    UVRange BotUV = getUVRange(3, 1, 0); // Bottom view

    float vertices[] = {
        // FRONT (Side view)
        -0.5f, -0.5f, -0.5f, MidUV.uMin, MidUV.vMin, // 0
        0.5f, -0.5f, -0.5f, MidUV.uMax, MidUV.vMin,  // 1
        0.5f, 0.5f, -0.5f, MidUV.uMax, MidUV.vMax,   // 2
        0.5f, 0.5f, -0.5f, MidUV.uMax, MidUV.vMax,   // 3
        -0.5f, 0.5f, -0.5f, MidUV.uMin, MidUV.vMax,  // 4
        -0.5f, -0.5f, -0.5f, MidUV.uMin, MidUV.vMin, // 5

        // BACK (Side view)
        -0.5f, -0.5f, 0.5f, MidUV.uMin, MidUV.vMin, // 6
        0.5f, -0.5f, 0.5f, MidUV.uMax, MidUV.vMin,  // 7
        0.5f, 0.5f, 0.5f, MidUV.uMax, MidUV.vMax,   // 8
        0.5f, 0.5f, 0.5f, MidUV.uMax, MidUV.vMax,   // 9
        -0.5f, 0.5f, 0.5f, MidUV.uMin, MidUV.vMax,  // 10
        -0.5f, -0.5f, 0.5f, MidUV.uMin, MidUV.vMin, // 11

        // LEFT (Side view)
        -0.5f, 0.5f, 0.5f, MidUV.uMin, MidUV.vMax,   // 12
        -0.5f, 0.5f, -0.5f, MidUV.uMax, MidUV.vMax,  // 13
        -0.5f, -0.5f, -0.5f, MidUV.uMax, MidUV.vMin, // 14
        -0.5f, -0.5f, -0.5f, MidUV.uMax, MidUV.vMin, // 15
        -0.5f, -0.5f, 0.5f, MidUV.uMin, MidUV.vMin,  // 16
        -0.5f, 0.5f, 0.5f, MidUV.uMin, MidUV.vMax,   // 17

        // RIGHT (Side view)
        0.5f, 0.5f, 0.5f, MidUV.uMin, MidUV.vMax,   // 18
        0.5f, 0.5f, -0.5f, MidUV.uMax, MidUV.vMax,  // 19
        0.5f, -0.5f, -0.5f, MidUV.uMax, MidUV.vMin, // 20
        0.5f, -0.5f, -0.5f, MidUV.uMax, MidUV.vMin, // 21
        0.5f, -0.5f, 0.5f, MidUV.uMin, MidUV.vMin,  // 22
        0.5f, 0.5f, 0.5f, MidUV.uMin, MidUV.vMax,   // 23

        // BOTTOM (Bottom view)
        -0.5f, -0.5f, -0.5f, BotUV.uMin, BotUV.vMax, // 24
        0.5f, -0.5f, -0.5f, BotUV.uMax, BotUV.vMax,  // 25
        0.5f, -0.5f, 0.5f, BotUV.uMax, BotUV.vMin,   // 26
        0.5f, -0.5f, 0.5f, BotUV.uMax, BotUV.vMin,   // 27
        -0.5f, -0.5f, 0.5f, BotUV.uMin, BotUV.vMin,  // 28
        -0.5f, -0.5f, -0.5f, BotUV.uMin, BotUV.vMax, // 29

        // TOP (Top view)
        -0.5f, 0.5f, -0.5f, topUV.uMin, topUV.vMax, // 30
        0.5f, 0.5f, -0.5f, topUV.uMax, topUV.vMax,  // 31
        0.5f, 0.5f, 0.5f, topUV.uMax, topUV.vMin,   // 32
        0.5f, 0.5f, 0.5f, topUV.uMax, topUV.vMin,   // 33
        -0.5f, 0.5f, 0.5f, topUV.uMin, topUV.vMin,  // 34
        -0.5f, 0.5f, -0.5f, topUV.uMin, topUV.vMax  // 35
    };

    // world space positions of our cubes
    cubePositions[0] = vec3(0.0f, 0.0f, 0.0f); // 1
    cubePositions[1] = vec3(2.0f, 5.0f, -15.0f);
    cubePositions[2] = vec3(-1.5f, -2.2f, -2.5f); // 3
    cubePositions[3] = vec3(-3.8f, -2.0f, -12.3f);
    cubePositions[4] = vec3(2.4f, -0.4f, -3.5f);
    cubePositions[5] = vec3(-1.7f, 3.0f, -7.5f); // 6
    cubePositions[6] = vec3(1.3f, -2.0f, -2.5f);
    cubePositions[7] = vec3(1.5f, 2.0f, -2.5f);
    cubePositions[8] = vec3(1.5f, 0.2f, -1.5f); // 9
    cubePositions[9] = vec3(-1.3f, 1.0f, -1.5f);

    // define camera
    cameraPos = vec3(0.0, 0.0, 3.0);
    cameraFront = vec3(0.0, 0.0, -1.0);
    cameraUp = vec3(0.0, 1.0, 0.0);

    loadShaders("src/shaders/VertexShader.glsl", "src/shaders/FragmentShader.glsl");

    // error checks
    checkShaderErrors();
    checkErrors();

    texture1 = loadTexture("images/GrassBlock.png"); // birinci texture

    // enable it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // upload the vertex data
    glGenBuffers(1, &VBO);                                                     // create
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                        // open
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // put data inside

    // glGenBuffers(1, &EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // upload the vertex attibute burda VBO ya vertex shader icin vertices ve indices verilerinin nasil kullanilacagini anlatir yani ilk deger iknci deger ne yapar gibi felan
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0); // fonksiyonu, VBO içindeki verinin nasıl işleneceğini OpenGL'e bildirir.
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //  unenable it
    glBindVertexArray(0);
}

void Game::gameLoop()
{
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks(); // Son güncelleme zamanını al

    while (gameState != GameState::EXIT) // bu dogru oldugu surece kod calisir.
    {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        deltaTime = (currentTime - lastFrame) / (float)SDL_GetPerformanceFrequency();
        lastFrame = currentTime; // Son zamanı güncelle

        if (deltaTime == 0.0f) // güvenlik için
            deltaTime = 0.0001f;

        while (SDL_PollEvent(&event))
        {
            handleEvents(event); // Olayları işleme.
            // player->handleInput(event); // oyuncu girislerini isle
        }

        // player->update(deltaTime);
        render(); // Ekranı güncelleme. Eğer gameState == GameState::EXIT olursa, döngü sona erer. Bu durumda oyun artık render() fonksiyonunu çağırmaz ve ekranı yenilemez.
    }
}

void Game::handleEvents(SDL_Event &event)
{
    cameraSpeed = 50.0f * deltaTime;

    switch (event.type)
    {
    case SDL_QUIT:
        gameState = GameState::EXIT;
        break;

    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
            cameraPos += cameraSpeed * cameraFront;
            break;
        case SDLK_s:
            cameraPos -= cameraSpeed * cameraFront;
            break;
        case SDLK_a:
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            break;
        case SDLK_d:
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            break;
        }
        break;

    case SDL_MOUSEMOTION:
        // Relative mouse movement kullanılıyor
        mouseCallback(window, event.motion.xrel, -event.motion.yrel); // Y yönü ters çevrildi
        break;

    case SDL_MOUSEWHEEL:
        scrollCallback(window, 0.0, event.wheel.y);
        break;
    }
}

void Game::render() // textureler arasinda alfa degeri degistirmek icin lazim parametre
{
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);            // renk ata arka plana
    glEnable(GL_DEPTH_TEST);                            // enables the depth so we can draw each sides in the true way
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // then clear it for the next frame

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); // GL_TEXTURE0 için
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    mat4 view = mat4(1.0f); // make sure to initialize matrix to identity matrix first
    view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    //            camera posizyonu  camera nereye bakiyor camera icin yukari yon neresi

    mat4 projection = mat4(1.0f); // perspective look
    projection = perspective(radians(fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

    // retrieve the matrix uniform locations
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    // pass them to the shaders (3 different ways)
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

    glBindVertexArray(VAO);

    for (unsigned int i = 0; i < 10; i++)
    {
        // calculate the model matrix for each object and pass it to shader before drawing
        mat4 model = mat4(1.0f);
        model = translate(model, cubePositions[i]);
        float angle = i == 0 ? 10.0f : 20.0f * i;
        model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f)); // ucuncu parametre hanig eksen etrafinda donecegini belirler.

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    SDL_GL_SwapWindow(window);

    std::cout << "FPS: " << 1.0f / deltaTime << std::endl;
}
// every thing we need to create a 3D cube is Projection, view, model.