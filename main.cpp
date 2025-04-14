#include <Windows.h>
#include "RenderWindow.h"
#include "Geometrie.h"
#include "Transform.h"

#include <ctime>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <DirectXMath.h>

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

    float rotationSpeeds[3] = { 1.0f, 1.5f, 2.0f };
    float angles[3] = { 0.0f, 0.0f, 0.0f };
    float stopDurations[3] = { 0.0f, 0.0f, 0.0f };
    bool isStopped[3] = { true, true, true };

    bool isSpacePressed = false;
    bool isRotating = false;
    bool isStartingSpin = false;

    srand(static_cast<unsigned int>(time(nullptr)));

    std::chrono::steady_clock::time_point spinStartTime;
    static float elapsed = 0.0f;

    auto startSpin = [&]() {
        isStartingSpin = true; 
        isRotating = false;

        for (int i = 0; i < 3; ++i) {
            angles[i] = 0.0f;
            stopDurations[i] = 5.0f + static_cast<float>(rand()) / RAND_MAX * 5.0f;
            isStopped[i] = false;
        }
        };

    while (true) {
        auto now = std::chrono::steady_clock::now();

        if (isStartingSpin) {
            spinStartTime = now;
            isStartingSpin = false;
            isRotating = true;
            elapsed = 0.0f;
        }
        else {
            elapsed = std::chrono::duration<float>(now - spinStartTime).count();
        }

        window->Update();

        int event;
        while (window->PollWindowEvents(event)) {
            if (event == WM_QUIT) {
                goto cleanup;
            }
            else if (event == WM_KEYDOWN && !isSpacePressed) {
                isSpacePressed = true;

                if (isStopped[0] && isStopped[1] && isStopped[2]) {
                    startSpin();
                }
            }
            else if (event == WM_KEYUP) {
                isSpacePressed = false;
            }
        }

        if (isRotating) {
            for (int i = 0; i < 3; ++i) {
                if (!isStopped[i]) {
                    if (elapsed >= stopDurations[i]) {
                        float finalAngle = stopDurations[i] * rotationSpeeds[i];
                        angles[i] = roundf(finalAngle / DirectX::XM_PIDIV2) * DirectX::XM_PIDIV2;
                        isStopped[i] = true;
                    }
                    else {
                        angles[i] = elapsed * rotationSpeeds[i];
                    }
                    transforms[i].SetRotationYPR(XMFLOAT3{ 0, angles[i], 0 });
                }
            }

            if (isStopped[0] && isStopped[1] && isStopped[2]) {
                isRotating = false;
            }
        }

        for (int i = 0; i < 3; ++i) {
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

cleanup:
    for (int i = 0; i < 3; ++i) {
        delete cubes[i];
        delete constantBuffers[i];
    }
    delete window;

    return 0;
}
