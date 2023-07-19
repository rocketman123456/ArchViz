#include "runtime/function/render/rhi/frame_graph/frame_graph.h"

#include "runtime/core/base/hash.h"
#include "runtime/core/base/macro.h"

#include <stack>

namespace ArchViz
{
    FrameGraphResourceType string_to_resource_type(const std::string& input_type)
    {
        if (input_type.compare("texture") == 0)
        {
            return FrameGraphResourceType::Texture;
        }
        else if (input_type.compare("attachment") == 0)
        {
            return FrameGraphResourceType::Attachment;
        }
        else if (input_type.compare("buffer") == 0)
        {
            return FrameGraphResourceType::Buffer;
        }
        else if (input_type.compare("reference") == 0)
        {
            // This is used for resources that need to create an edge but are not actually used by the render pass
            return FrameGraphResourceType::Reference;
        }

        ASSERT(false);
        return FrameGraphResourceType::Invalid;
    }

    void generate_graphviz(const FrameGraph& graph, std::string& result)
    {
        result.clear();
        result += "digraph " + graph.name + "{\n";

        for (auto& node_index : graph.nodes)
        {
            auto& node = graph.builder.node_cache.nodes[node_index.index];
            for (auto& edge_index : node.edges_forward)
            {
                auto& other_node = graph.builder.node_cache.nodes[edge_index.index];
                result += "\t" + node.name + " -> " + other_node.name + ";\n";
            }
        }

        result += "}\n";
    }

    void FrameGraphBuilder::init()
    {
        // render_pass_cache.render_passes.resize(FrameGraphBuilder::k_max_render_pass_count);
        // resource_cache.resources.resize(FrameGraphBuilder::k_max_resources_count);
        // node_cache.nodes.resize(FrameGraphBuilder::k_max_nodes_count);
        LOG_DEBUG("init frame graph builder");
    }

    void FrameGraphBuilder::shutdown()
    {
        LOG_DEBUG("shutdown frame graph builder");
        render_pass_cache.render_passes.clear();
        resource_cache.resources.clear();
        node_cache.nodes.clear();
    }

    FrameGraphNode* FrameGraphBuilder::getNode(const std::string& name)
    {
        auto it = node_cache.node_map.find(Hash::u64(name));
        if (it == node_cache.node_map.end())
            return {};
        return &node_cache.nodes[it->second.index];
    }

    FrameGraphNode* FrameGraphBuilder::accessNode(FrameGraphNodeHandle handle) { return &node_cache.nodes[handle.index]; }

    FrameGraphResource* FrameGraphBuilder::getResource(const std::string& name)
    {
        auto it = resource_cache.resource_map.find(Hash::u64(name));
        if (it == resource_cache.resource_map.end())
            return {};
        return &resource_cache.resources[it->second.index];
    }

    FrameGraphResource* FrameGraphBuilder::accessResource(FrameGraphNodeHandle handle) { return &resource_cache.resources[handle.index]; }

    void FrameGraph::init()
    {
        builder.init();
        // nodes.resize(FrameGraphBuilder::k_max_nodes_count);

        for (uint32_t i = 0; i < builder.node_cache.nodes.size(); ++i)
        {
            nodes.push_back({i});
        }

        LOG_DEBUG("init frame graph builder");
    }

    void FrameGraph::shutdown()
    {
        LOG_DEBUG("shutdown frame graph builder");
        nodes.clear();
        builder.shutdown();
    }

    void FrameGraph::reset()
    {
        // nodes.clear();
        // nodes.resize(FrameGraphBuilder::k_max_nodes_count);
        LOG_DEBUG("reset frame graph");
    }

    void FrameGraph::compile()
    {
        LOG_DEBUG("start compile frame graph");

        // - check that input has been produced by a different node
        // - cull inactive nodes

        // TODO: first clear all edges
        // TODO: re-compute all edges

        std::vector<FrameGraphNodeHandle> sorted_nodes;

        std::vector<uint8_t> visited;
        visited.resize(nodes.size());
        memset(visited.data(), 0, sizeof(uint8_t) * visited.size());

        std::vector<FrameGraphNodeHandle> stack;

        for (uint32_t i = 0; i < nodes.size(); ++i)
        {
            FrameGraphNode* node = builder.accessNode(nodes[i]);

            if (!node->enabled)
                continue;

            stack.push_back(nodes[i]);

            while (stack.size() > 0)
            {
                auto& node_handle = stack.back();
                if (visited[node_handle.index] == 2)
                {
                    stack.pop_back();
                    continue;
                }

                if (visited[node_handle.index] == 1)
                {
                    visited[node_handle.index] = 2; // added
                    sorted_nodes.push_back(node_handle);
                    stack.pop_back();
                    continue;
                }

                visited[node_handle.index] = 1; // visited

                FrameGraphNode* node = builder.accessNode(node_handle);

                // Leaf node
                if (node->edges_backward.size() == 0)
                {
                    continue;
                }

                for (uint32_t r = 0; r < node->edges_backward.size(); ++r)
                {
                    FrameGraphNodeHandle child_handle = node->edges_backward[r];
                    if (!visited[child_handle.index])
                    {
                        stack.push_back(child_handle);
                    }
                }
            }
        }

        nodes.clear();
        for (auto& sort : sorted_nodes)
        {
            nodes.push_back(sort);
        }

        LOG_DEBUG("finish compile frame graph");
    }

    void FrameGraph::printResult()
    {
        std::string log = "";

        for (auto& node_index : nodes)
        {
            auto& node = builder.node_cache.nodes[node_index.index];
            log += node.name + " ";
        }

        LOG_DEBUG(log);
    }

    void FrameGraph::render()
    {
        LOG_DEBUG("start render frame graph");

        LOG_DEBUG("finish render frame graph");
    }

} // namespace ArchViz
