// https://floooh.github.io/2018/06/17/handles-vs-pointers.html
// https://giordi91.github.io/post/resourcesystem/
#pragma once

#include <string>
#include <unordered_map>

namespace ArchViz
{
    class ResourceManager
    {
    public:
        void createMesh();
        void createTexture();

        void getMesh();
        void getTexture();

    private:
    };
} // namespace ArchViz
