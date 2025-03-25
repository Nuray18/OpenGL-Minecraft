#include "headers/Game.h" // bu yazim kendi yazdigim kodlar icin
#include <cmath>

using namespace std;

Game::Game()
{
    window = nullptr;
    gameState = GameState::PLAY;
    screenWidth = 800;
    screenHeight = 600;

    player = new Player(100, 100, 50, 60);
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

void Game::setupOpenGL()
{
    glViewport(0, 0, screenWidth, screenHeight);

    // Üçgenin köşe verileri (x, y, z)
    float vertices[] = {
        // positions         // colors
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // top right
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // top left
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3};

    // enable it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // upload the vertex data
    glGenBuffers(1, &VBO);                                                     // create
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                        // open
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // put data inside

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // upload the vertex attibute burda VBO ya vertex shader icin vertices ve indices verilerinin nasil kullanilacagini anlatir yani ilk deger iknci deger ne yapar gibi felan
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // fonksiyonu, VBO içindeki verinin nasıl işleneceğini OpenGL'e bildirir.
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // unenable it
    glBindVertexArray(0);

    loadShaders("src/shaders/VertexShader.glsl", "src/shaders/FragmentShader.glsl");
}

void Game::gameLoop()
{
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks(); // Son güncelleme zamanını al

    while (gameState != GameState::EXIT) // bu dogru oldugu surece kod calisir.
    {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // DeltaTime hesapla (saniye cinsinden)
        lastTime = currentTime;                               // Son zamanı güncelle

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
    switch (event.type)
    {
    case SDL_QUIT:                   // bu function ekrandaki x buttonuna basarak oyundan cikmagi saglar.
        gameState = GameState::EXIT; // burda gameState Exit oldugunda gameLoop functionu yanlis olur ve dongu biter.
        break;
    }
}

void Game::render()
{

    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // renk ata arka plana
    glClear(GL_COLOR_BUFFER_BIT);

    float timeValue = SDL_GetTicks() / 1000.0f * 6;
    float redValue = sin(timeValue) / 2.0f + 0.5f; // bu islem ile sin degerinin 0 ile 1 arasinda dalgali olarak sonuclar uretmesini saglariz
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUseProgram(shaderProgram);
    glUniform4f(vertexColorLocation, redValue, 0.0f, 0.0f, 0.5f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
}
