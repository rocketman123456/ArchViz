#include "runtime/function/render/rhi/frame_graph/frame_graph.h"

#include <filesystem>
#include <iostream>
#include <memory>

using namespace ArchViz;
using namespace std;

int main(int argc, char** argv)
{
    FrameGraph graph;
    graph.name = "test_graph";

    FrameGraphNodeCreation create_info;

    // create nodes
    {
        FrameGraphNode node_a;
        node_a.name = "A";
        node_a.edges_forward.push_back({3});
        node_a.edges_forward.push_back({5});

        FrameGraphNode node_b;
        node_b.name = "B";
        node_b.edges_forward.push_back({1});
        node_b.edges_forward.push_back({2});
        node_b.edges_backward.push_back({4});

        FrameGraphNode node_c;
        node_c.name = "C";
        node_c.edges_forward.push_back({0});
        node_c.edges_backward.push_back({3});

        FrameGraphNode node_d;
        node_d.name = "D";
        node_d.edges_forward.push_back({0});
        node_d.edges_backward.push_back({3});
        node_d.edges_backward.push_back({5});

        FrameGraphNode node_e;
        node_e.name = "E";
        node_e.edges_backward.push_back({1});
        node_e.edges_backward.push_back({2});

        FrameGraphNode node_f;
        node_f.name = "F";
        node_f.edges_forward.push_back({2});
        node_f.edges_backward.push_back({4});

        graph.builder.node_cache.nodes.push_back(node_e);
        graph.builder.node_cache.nodes.push_back(node_c);
        graph.builder.node_cache.nodes.push_back(node_d);
        graph.builder.node_cache.nodes.push_back(node_b);
        graph.builder.node_cache.nodes.push_back(node_a);
        graph.builder.node_cache.nodes.push_back(node_f);
    }

    graph.init();

    graph.compile();

    graph.printResult();

    std::string result;
    generate_graphviz(graph, result);
    cout << result << endl;

    graph.shutdown();

    return 0;
}
