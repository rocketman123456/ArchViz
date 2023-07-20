#pragma once
#include "runtime/function/render/rhi/frame_graph/frame_resource.h"

namespace ArchViz
{
    template<typename T, typename H>
    struct SimpleResourceCache
    {
        std::unordered_map<uint64_t, H> resource_map {};

        std::vector<T> resources;
    };

    struct FrameGraphNode
    {
        int32_t ref_count {0};

        // TODO: render pass handle
        uint32_t render_pass {};
        // TODO: framebuffer handle
        uint32_t framebuffer {};

        std::shared_ptr<FrameGraphRenderPass> graph_render_pass;

        std::vector<FrameGraphResourceHandle> inputs;
        std::vector<FrameGraphResourceHandle> outputs;

        // std::vector<FrameGraphNodeHandle> edges_forward;
        std::vector<FrameGraphNodeHandle> edges_backward;

        bool enabled {true};

        std::string name {};
    };

    struct FrameGraphRenderPassCache
    {
        std::unordered_map<uint64_t, FrameGraphRenderPassHandle> render_pass_map {};

        std::vector<FrameGraphRenderPass> render_passes {};
    };

    struct FrameGraphResourceCache
    {
        std::unordered_map<uint64_t, FrameGraphResourceHandle> resource_map {};

        std::vector<FrameGraphResource> resources {};
    };

    struct FrameGraphNodeCache
    {
        std::unordered_map<uint64_t, FrameGraphNodeHandle> node_map {};

        std::vector<FrameGraphNode> nodes {};
    };

    struct FrameGraphBuilder
    {
        void init(const FrameGraphCreation& info);
        void shutdown();

        FrameGraphNode* getNode(const std::string& name);
        FrameGraphNode* accessNode(FrameGraphNodeHandle handle);

        FrameGraphResource* getResource(const std::string& name);
        FrameGraphResource* accessResource(FrameGraphResourceHandle handle);

        FrameGraphResourceHandle createNodeOutput(const FrameGraphResourceOutputCreation& creation, FrameGraphNodeHandle producer);
        FrameGraphResourceHandle createNodeInput(const FrameGraphResourceInputCreation& creation);
        FrameGraphNodeHandle     createNode(const FrameGraphNodeCreation& creation);

        static constexpr uint32_t k_max_render_pass_count = 256;
        static constexpr uint32_t k_max_resources_count   = 1024;
        static constexpr uint32_t k_max_nodes_count       = 1024;

        static constexpr const char* k_name = "frame_graph_builder";

        FrameGraphRenderPassCache render_pass_cache {};
        FrameGraphResourceCache   resource_cache {};
        FrameGraphNodeCache       node_cache {};
    };

    struct FrameGraph
    {
        void init(const FrameGraphCreation& info);
        void shutdown();

        // NOTE(marco): each frame we rebuild the graph so that we can enable only the nodes we are interested in
        void reset();
        void compile();

        void computeEdges(FrameGraphNode* node, uint32_t node_index);

        void printResult();

        void render();

        FrameGraphBuilder builder {};

        // NOTE(marco): nodes sorted in topological order
        std::vector<FrameGraphNodeHandle> nodes {};

        std::string name {};
    };

    void generate_graphviz(const FrameGraph& graph, std::string& result);
} // namespace ArchViz
