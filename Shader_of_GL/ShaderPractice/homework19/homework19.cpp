#include "TankScene.h"

int main()
{
    return practice::RunScene("homework19 - Tank and camera", tank::KeyCallback, tank::UpdateScene, tank::DrawAll);
}
