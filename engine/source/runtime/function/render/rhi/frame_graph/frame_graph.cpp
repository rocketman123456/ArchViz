#include "runtime/function/render/rhi/frame_graph/frame_graph.h"

#include "runtime/core/base/hash.h"
#include "runtime/core/base/macro.h"

#include <stack>

namespace ArchViz
{
    static FrameGraphResourceType string_to_resource_type(const std::string& input_type)
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

    static RenderPassOperation::Enum string_to_render_pass_operation(const std::string& op)
    {
        if (op.compare("VK_ATTACHMENT_LOAD_OP_CLEAR") == 0)
        {
            return RenderPassOperation::Clear;
        }
        else if (op.compare("VK_ATTACHMENT_LOAD_OP_LOAD") == 0)
        {
            return RenderPassOperation::Load;
        }

        ASSERT(false);
        return RenderPassOperation::DontCare;
    }

    void generate_graphviz(const FrameGraph& graph, std::string& result)
    {
        result.clear();
        result += "digraph " + graph.name + "{\n";

        for (auto& node_index : graph.nodes)
        {
            auto& node = graph.builder.node_cache.nodes[node_index.index];
            for (auto& edge_index : node.edges_backward)
            {
                auto& other_node = graph.builder.node_cache.nodes[edge_index.index];
                result += "\t" + other_node.name + " -> " + node.name + ";\n";
            }
        }

        result += "}\n";
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    FrameGraphNode* FrameGraphBuilder::getNode(const std::string& name)
    {
        auto it = node_cache.node_map.find(Hash::u64(name));
        if (it == node_cache.node_map.end())
            return {};
        return &node_cache.nodes[it->second.index];
    }

    FrameGraphNode* FrameGraphBuilder::accessNode(FrameGraphNodeHandle handle)
    {
        if (handle.index == k_invalid_index)
            return nullptr;
        return &node_cache.nodes[handle.index];
    }

    FrameGraphResource* FrameGraphBuilder::getResource(const std::string& name)
    {
        auto it = resource_cache.resource_map.find(Hash::u64(name));
        if (it == resource_cache.resource_map.end())
            return {};
        return &resource_cache.resources[it->second.index];
    }

    FrameGraphResource* FrameGraphBuilder::accessResource(FrameGraphResourceHandle handle)
    {
        if (handle.index == k_invalid_index)
            return nullptr;
        return &resource_cache.resources[handle.index];
    }

    FrameGraphResourceHandle FrameGraphBuilder::createNodeOutput(const FrameGraphResourceOutputCreation& creation, FrameGraphNodeHandle producer)
    {
        auto it = resource_cache.resource_map.find(Hash::u64(creation.name));
        if (it == resource_cache.resource_map.end())
        {
            // create resource
            FrameGraphResource resource;
            resource.name          = creation.name;
            resource.producer      = producer;
            resource.output_handle = {static_cast<uint32_t>(resource_cache.resources.size())};

            resource_cache.resources.push_back(resource);
            resource_cache.resource_map[Hash::u64(creation.name)] = resource.output_handle;

            return resource.output_handle;
        }
        else
        {
            auto resource = accessResource(it->second);
            if (resource->producer.index == k_invalid_index)
            {
                resource->producer = producer;
            }
        }
        return it->second;
    }

    FrameGraphResourceHandle FrameGraphBuilder::createNodeInput(const FrameGraphResourceInputCreation& creation)
    {
        auto it = resource_cache.resource_map.find(Hash::u64(creation.name));
        if (it == resource_cache.resource_map.end())
        {
            // create resource
            FrameGraphResource resource;
            resource.name          = creation.name;
            resource.producer      = {k_invalid_index};
            resource.output_handle = {static_cast<uint32_t>(resource_cache.resources.size())};

            resource_cache.resources.push_back(resource);
            resource_cache.resource_map[Hash::u64(creation.name)] = resource.output_handle;

            return resource.output_handle;
        }
        return it->second;
    }

    FrameGraphNodeHandle FrameGraphBuilder::createNode(const FrameGraphNodeCreation& creation)
    {
        auto it = node_cache.node_map.find(Hash::u64(creation.name));
        if (it == node_cache.node_map.end())
        {
            // create resource
            FrameGraphNode node;
            node.name = creation.name;

            node_cache.node_map[Hash::u64(creation.name)] = {static_cast<uint32_t>(node_cache.nodes.size())};
            node_cache.nodes.push_back(node);

            return node_cache.node_map[Hash::u64(creation.name)];
        }
        return it->second;
    }

    void FrameGraphBuilder::init(const FrameGraphCreation& info)
    {
        auto& node_infos = info.nodes;

        for (auto& info : node_infos)
        {
            // create nodes
            FrameGraphNodeHandle node_handle = createNode(info);

            auto& inputs  = info.inputs;
            auto& outputs = info.outputs;

            auto node = accessNode(node_handle);
            ASSERT(node);

            // TODO : add resource ref count
            // create input resources
            for (auto& input : inputs)
            {
                auto handle = createNodeInput(input);
                node->inputs.push_back(handle);
            }
            // create output resources
            for (auto& output : outputs)
            {
                auto handle = createNodeOutput(output, node_handle);
                node->outputs.push_back(handle);
            }
        }

        LOG_DEBUG("init frame graph builder");
    }

    void FrameGraphBuilder::shutdown()
    {
        LOG_DEBUG("shutdown frame graph builder");
        render_pass_cache.render_passes.clear();
        resource_cache.resources.clear();
        node_cache.nodes.clear();
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    void FrameGraph::init(const FrameGraphCreation& info)
    {
        name = info.name;

        builder.init(info);

        for (uint32_t i = 0; i < builder.node_cache.nodes.size(); ++i)
        {
            nodes.push_back({i});
        }

        LOG_DEBUG("init frame graph builder");
    }

    void FrameGraph::shutdown()
    {
        LOG_DEBUG("shutdown frame graph builder");
        builder.shutdown();
        nodes.clear();
    }

    void FrameGraph::reset()
    {
        for (uint32_t i = 0; i < builder.node_cache.nodes.size(); ++i)
        {
            nodes[i].index = i;
        }
        LOG_DEBUG("reset frame graph");
    }

    void FrameGraph::computeEdges(FrameGraphNode* node, uint32_t node_index)
    {
        auto& inputs = node->inputs;
        for (auto& input : inputs)
        {
            auto resource = builder.accessResource(input);
            if (resource != nullptr && resource->producer.index != k_invalid_index)
            {
                node->edges_backward.push_back(resource->producer);
            }
        }
    }

    void FrameGraph::compile()
    {
        LOG_DEBUG("start compile frame graph");

        // - check that input has been produced by a different node
        // - cull inactive nodes

        // first clear all edges
        for (uint32_t i = 0; i < nodes.size(); ++i)
        {
            FrameGraphNode* node = builder.accessNode(nodes[i]);

            // NOTE(marco): we want to clear all edges first, then populate them. If we clear them inside the loop
            // below we risk clearing the list after it has already been used by one of the child nodes
            node->edges_backward.clear();
        }

        // compute all edges
        for (uint32_t i = 0; i < nodes.size(); ++i)
        {
            FrameGraphNode* node = builder.accessNode(nodes[i]);

            if (!node->enabled)
                continue;

            computeEdges(node, i);
        }

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
                    continue;

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

        // create graph resource
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
