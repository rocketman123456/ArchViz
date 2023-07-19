#pragma once
#include "runtime/function/render/rhi/frame_graph/frame_resource.h"

namespace ArchViz
{
    struct FrameGraphRenderPassCache
    {
        std::unordered_map<uint64_t, FrameGraphRenderPassHandle> render_pass_map {};

        std::vector<FrameGraphRenderPass> render_passes;
    };

    struct FrameGraphResourceCache
    {
        std::unordered_map<uint64_t, FrameGraphResourceHandle> resource_map {};

        std::vector<FrameGraphResource> resources;
    };

    struct FrameGraphNodeCache
    {
        std::unordered_map<uint64_t, FrameGraphNodeHandle> node_map {};

        std::vector<FrameGraphNode> nodes;
    };

    struct FrameGraphBuilder
    {
        void init();
        void shutdown();

        FrameGraphNode* getNode(const std::string& name);
        FrameGraphNode* accessNode(FrameGraphNodeHandle handle);

        FrameGraphResource* getResource(const std::string& name);
        FrameGraphResource* accessResource(FrameGraphNodeHandle handle);

        static constexpr uint32_t k_max_render_pass_count = 256;
        static constexpr uint32_t k_max_resources_count   = 1024;
        static constexpr uint32_t k_max_nodes_count       = 1024;

        static constexpr const char* k_name = "frame_graph_builder";

        FrameGraphRenderPassCache render_pass_cache;
        FrameGraphResourceCache   resource_cache;
        FrameGraphNodeCache       node_cache;
    };

    struct FrameGraph
    {
        void init();
        void shutdown();

        // NOTE(marco): each frame we rebuild the graph so that we can enable only the nodes we are interested in
        void reset();
        void compile();

        void printResult();

        void render();

        FrameGraphBuilder builder;

        // NOTE(marco): nodes sorted in topological order
        std::vector<FrameGraphNodeHandle> nodes;

        std::string name;
    };

    FrameGraphResourceType string_to_resource_type(const std::string& input_type);

    void generate_graphviz(const FrameGraph& graph, std::string& result);
} // namespace ArchViz
