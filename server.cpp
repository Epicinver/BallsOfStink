#include <iostream>
#include <vector>
#include <thread>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

const int WIDTH = 20;
const int HEIGHT = 20;
const int PORT = 54000;

struct SnakeSegment {
    int x, y;
};

struct Snake {
    std::vector<SnakeSegment> body;
    int direction;
};

Snake snake1, snake2;
int foodX, foodY;
bool gameOver;

void Setup() {
    gameOver = false;
    snake1.body.push_back({WIDTH / 4, HEIGHT / 2});
    snake2.body.push_back({3 * WIDTH / 4, HEIGHT / 2});
    foodX = rand() % WIDTH;
    foodY = rand() % HEIGHT;
}

void MoveSnake(Snake &snake) {
    SnakeSegment newHead = snake.body[0];
    switch (snake.direction) {
        case 0: newHead.x--; break;
        case 1: newHead.x++; break;
        case 2: newHead.y--; break;
        case 3: newHead.y++; break;
    }
    snake.body.insert(snake.body.begin(), newHead);
    snake.body.pop_back();
}

void GameLogic() {
    MoveSnake(snake1);
    MoveSnake(snake2);

    if (snake1.body[0].x == foodX && snake1.body[0].y == foodY) {
        snake1.body.push_back({foodX, foodY});
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
    }

    if (snake2.body[0].x == foodX && snake2.body[0].y == foodY) {
        snake2.body.push_back({foodX, foodY});
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
    }

    if (snake1.body[0].x >= WIDTH || snake1.body[0].x < 0 || snake1.body[0].y >= HEIGHT || snake1.body[0].y < 0) gameOver = true;
    if (snake2.body[0].x >= WIDTH || snake2.body[0].x < 0 || snake2.body[0].y >= HEIGHT || snake2.body[0].y < 0) gameOver = true;

    for (int i = 1; i < snake1.body.size(); ++i) {
        if (snake1.body[i].x == snake1.body[0].x && snake1.body[i].y == snake1.body[0].y) gameOver = true;
    }

    for (int i = 1; i < snake2.body.size(); ++i) {
        if (snake2.body[i].x == snake2.body[0].x && snake2.body[i].y == snake2.body[0].y) gameOver = true;
    }
}

void ClientHandler(SOCKET clientSocket, Snake &snake) {
    char buffer[256];
    while (!gameOver) {
        int bytesReceived = recv(clientSocket, buffer, 256, 0);
        if (bytesReceived > 0) {
            int direction = buffer[0] - '0';
            snake.direction = direction;
        }
        GameLogic();
        std::string gameState = std::to_string(snake1.body[0].x) + "," + std::to_string(snake1.body[0].y) + "," +
                                std::to_string(snake2.body[0].x) + "," + std::to_string(snake2.body[0].y) + "," +
                                std::to_string(foodX) + "," + std::to_string(foodY);
        send(clientSocket, gameState.c_str(), gameState.size() + 1, 0);
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.s_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&hint, sizeof(hint));
    listen(listening, SOMAXCONN);

    SOCKET clientSocket1 = accept(listening, nullptr, nullptr);
    SOCKET clientSocket2 = accept(listening, nullptr, nullptr);

    std::thread clientThread1(ClientHandler, clientSocket1, std::ref(snake1));
    std::thread clientThread2(ClientHandler, clientSocket2, std::ref(snake2));

    clientThread1.join();
    clientThread2.join();

    WSACleanup();
    return 0;
}
