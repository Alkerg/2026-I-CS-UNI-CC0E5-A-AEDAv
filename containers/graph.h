#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include "../types.h"
#include "./traits.h"

using namespace std;



// Nodo de grafo
template <typename NodeTraits = DefaultNodeTraits>
class GraphNode{
public:
    using traits_type = NodeTraits;
    using id_type     = typename NodeTraits::id_type;
    using value_type  = typename NodeTraits::value_type;
    using MySelf      = GraphNode<NodeTraits>;
private:
    id_type    m_id;
    value_type m_value;
public:
    GraphNode() {
        m_id = id_type();
        m_value = value_type();
    }
    GraphNode(id_type id, value_type value){
        m_id = id;
        m_value = value;
    }
    GraphNode(const MySelf &other){
        m_id = other.m_id;
        m_value = other.m_value;
    }
    GraphNode(MySelf &&other) noexcept  {
        m_id = move(other.m_id);
        m_value = move(other.m_value);
    }
    MySelf& operator=(const MySelf &other) = default;
    MySelf& operator=(MySelf &&other) = default;

    id_type     getId() const { return m_id; }
    value_type  getValue() const { return m_value; }
    value_type& getValueRef() { return m_value; }
    void        setValue(value_type value) { m_value = value; }
};

template <typename NodeTraits>
ostream& operator<<(ostream& os, const GraphNode<NodeTraits>& node){
    return os << "(" << node.getId() << ", " << node.getValue() << ")";
}

// Arista de grafo
template <typename EdgeTraits = DefaultEdgeTraits>
class GraphEdge{
public:
    using traits_type  = EdgeTraits;
    using id_type      = typename EdgeTraits::id_type;
    using node_id_type = typename EdgeTraits::node_id_type;
    using weight_type  = typename EdgeTraits::weight_type;
    using MySelf       = GraphEdge<EdgeTraits>;
private:
    id_type      m_id;
    node_id_type m_source;
    node_id_type m_target;
    weight_type  m_weight;
public:
    GraphEdge() {
        m_id = id_type();
        m_source = node_id_type();
        m_target = node_id_type();
        m_weight = weight_type();
    }
    GraphEdge(id_type id, node_id_type source, node_id_type target, weight_type weight){
        m_id = id;
        m_source = source;
        m_target = target;
        m_weight = weight;
    }
    GraphEdge(const MySelf &other){
        m_id = other.m_id;
        m_source = other.m_source;
        m_target = other.m_target;
        m_weight = other.m_weight;
    }
    GraphEdge(MySelf &&other) noexcept{
        m_id = move(other.m_id);
        m_source = move(other.m_source);
        m_target = move(other.m_target);
        m_weight = move(other.m_weight);
    }
    MySelf& operator=(const MySelf &other) = default;
    MySelf& operator=(MySelf &&other) = default;

    id_type      getId() const { return m_id; }
    node_id_type getSource() const { return m_source; }
    node_id_type getTarget() const { return m_target; }
    weight_type  getWeight() const { return m_weight; }
    void         setWeight(weight_type weight) { m_weight = weight; }
};

template <typename EdgeTraits>
ostream& operator<<(ostream& os, const GraphEdge<EdgeTraits>& edge){
    return os<<"Edge ID="<<edge.getId()<<"\t"<<"("<<edge.getSource()<<")-"<<edge.getWeight()<<"->("<<edge.getTarget()<<")";
}

// Traits de grafo dirigido y no dirigido.
template <typename NodeTrait = DefaultNodeTraits, typename EdgeTrait = DefaultEdgeTraits>
struct DirectedGraphTrait{
    using Node = GraphNode<NodeTrait>;
    using Edge = GraphEdge<EdgeTrait>;
    static const TBool is_directed = true;
};

template <typename NodeTrait = DefaultNodeTraits, typename EdgeTrait = DefaultEdgeTraits>
struct UndirectedGraphTrait{
    using Node = GraphNode<NodeTrait>;
    using Edge = GraphEdge<EdgeTrait>;
    static const TBool is_directed = false;
};

// Grafo
template <typename GraphTraits = UndirectedGraphTrait<>>
class Graph{
public:
    using graph_traits = GraphTraits;
    using node_type    = typename GraphTraits::Node;
    using edge_type    = typename GraphTraits::Edge;
    using node_id_type = typename node_type::id_type;
    using value_type   = typename node_type::value_type;
    using edge_id_type = typename edge_type::id_type;
    using weight_type  = typename edge_type::weight_type;
    using MySelf       = Graph<GraphTraits>;

    using node_container = unordered_map<node_id_type, node_type>;
    using edge_container = unordered_map<edge_id_type, edge_type>;

private:
    node_container m_nodes;
    edge_container m_edges;
    edge_id_type   m_nextEdgeId;   // Ids de arista autogenerados
    mutable shared_mutex m_mtx;

public:
    // Iterador generico
    template <typename MapIterator, typename Value>
    class map_iterator{
        MapIterator m_it;
    public:
        map_iterator(MapIterator it) : m_it(it) {}
        map_iterator(const map_iterator &other) = default;
        map_iterator(map_iterator &&other) = default;
        Value& operator*() const { return m_it->second; }
        map_iterator& operator++() { ++m_it; return *this; }
        TBool operator==(const map_iterator &o) const { return m_it == o.m_it; }
        TBool operator!=(const map_iterator &o) const { return m_it != o.m_it; }
    };

    // Vista generica de un map
    template <typename Container, typename Iterator, typename ConstIterator>
    class map_view{
        Container& m_c;
    public:
        map_view(Container& c) : m_c(c) {}
        Iterator      begin()        { return Iterator(m_c.begin()); }
        Iterator      end()          { return Iterator(m_c.end()); }
        ConstIterator cbegin() const { return ConstIterator(m_c.cbegin()); }
        ConstIterator cend()   const { return ConstIterator(m_c.cend()); }
    };

    // Iteradores y vistas de nodos y aristas
    using node_iterator       = map_iterator<typename node_container::iterator, node_type>;
    using const_node_iterator = map_iterator<typename node_container::const_iterator, const node_type>;
    using edge_iterator       = map_iterator<typename edge_container::iterator, edge_type>;
    using const_edge_iterator = map_iterator<typename edge_container::const_iterator, const edge_type>;

    using NodesView = map_view<node_container, node_iterator, const_node_iterator>;
    using EdgesView = map_view<edge_container, edge_iterator, const_edge_iterator>;

    // Constructores
    Graph() : m_nextEdgeId(edge_id_type()) {}
    Graph(const MySelf &other) : m_nextEdgeId(edge_id_type()){
        shared_lock<shared_mutex> lockOther(other.m_mtx);
        m_nodes      = other.m_nodes;
        m_edges      = other.m_edges;
        m_nextEdgeId = other.m_nextEdgeId;
    }
    Graph(MySelf &&other) : m_nextEdgeId(edge_id_type()){
        unique_lock<shared_mutex> lockOther(other.m_mtx);
        m_nodes      = move(other.m_nodes);
        m_edges      = move(other.m_edges);
        m_nextEdgeId = other.m_nextEdgeId;
    }
    MySelf& operator=(const MySelf &other){
        if(this != &other){
            MySelf temp(other);
            unique_lock<shared_mutex> lock(m_mtx);
            swap(m_nodes, temp.m_nodes);
            swap(m_edges, temp.m_edges);
            swap(m_nextEdgeId, temp.m_nextEdgeId);
        }
        return *this;
    }
    MySelf& operator=(MySelf &&other){
        if(this != &other){
            MySelf temp(move(other));
            unique_lock<shared_mutex> lock(m_mtx);
            swap(m_nodes, temp.m_nodes);
            swap(m_edges, temp.m_edges);
            swap(m_nextEdgeId, temp.m_nextEdgeId);
        }
        return *this;
    }

    // Añadir nodos
    TBool addNode(node_id_type id, value_type value = value_type()){
        unique_lock<shared_mutex> lock(m_mtx);
        if(m_nodes.count(id) > 0)
            return false;         // el nodo ya existe
        m_nodes.insert({id, node_type(id, value)});
        return true;
    }

    // Buscar nodo
    const node_type* findNode(node_id_type id) const{
        shared_lock<shared_mutex> lock(m_mtx);
        auto it = m_nodes.find(id);
        if(it != m_nodes.end())
            return &it->second;
        return nullptr;
    }

    // Remover nodo
    TBool removeNode(node_id_type id){
        unique_lock<shared_mutex> lock(m_mtx);
        if(m_nodes.count(id) == 0)
            return false;
        m_nodes.erase(id);

        // borrar aristas incidentes (entrantes y salientes)
        vector<edge_id_type> to_delete;
        for(const auto &pair : m_edges){
            const edge_type &edge = pair.second;
            if(edge.getSource() == id || edge.getTarget() == id)
                to_delete.push_back(edge.getId());
        }
        for(TSize i = 0; i < to_delete.size(); ++i)
            m_edges.erase(to_delete[i]); 
        return true;
    }

    // Añadir arista
    edge_id_type addEdge(node_id_type source, node_id_type target, weight_type weight = weight_type()){
        unique_lock<shared_mutex> lock(m_mtx);
        if(m_nodes.count(source) == 0 || m_nodes.count(target) == 0)
            return edge_id_type(-1);               // no crear aristas colgantes
        edge_id_type id = m_nextEdgeId;
        ++m_nextEdgeId;
        m_edges.insert({id, edge_type(id, source, target, weight)});
        return id;
    }

    // Buscar arista
    const edge_type* findEdge(edge_id_type id) const{
        shared_lock<shared_mutex> lock(m_mtx);
        auto it = m_edges.find(id);
        if(it != m_edges.end())
            return &it->second;
        return nullptr;
    }

    // Remover arista
    TBool removeEdge(edge_id_type id){
        unique_lock<shared_mutex> lock(m_mtx);
        if(m_edges.count(id) == 0)
            return false;
        return m_edges.erase(id) > 0;
    }

    // Adyacencia (considera direccion)
    vector<node_id_type> getAdjacentNodes(node_id_type id) const{
        shared_lock<shared_mutex> lock(m_mtx);
        vector<node_id_type> neighbors;

        for(const auto &pair : m_edges){
            const edge_type &edge = pair.second;
            if(edge.getSource() == id)
                neighbors.push_back(edge.getTarget()); // nodos salientes
            else if(!GraphTraits::is_directed && edge.getTarget() == id)
                neighbors.push_back(edge.getSource());  // añade nodos entrantes
        }
        return neighbors;
    }

    // Contadores y estado
    TSize nodeCount() const{
        shared_lock<shared_mutex> lock(m_mtx);
        return m_nodes.size();
    }
    TSize edgeCount() const{
        shared_lock<shared_mutex> lock(m_mtx);
        return m_edges.size();
    }
    TBool empty() const{
        shared_lock<shared_mutex> lock(m_mtx);
        return m_nodes.empty();
    }
    void clear(){
        unique_lock<shared_mutex> lock(m_mtx);
        m_nodes.clear();
        m_edges.clear();
        m_nextEdgeId = edge_id_type();
    }

    // Acceso para nodos y aristas
    NodesView nodes() { return NodesView(m_nodes); }
    EdgesView edges() { return EdgesView(m_edges); }

    // Recorrido foreach propio
    template <typename Func>
    void ForEach(Func func){
        shared_lock<shared_mutex> lock(m_mtx);
        for(auto &pair : m_nodes)
            func(pair.second);
    }

    // Impresion del grafo
    TString toString() const{
        shared_lock<shared_mutex> lock(m_mtx);
        ostringstream oss;
        TBool first = true;
        for(const auto &pair : m_edges){
            const edge_type &edge = pair.second;
            if(!first)
                oss << "\n";
            first = false;
            oss<<"("<<edge.getSource()<< ","<<m_nodes.at(edge.getSource()).getValue()<<")-"
                <<edge.getWeight()<<"->("<<edge.getTarget()<<","<<m_nodes.at(edge.getTarget()).getValue()<<")";
        }
        return oss.str();
    }
};

// Imprime el grafo con el operador <<
template <typename GraphTraits>
ostream& operator<<(ostream& os, const Graph<GraphTraits>& graph){
    return os << graph.toString();
}

// Añade nodos "(id,value)" y aristas "(source,target,weight)" con operador >>
template <typename GraphTraits>
istream& operator>>(istream& is, Graph<GraphTraits>& graph){
    using node_id_type = typename Graph<GraphTraits>::node_id_type;
    using value_type   = typename Graph<GraphTraits>::value_type;
    using weight_type  = typename Graph<GraphTraits>::weight_type;

    TChar ch;
    while(is>>ch){                 // salta espacios y separadores buscando '('
        if(ch != '(')
            continue;
        TString inside;
        getline(is, inside, ')');    // contenido hasta ')'
        stringstream fields(inside);
        TString part;
        vector<TString> tokens;
        while(getline(fields, part, ','))
            tokens.push_back(part);
        
        // Caso de nodo
        if(tokens.size() == 2){
            stringstream s0(tokens[0]), s1(tokens[1]);
            node_id_type id;
            value_type value;

            // Añadir nodo
            if((s0 >> id) && (s1 >> value))
                graph.addNode(id, value);
        
        // Caso de arista
        }else if(tokens.size() == 3){
            stringstream s0(tokens[0]), s1(tokens[1]), s2(tokens[2]);
            node_id_type source, target;
            weight_type weight;

            // Añadir arista
            if((s0 >> source) && (s1 >> target) && (s2 >> weight))
                graph.addEdge(source, target, weight);
        }
    }
    return is;
}
