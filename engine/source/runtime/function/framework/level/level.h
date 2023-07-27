#pragma once

#include "runtime/function/framework/object/object_id_allocator.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace ArchViz
{
    class GObject;
    class ObjectInstanceRes;
    class AssetManager;
    class ConfigManager;
    class ComponentManager;

    using LevelObjectsMap = std::unordered_map<GObjectID, std::shared_ptr<GObject>>;

    /// could use as ecs' entity manager
    /// The main class to manage all game objects
    class Level
    {
    public:
        virtual ~Level() {};

        bool load(const std::string& level_res_url);
        void unload();

        bool save();

        void tick(float delta_time);

        const std::string& getLevelResUrl() const { return m_level_res_url; }

        const LevelObjectsMap& getAllGObjects() const { return m_gobjects; }

        std::weak_ptr<GObject> getGObjectByID(GObjectID go_id) const;

        GObjectID createObject(const ObjectInstanceRes& object_instance_res);
        void      deleteGObjectByID(GObjectID go_id);

    protected:
        void clear();

        bool        m_is_loaded {false};
        std::string m_level_res_url;

        // all game objects in this level, key: object id, value: object instance
        // TODO : use ecs
        LevelObjectsMap m_gobjects;
    };
} // namespace ArchViz
