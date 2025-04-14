#include <Windows.h>
#include "RenderWindow.h"
#include "Geometrie.h"
#include "Transform.h"

#include <ctime>      // For srand/time
#include <cstdlib>    // For rand
#include <cmath>      // For roundf
#include <DirectXMath.h>
#include <chrono>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    RenderWindow* window = new RenderWindow(hInst);
    window->Initialize();

    Geometrie* cubes[3];
    for (int i = 0; i < 3; ++i) {
        cubes[i] = new Geometrie();
        cubes[i]->CreateCube(window->GetDevice(), window->GetCommandList(), 1.0f, 1.0f, 1.0f);
    }
    window->CloseCommandList();

    UploadBuffer<ObjectData>* constantBuffers[3];
    for (int i = 0; i < 3; ++i) {
        constantBuffers[i] = new UploadBuffer<ObjectData>(window->GetDevice(), 1, true);
        constantBuffers[i]->Resource()->SetName(L"PASS_BUFFER");
    }

    TRANSFORM transforms[3];
    transforms[0].SetPosition(XMFLOAT3{ -2.0f, 0.0f, 0.0f });
    transforms[1].SetPosition(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
    transforms[2].SetPosition(XMFLOAT3{ 2.0f, 0.0f, 0.0f });

    bool closed = false;
    float time = 0.0f;
    float rotationSpeeds[3] = { 1.0f, 1.5f, 2.0f };

    bool isSpacePressed = false;
    float speedMultiplier = 1.5f;

    float stopTimers[3] = { 0.0f, 0.0f, 0.0f };
    float stopDurations[3];
    bool isStopped[3] = { false, false, false };

    srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < 3; ++i) {
        stopDurations[i] = 5.0f + static_cast<float>(rand()) / RAND_MAX * 7.0f;
    }

    bool isRotating = false;

    while (!closed) {
        time += 0.001f;
        window->Update();

        int event;
        while (window->PollWindowEvents(event)) {
            if (event == WM_QUIT) {
                closed = true;
            }
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (!isSpacePressed) {
                    isRotating = !isRotating;
                    isSpacePressed = true;
                }
            }
        }

        for (int i = 0; i < 3; ++i) {
            if (!isStopped[i]) {
                stopTimers[i] += 0.001f;

                float angle = 0.0f;

                if (isRotating) {
                    angle = time * rotationSpeeds[i] * speedMultiplier;
                }

                if (stopTimers[i] >= stopDurations[i]) {
                    float snappedAngle = roundf(angle / DirectX::XM_PIDIV2) * DirectX::XM_PIDIV2;
                    angle = snappedAngle;
                    isStopped[i] = true;
                }

                transforms[i].SetRotationYPR(XMFLOAT3{ 0, angle, 0 });
            }

            ObjectData objConstants;
            DirectX::XMStoreFloat4x4(&objConstants.world, DirectX::XMMatrixTranspose(transforms[i].GetMatrix()));
            constantBuffers[i]->CopyData(0, objConstants);
        }

        window->BeginDraw();
        for (int i = 0; i < 3; ++i) {
            window->Draw(*window->mShader, *cubes[i], constantBuffers[i]);
        }
        window->EndDraw();
    }

    // Cleanup resources
    for (int i = 0; i < 3; ++i) {
        delete cubes[i];
        delete constantBuffers[i];
    }
    delete window;


    return 0;
}
